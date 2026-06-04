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
 * Harness driver for Assignment-3 abstract interpretation.
 *
 * Owns all `AbstractExecution::*` method bodies that are not student TODOs:
 * the analysis driver (analyse / runOnModule), ICFG / WTO traversal
 * (handleFunction / handleICFGNode), call-site dispatch (handleCallSite /
 * handleStubFunctions / handleCheckpointStubs), the abstract-state helpers
 * forwarding to AbstractInterpretation, and the validators.
 *
 * Pure bug-reporting concerns (AEReporter class + JSON summary) live in
 * AEReporter.cpp.  Student TODOs live in Assignment_3.cpp.
 */

#include "Assignment_3.h"
// harness-only: the abstract-state helpers and the post-trace accessor need
// the full AbstractInterpretation definition.  Student code (Assignment_3.cpp)
// never includes this header, so it cannot reach AbsExtAPI / getUtils.
#include "AE/Svfexe/AbstractInterpretation.h"
#include "WPA/Andersen.h"
#include <sstream>

using namespace SVF;

// ---------------------------------------------------------------------------
// Bug-reporter thin wrappers — turn an ICFG node into an AEException and
// dispatch through the AEReporter facade.
// ---------------------------------------------------------------------------

void AbstractExecution::reportBufOverflow(const ICFGNode* node) {
	AEException bug(node->toString());
	bugReporter.addBugToReporter("buffer-overflow", bug, node);
}

void AbstractExecution::reportNullDeref(const ICFGNode* node) {
	AEException bug(node->toString());
	bugReporter.addBugToReporter("nullptr-deref", bug, node);
}

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

void AbstractExecution::runOnModule(SVF::ICFG* _icfg) {
	svfir = PAG::getPAG();
	icfg = _icfg;
	analyse();
	if (!bugReporter.getCaseConfig().emitJson)
		bugReporter.printReport();
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
/// Additionally requires that the number of reported bugs is at least the
/// number of UNSAFE_* stubs in the program.
void AbstractExecution::ensureAllAssertsValidated() {
	static const Set<std::string> kAssertStubs = {"svf_assert", "svf_assert_eq"};
	static const Set<std::string> kCheckpointStubs = {
	    "UNSAFE_PTRDEREF", "SAFE_PTRDEREF",
	    "UNSAFE_BUFACCESS", "SAFE_BUFACCESS"};
	u32_t unsafe_to_be_verified = 0;
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
		if (name.rfind("UNSAFE_", 0) == 0)
			unsafe_to_be_verified++;
		if (!bugReporter.isAssertionPoint(call)) {
			std::stringstream ss;
			ss << "The stub function callsite (" << name
			   << ") was not reached by the student's control flow: "
			   << call->toString();
			std::cerr << ss.str() << std::endl;
			assert(false);
		}
	}

	assert(unsafe_to_be_verified <= bugReporter.getBugReporter().getBugSet().size() &&
		       "The number of UNSAFE_* stubs (ground truth) should <= the number of bugs reported");
}

void AbstractExecution::analyse() {
	initWTO();
	// AbstractStateManager was folded into AbstractInterpretation upstream
	// (the AE/Svfexe/AbstractStateManager.h header was removed).  Use the
	// AbstractInterpretation singleton; it pulls SVFIR from PAG::getPAG()
	// internally and does not need an explicit Andersen analysis to be passed
	// in.
	ai = &AbstractInterpretation::getAEInstance();

	handleGlobalNode();

	if (const FunObjVar* fun = svfir->getFunObjVar("main")) {
		// arguments of main are initialised as \top to represent all possible inputs
		for (u32_t i = 0; i < fun->arg_size(); ++i) {
			AbstractState& as = getAbsStateFromTrace(icfg->getGlobalICFGNode());
			as[fun->getArg(i)->getId()] = IntervalValue::top();
		}
		assert(svfir->getFunObjVar("main") != nullptr && "Main function not found");
		handleFunction(svfir->getICFG()->getFunEntryICFGNode(svfir->getFunObjVar("main")));
	}
}

// ---------------------------------------------------------------------------
// Ground-truth helpers used by handleCheckpointStubs.  Computed from SVF
// primitives only so the stub verdict cannot be biased by student bugs.
// ---------------------------------------------------------------------------

