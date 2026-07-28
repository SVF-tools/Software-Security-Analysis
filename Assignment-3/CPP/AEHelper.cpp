//===- AEHelper.cpp -- Abstract Interpretation harness --//
//
//                     SVF: Static Value-Flow Analysis
//
// Copyright (C) <2013-2022>  <Yulei Sui>
//

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Affero General Public License for more details.

// You should have received a copy of the GNU Affero General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//===----------------------------------------------------------------------===//
/*
 * Harness for Assignment-3 abstract interpretation.
 *
 * Owns the harness-side `AbstractExecution::*` methods that students don't
 * design:
 *   - Interprocedural WTO construction   (initWTO)
 *   - Stub / checkpoint sub-dispatch     (handleStubFunctions /
 *                                         handleCheckpointStubs) — invoked
 *                                         from the student's handleCallSite
 *                                         in Assignment_3.cpp.
 *   - External-API whitelist             (isExternalCallForAssignment)
 *   - Abstract-state helpers             (getAbsValue / updateAbsValue /
 *                                         loadValue / storeValue / GEP* /
 *                                         getAEState / postAbsTrace)
 *   - Validator                          (ensureAllAssertsValidated)
 *
 * Pure bug-reporting concerns (AEReporter class + JSON / coverage summary)
 * live in AEReporter.cpp.  The analysis driver (runOnModule / analyse /
 * handleCallSite / reportBufOverflow / reportNullDeref) and the six student
 * tasks live in Assignment_3.cpp.
 */

#include "Assignment_3.h"
#include "AEState.h"
#include "WPA/Andersen.h"
#include <sstream>

using namespace SVF;

/// Whitelist of external-call names the assignment expects students to model
/// in `updateStateOnExtCall`.  Covers:
///   - Assignment-specific stubs:        `mem_insert`, `str_insert`
///   - Memory family:                    `memcpy`, `memmove`, `memset`
///   - String family:                    `strcpy`, `strncpy`, `strcat`,
///                                       `strncat`, `strlen`, `wcslen`
///   - Ground-truth checkpoint stubs:    `SAFE_/UNSAFE_BUFACCESS`,
///                                       `SAFE_/UNSAFE_PTRDEREF`
///
/// The library APIs are matched by substring because Clang emits the memory
/// family as LLVM intrinsics (e.g. `llvm.memcpy.p0.p0.i64`) and the substring
/// is preserved in the mangled name.
bool AbstractExecution::isExternalCallForAssignment(const SVF::FunObjVar* func) {
	const std::string& name = func->getName();
	static const Set<std::string> exactStubs = {
	    "mem_insert", "str_insert",
	    "UNSAFE_BUFACCESS", "SAFE_BUFACCESS",
	    "UNSAFE_PTRDEREF", "SAFE_PTRDEREF"};
	if (exactStubs.count(name))
		return true;
	static const std::vector<std::string> apiSubstrings = {
	    "memcpy", "memmove", "memset",
	    "strcpy", "strncpy", "strcat", "strncat",
	    "strlen", "wcslen"};
	for (const auto& key : apiSubstrings) {
		if (name.find(key) != std::string::npos)
			return true;
	}
	return false;
}

// ---------------------------------------------------------------------------
// WTO construction.  Each (mutually) recursive function's entry node becomes
// a WTO cycle head because intra-SCC call edges are turned into back-edges.
// The same widening/narrowing machinery used for loops then drives recursion
// to a fixpoint via handleICFGCycle; recursive callsites are filtered out in
// handleCallSite via `inSameCallGraphSCC`.
// ---------------------------------------------------------------------------

void AbstractExecution::initWTO() {
	ander = AndersenWaveDiff::createAndersenWaveDiff(svfir);
	Andersen::CallGraphSCC* callGraphScc = ander->getCallGraphSCC();
	callGraphScc->find();
	auto callGraph = ander->getCallGraph();

	for (auto it = callGraph->begin(); it != callGraph->end(); ++it) {
		const FunObjVar* fun = it->second->getFunction();
		if (fun->isDeclaration())
			continue;

		NodeID repNodeId = callGraphScc->repNode(it->second->getId());
		const NodeBS& cgSCCNodes = callGraphScc->subNodes(repNodeId);

		bool isEntry = it->second->getInEdges().empty();
		for (auto inEdge : it->second->getInEdges())
			if (!cgSCCNodes.test(inEdge->getSrcID()))
				isEntry = true;
		if (!isEntry)
			continue;

		Set<const FunObjVar*> funcScc;
		for (const auto& node : cgSCCNodes)
			funcScc.insert(callGraph->getGNode(node)->getFunction());

		auto* wto = new ICFGWTO(icfg->getFunEntryICFGNode(fun), funcScc);
		wto->init();
		funcToWTO[fun] = wto;
	}
}