namespace {
bool harnessSafeAccess(AbstractState& as, SVFIR* svfir, const ValVar* value,
                       const IntervalValue& len) {
	AbstractValue ptrVal = as[value->getId()];
	if (!ptrVal.isAddr())
		return true;
	for (const auto& addr : ptrVal.getAddrs()) {
		if (AbstractState::isBlackHoleObjAddr(addr) || AbstractState::isNullMem(addr))
			continue;
		NodeID objId = as.getIDFromAddr(addr);
		const BaseObjVar* baseObj = svfir->getBaseObject(objId);
		if (!baseObj || baseObj->isBlackHoleObj() || !baseObj->isConstantByteSize())
			continue;
		u32_t size = baseObj->getByteSizeOfObj();
		IntervalValue baseOffset(0);
		const SVFVar* svfVar = svfir->getGNode(objId);
		if (auto* gepObj = SVFUtil::dyn_cast<GepObjVar>(svfVar))
			baseOffset = IntervalValue((s64_t)gepObj->getConstantFieldIdx());
		IntervalValue offset = baseOffset + len;
		if (offset.ub().getIntNumeral() >= (s64_t)size)
			return false;
	}
	return true;
}

bool harnessSafeDeref(AbstractState& as, const ValVar* value) {
	if (!value || value->getId() == IRGraph::NullPtr)
		return false;
	const AbstractValue& absVal = as[value->getId()];
	if (!absVal.isAddr())
		return true;
	for (const auto& addr : absVal.getAddrs()) {
		if (AbstractState::isBlackHoleObjAddr(addr))
			continue;
		if (AbstractState::isNullMem(addr))
			return false;
		if (as.isFreedMem(addr))
			return false;
	}
	return true;
}
} // namespace

void AbstractExecution::handleCallSite(const CallICFGNode* callNode) {
	const FunObjVar* callee = callNode->getCalledFunction();
	if (!callee)
		return;
	std::string fun_name = callee->getName();
	if (fun_name == "svf_assert" || fun_name == "svf_assert_eq") {
		handleStubFunctions(callNode);
	}
	else if (fun_name == "SAFE_BUFACCESS" || fun_name == "UNSAFE_BUFACCESS" ||
	         fun_name == "SAFE_PTRDEREF" || fun_name == "UNSAFE_PTRDEREF") {
		// Ground-truth checkpoints for the buffer/nullptr checkers.
		handleCheckpointStubs(callNode);
	}
	else if (fun_name == "nd" || fun_name == "rand") {
		NodeID lhsId = callNode->getRetICFGNode()->getActualRet()->getId();
		postAbsTrace()[callNode][lhsId] = AbstractValue(IntervalValue::top());
	}
	else if (SVFUtil::isExtCall(callee)) {
		// External API value summaries.  Student implements the memory and
		// string families plus assignment-specific stubs in updateStateOnExtCall;
		// unmodelled functions fall back to SVF inside that dispatcher.  Run the
		// bug checkers on the API's pointer/length arguments afterwards.
		updateStateOnExtCall(callNode);
		nullptrDerefDetection(callNode);
		bufOverflowDetection(callNode);
	}
	else {
		// Skip recursive callsites (within the same call-graph SCC): the
		// interprocedural WTO built in initWTO() already encoded this as a
		// back-edge, so the outer cycle's widen/narrow iteration in
		// handleICFGCycle drives the recursion to a fixpoint.  Mirrors
		// SVF's `AbstractInterpretation::skipRecursiveCall`.
		const FunObjVar* caller = callNode->getCaller();
		if (caller && ander && ander->inSameCallGraphSCC(caller, callee))
			return;
		handleFunction(svfir->getICFG()->getFunEntryICFGNode(callee));
		const RetICFGNode* retNode = callNode->getRetICFGNode();
		if (postAbsTrace().count(callNode))
			postAbsTrace()[retNode] = postAbsTrace()[callNode];
	}
}