/// Verify that every ground-truth stub call site was reached by the student's
/// analysis (added to `assert_points` via handleCallSite -> handleStubFunctions
/// / handleCheckpointStubs).  A missed stub site means the student's
/// control-flow logic skipped a place the grader cares about.
///
/// Recognised stubs:
///   - svf_assert / svf_assert_eq         : abstract-state assertion checks
///   - UNSAFE_PTRDEREF / SAFE_PTRDEREF    : null-deref ground truth
///   - UNSAFE_BUFACCESS / SAFE_BUFACCESS  : buffer-access ground truth
///
/// Additionally requires exact report counts for each checkpoint bug kind.
void AbstractExecution::ensureAllAssertsValidated() {
	static const Set<std::string> kAssertStubs = {"svf_assert", "svf_assert_eq"};
	static const Set<std::string> kCheckpointStubs = {
	    "UNSAFE_PTRDEREF", "SAFE_PTRDEREF",
	    "UNSAFE_BUFACCESS", "SAFE_BUFACCESS"};
	for (auto it = svfir->getICFG()->begin(); it != svfir->getICFG()->end(); ++it) {
		const ICFGNode* node = it->second;
		const CallICFGNode* call = SVFUtil::dyn_cast<CallICFGNode>(node);
		if (!call)
			continue;
		const FunObjVar* fun = call->getCalledFunction();
		if (!fun)
			continue;
		const std::string& name = fun->getName();
		const bool isAssertStub = kAssertStubs.count(name) > 0;
		const bool isCheckpointStub = kCheckpointStubs.count(name) > 0;
		if (!isAssertStub && !isCheckpointStub)
			continue;
		if (!bugReporter.isAssertionPoint(call)) {
			std::stringstream ss;
			ss << "The stub function callsite (" << name
			   << ") was not reached by the student's control flow: "
			   << call->toString();
			std::cerr << ss.str() << std::endl;
			assert(false);
		}
	}

	static const std::vector<std::string> kReportKinds = {
	    "buffer-overflow", "nullptr-deref"};
	for (const std::string& kind : kReportKinds) {
		const u32_t expected = bugReporter.getExpectedReportCount(kind);
		const u32_t actual = bugReporter.getReportCount(kind);
		if (actual != expected) {
			std::cerr << "Assignment 3 report-count mismatch for " << kind
			          << ": expected " << expected << ", got " << actual
			          << std::endl;
			assert(false && "Assignment 3 report count did not match checkpoints");
		}
	}
}

/// Record SAFE/UNSAFE checkpoint expectations. The checkpoint does not inspect
/// abstract state or emit a bug; only the student's checker may report one.
void AbstractExecution::handleCheckpointStubs(const CallICFGNode* callNode) {
	bugReporter.noteAssertionPoint(callNode);
	const std::string fun_name = callNode->getCalledFunction()->getName();
	if (fun_name == "UNSAFE_BUFACCESS")
		bugReporter.noteExpectedReport("buffer-overflow");
	else if (fun_name == "UNSAFE_PTRDEREF")
		bugReporter.noteExpectedReport("nullptr-deref");
}