/// Validate the SAFE/UNSAFE checkpoint stub functions.  Validation uses the
/// harness-only `harnessSafeAccess` / `harnessSafeDeref` helpers, NOT the
/// student's `canSafelyAccessMemory` / `canSafelyDerefPtr` — so the stub
/// verdict cannot be biased by student bugs.
void AbstractExecution::handleCheckpointStubs(const CallICFGNode* callNode) {
	bugReporter.noteAssertionPoint(callNode);
	const std::string fun_name = callNode->getCalledFunction()->getName();
	if (fun_name == "SAFE_BUFACCESS" || fun_name == "UNSAFE_BUFACCESS") {
		if (callNode->arg_size() < 2)
			return;
		AbstractState& as = getAbsStateFromTrace(callNode);
		IntervalValue len = as[callNode->getArgument(1)->getId()].getInterval();
		if (len.isBottom())
			len = IntervalValue(0);
		const ValVar* ptr = callNode->getArgument(0);
		if (!harnessSafeAccess(as, svfir, ptr, len - IntervalValue(1)))
			reportBufOverflow(callNode);
	}
	else if (fun_name == "SAFE_PTRDEREF" || fun_name == "UNSAFE_PTRDEREF") {
		if (callNode->arg_size() < 1)
			return;
		AbstractState& as = getAbsStateFromTrace(callNode);
		const ValVar* ptr = callNode->getArgument(0);
		if (!harnessSafeDeref(as, ptr))
			reportNullDeref(callNode);
	}
}

/// Handle the abstract-state assertion stubs.  `svf_assert(expr)` requires the
/// expression to hold true; `svf_assert_eq(a, b)` requires the two intervals
/// to be equal.  Both record the call site in `assert_points` so
/// `ensureAllAssertsValidated` can verify coverage.
void AbstractExecution::handleStubFunctions(const SVF::CallICFGNode* callNode) {
	if (callNode->getCalledFunction()->getName() == "svf_assert") {
		bugReporter.noteAssertionPoint(callNode);
		u32_t arg0 = callNode->getArgument(0)->getId();
		AbstractState& as = getAbsStateFromTrace(callNode);

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
		u32_t arg0 = callNode->getArgument(0)->getId();
		u32_t arg1 = callNode->getArgument(1)->getId();
		AbstractState& as = getAbsStateFromTrace(callNode);
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
// Abstract-state helpers — wrap operations on the underlying
// AbstractInterpretation singleton.  Defined here (not in the header) so
// student code never sees AbstractInterpretation/AbsExtAPI directly.
// ===========================================================================
namespace SVF {

const AbstractValue& AbstractExecution::getAbsValue(const ValVar* var, const ICFGNode* node) {
	return ai->getAbsValue(var, node);
}
const AbstractValue& AbstractExecution::getAbsValue(const ObjVar* var, const ICFGNode* node) {
	return ai->getAbsValue(var, node);
}
const AbstractValue& AbstractExecution::getAbsValue(const SVFVar* var, const ICFGNode* node) {
	return ai->getAbsValue(var, node);
}
void AbstractExecution::updateAbsValue(const ValVar* var, const AbstractValue& val, const ICFGNode* node) {
	ai->updateAbsValue(var, val, node);
}
void AbstractExecution::updateAbsValue(const ObjVar* var, const AbstractValue& val, const ICFGNode* node) {
	ai->updateAbsValue(var, val, node);
}
void AbstractExecution::updateAbsValue(const SVFVar* var, const AbstractValue& val, const ICFGNode* node) {
	ai->updateAbsValue(var, val, node);
}
AbstractValue AbstractExecution::loadValue(const ValVar* pointer, const ICFGNode* node) {
	return ai->loadValue(pointer, node);
}
void AbstractExecution::storeValue(const ValVar* pointer, const AbstractValue& val, const ICFGNode* node) {
	ai->storeValue(pointer, val, node);
}
AddressValue AbstractExecution::getGepObjAddrs(const ValVar* pointer, IntervalValue offset) {
	return ai->getGepObjAddrs(pointer, offset);
}
IntervalValue AbstractExecution::getGepElementIndex(const GepStmt* gep) {
	return ai->getGepElementIndex(gep);
}
IntervalValue AbstractExecution::getGepByteOffset(const GepStmt* gep) {
	return ai->getGepByteOffset(gep);
}
u32_t AbstractExecution::getAllocaInstByteSize(const AddrStmt* addr) {
	return ai->getAllocaInstByteSize(addr);
}

// harness-only post-trace accessors (need full AbstractInterpretation type).
AbstractState& AbstractExecution::getAbsStateFromTrace(const ICFGNode* node) {
	return (*ai)[node];
}
Map<const ICFGNode*, AbstractState>& AbstractExecution::postAbsTrace() {
	return ai->getTrace();
}

} // namespace SVF