/// Handle the abstract-state assertion stubs.  `svf_assert(expr)` requires the
/// expression to hold true; `svf_assert_eq(a, b)` requires the two intervals
/// to be equal.  Both record the call site in `assert_points` so
/// `ensureAllAssertsValidated` can verify coverage.
void AbstractExecution::handleStubFunctions(const SVF::CallICFGNode* callNode) {
	if (callNode->getCalledFunction()->getName() == "svf_assert") {
		bugReporter.noteAssertionPoint(callNode);
		u32_t arg0 = callNode->getArgument(0)->getId();
		AEState& as = getAEState(callNode);

		if (as[arg0].getInterval().is_infinite()) {
			SVFUtil::errs() << "svf_assert Fail. " << callNode->toString() << "\n";
			assert(false);
		}
		else {
			if (as[arg0].getInterval().equals(IntervalValue(1, 1))) {
				std::stringstream ss;
				ss << "The assertion (" << callNode->toString() << ")"
				   << " is successfully verified!!\n";
				SVFUtil::outs() << ss.str() << std::endl;
			}
			else {
				std::stringstream ss;
				ss << "The assertion (" << callNode->toString() << ")"
				   << " is unsatisfiable!!\n";
				SVFUtil::outs() << ss.str() << std::endl;
				assert(false);
			}
		}
		return;
	}
	else if (callNode->getCalledFunction()->getName() == "svf_assert_eq")  {
		bugReporter.noteAssertionPoint(callNode);
		u32_t arg0 = callNode->getArgument(0)->getId();
		u32_t arg1 = callNode->getArgument(1)->getId();
		AEState& as = getAEState(callNode);
		if (as[arg0].getInterval().equals(as[arg1].getInterval())) {
			SVFUtil::errs() << SVFUtil::sucMsg("The assertion is successfully verified!!\n");
		}
		else {
			SVFUtil::errs() << "svf_assert_eq Fail. " << callNode->toString() << "\n";
			assert(false);
		}
		return;
	}
}

// ===========================================================================
// Abstract-state helpers owned by Assignment-3.
// ===========================================================================
namespace SVF {

const AbstractValue& AbstractExecution::getAbsValue(const ValVar* var, const ICFGNode* node) {
	return getAEState(node).getAbsValue(var);
}
const AbstractValue& AbstractExecution::getAbsValue(const ObjVar* var, const ICFGNode* node) {
	return getAEState(node).getAbsValue(var);
}
const AbstractValue& AbstractExecution::getAbsValue(const SVFVar* var, const ICFGNode* node) {
	return getAEState(node).getAbsValue(var);
}
void AbstractExecution::updateAbsValue(const ValVar* var, const AbstractValue& val, const ICFGNode* node) {
	getAEState(node).updateAbsValue(var, val);
}
void AbstractExecution::updateAbsValue(const ObjVar* var, const AbstractValue& val, const ICFGNode* node) {
	getAEState(node).updateAbsValue(var, val);
}
void AbstractExecution::updateAbsValue(const SVFVar* var, const AbstractValue& val, const ICFGNode* node) {
	getAEState(node).updateAbsValue(var, val);
}
AbstractValue AbstractExecution::loadValue(const ValVar* pointer, const ICFGNode* node) {
	return getAEState(node).loadValue(pointer);
}
void AbstractExecution::storeValue(const ValVar* pointer, const AbstractValue& val, const ICFGNode* node) {
	getAEState(node).storeValue(pointer, val);
}
AddressValue AbstractExecution::getGepObjAddrs(const ValVar* pointer, IntervalValue offset) {
	return getAEState(pointer->getICFGNode()).getGepObjAddrs(pointer, offset);
}
IntervalValue AbstractExecution::getGepElementIndex(const GepStmt* gep) {
	return getAEState(gep->getICFGNode()).getGepElementIndex(gep);
}
IntervalValue AbstractExecution::getGepByteOffset(const GepStmt* gep) {
	return getAEState(gep->getICFGNode()).getGepByteOffset(gep);
}
u32_t AbstractExecution::getAllocaInstByteSize(const AddrStmt* addr) {
	return getAEState(addr->getICFGNode()).getAllocaInstByteSize(addr);
}

/// CallPE is phi-like: the formal parameter joins the caller-side value from
/// every call site represented by the statement.
void AbstractExecution::updateStateOnCall(const CallPE* callPE) {
	AEState& state = getAEState(callPE->getICFGNode());
	AbstractValue joined;
	for (u32_t index = 0; index < callPE->getOpVarNum(); ++index) {
		const ICFGNode* callNode = callPE->getOpCallICFGNode(index);
		if (postAbsTrace.count(callNode))
			joined.join_with(postAbsTrace[callNode][callPE->getOpVarID(index)]);
	}
	state[callPE->getResID()] = joined;
}

// Assignment-3-owned post-trace accessors.
AEState& AbstractExecution::getAEState(const ICFGNode* node) {
	return postAbsTrace[node];
}

} // namespace SVF
