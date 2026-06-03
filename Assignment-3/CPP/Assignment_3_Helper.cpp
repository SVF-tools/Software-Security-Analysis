//===- Assignment_3_Helper.cpp -- Abstract Interpretation --//
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
 * Helper Functions for Abstract Interpretation and buffer overflow detection
 *
 * Created on: Feb 19, 2024
 */

#include "Assignment_3.h"
// harness-only: the facade implementation and the post-trace accessor need the
// full AbstractInterpretation definition. Student code (Assignment_3.cpp) never
// includes this header, so it cannot reach AbsExtAPI / getUtils.
#include "AE/Svfexe/AbstractInterpretation.h"
#include "WPA/Andersen.h"
#include <iomanip>
#include <sstream>

using namespace SVF;

std::string SVF::ass3JsonEscape(const std::string& input) {
	std::ostringstream os;
	for (char ch : input) {
		switch (ch) {
		case '"': os << "\\\""; break;
		case '\\': os << "\\\\"; break;
		case '\b': os << "\\b"; break;
		case '\f': os << "\\f"; break;
		case '\n': os << "\\n"; break;
		case '\r': os << "\\r"; break;
		case '\t': os << "\\t"; break;
		default:
			if (static_cast<unsigned char>(ch) < 0x20) {
				os << "\\u" << std::hex << std::setw(4) << std::setfill('0')
				   << static_cast<int>(static_cast<unsigned char>(ch))
				   << std::dec << std::setfill(' ');
			}
			else {
				os << ch;
			}
		}
	}
	return os.str();
}

static std::string ass3BaseName(const std::string& path) {
	size_t slash = path.find_last_of("/\\");
	if (slash == std::string::npos)
		return path;
	return path.substr(slash + 1);
}

static bool ass3ReportMatchesTarget(const AssignmentBugReport& report,
                                    const std::string& target) {
	if (target.empty())
		return false;
	if (report.location.find(target) != std::string::npos ||
	    report.message.find(target) != std::string::npos)
		return true;

	size_t colon = target.rfind(':');
	if (colon == std::string::npos)
		return false;
	std::string file = ass3BaseName(target.substr(0, colon));
	std::string line = target.substr(colon + 1);
	if (file.empty() || line.empty())
		return false;

	bool fileSeen = report.location.find(file) != std::string::npos ||
	                report.message.find(file) != std::string::npos;
	bool lineSeen = report.location.find("\"ln\": " + line) != std::string::npos ||
	                report.location.find("\"ln\":" + line) != std::string::npos ||
	                report.message.find("\"ln\": " + line) != std::string::npos ||
	                report.message.find("\"ln\":" + line) != std::string::npos ||
	                report.location.find(":" + line) != std::string::npos ||
	                report.message.find(":" + line) != std::string::npos;
	return fileSeen && lineSeen;
}

// Branch refinement, statement transfer functions (updateAbsState +
// updateStateOn*), the GEP-offset tracking, getAccessOffset, the memory-safety
// helpers (canSafelyAccessMemory / canSafelyDerefPtr) and the buffer/null
// checkers are all student TODOs this year and live in Assignment_3.cpp.  The
// stub validators in this file deliberately do not call them — they compute
// ground truth from SVF primitives only, then query the BugReporter for the
// student's verdict.

/// Report a buffer overflow for a given ICFG node
void AbstractExecution::reportBufOverflow(const ICFGNode* node) {
	// Create an exception with the node's string representation
	AEException bug(node->toString());
	// Add the bug to the reporter using the helper
	bugReporter.addBugToReporter("buffer-overflow", bug, node);
}

/// Report a nullptr dereference for a given ICFG node
void AbstractExecution::reportNullDeref(const ICFGNode* node) {
	AEException bug(node->toString());
	bugReporter.addBugToReporter("nullptr-deref", bug, node);
}

bool AbstractExecution::isExternalCallForAssignment(const SVF::FunObjVar* func) {
	Set<std::string> extFuncs = {
	    "mem_insert", "str_insert",
	    "UNSAFE_BUFACCESS", "SAFE_BUFACCESS",
	    "UNSAFE_PTRDEREF", "SAFE_PTRDEREF"};
	if (extFuncs.find(func->getName()) != extFuncs.end()) {
		return true;
	} else {
		return false;
	}
}

void AbstractExecution::runOnModule(SVF::ICFG* _icfg) {
	svfir = PAG::getPAG();
	icfg = _icfg;
	analyse();
	if (!caseConfig.emitJson)
		bugReporter.printReport();
}

u32_t AbstractExecution::getAnalyzedNodeCount() const {
	return static_cast<u32_t>(analyzedNodes.size());
}

u32_t AbstractExecution::getTotalNodeCount() const {
	if (!icfg)
		return 0;
	u32_t total = 0;
	for (auto it = icfg->begin(); it != icfg->end(); ++it)
		total++;
	return total;
}

double AbstractExecution::getICFGCoverage() const {
	u32_t total = getTotalNodeCount();
	if (total == 0)
		return 0.0;
	return 100.0 * static_cast<double>(getAnalyzedNodeCount()) / static_cast<double>(total);
}

bool AbstractExecution::hasTargetReport() const {
	if (caseConfig.targetLoc.empty())
		return false;
	for (const AssignmentBugReport& report : bugReporter.getReports()) {
		if (ass3ReportMatchesTarget(report, caseConfig.targetLoc))
			return true;
	}
	return false;
}

void AbstractExecution::writeJsonSummary(std::ostream& os, double wallSeconds,
                                         int exitCode, bool assertsValidated) const {
	const auto& reports = bugReporter.getReports();
	const bool targetHit = hasTargetReport();
	const u32_t tp = caseConfig.targetLoc.empty() ? 0 : (targetHit ? 1 : 0);
	const u32_t fp = reports.size() > tp ? static_cast<u32_t>(reports.size() - tp) : 0;

	os << "{\n";
	os << "  \"case_id\": \"" << ass3JsonEscape(caseConfig.caseId) << "\",\n";
	os << "  \"target\": \"" << ass3JsonEscape(caseConfig.targetLoc) << "\",\n";
	os << "  \"tags\": \"" << ass3JsonEscape(caseConfig.tags) << "\",\n";
	os << "  \"exit_code\": " << exitCode << ",\n";
	os << "  \"asserts_validated\": " << (assertsValidated ? "true" : "false") << ",\n";
	os << "  \"tp\": " << tp << ",\n";
	os << "  \"fp\": " << fp << ",\n";
	os << "  \"reports\": " << reports.size() << ",\n";
	os << "  \"wall_sec\": " << std::fixed << std::setprecision(3) << wallSeconds << ",\n";
	os << "  \"icfg_nodes\": " << getTotalNodeCount() << ",\n";
	os << "  \"analyzed_icfg_nodes\": " << getAnalyzedNodeCount() << ",\n";
	os << "  \"icfg_coverage\": " << std::fixed << std::setprecision(2) << getICFGCoverage() << ",\n";
	os << "  \"report_list\": [";
	for (size_t i = 0; i < reports.size(); ++i) {
		const AssignmentBugReport& report = reports[i];
		os << (i == 0 ? "\n" : ",\n");
		os << "    {\"kind\": \"" << ass3JsonEscape(report.kind)
		   << "\", \"node\": " << report.nodeId
		   << ", \"location\": \"" << ass3JsonEscape(report.location)
		   << "\", \"message\": \"" << ass3JsonEscape(report.message) << "\"}";
	}
	if (!reports.empty())
		os << "\n  ";
	os << "]\n";
	os << "}\n";
}

/**
 * @brief Build the interprocedural WTO per call-graph SCC entry.
 *
 * Each (mutually) recursive function's entry node becomes a WTO cycle head
 * because intra-SCC call edges are turned into back-edges.  The same
 * widening/narrowing machinery used for loops then drives recursion to a
 * fixpoint via handleICFGCycle; there is no separate "is recursive?" check.
 */
void AbstractExecution::initWTO() {
	ander = AndersenWaveDiff::createAndersenWaveDiff(svfir);
	// Find the strongly connected components of the call graph so we can hand
	// each SCC's member set to ICFGWTO below.
	Andersen::CallGraphSCC* callGraphScc = ander->getCallGraphSCC();
	callGraphScc->find();
	auto callGraph = ander->getCallGraph();

	// Build one interprocedural WTO per call-graph-SCC entry function.  The
	// SCC's function set is passed to ICFGWTO so that call edges *into* a
	// callee in the same SCC become back-edges: a (mutually) recursive
	// function's entry node then shows up as a WTO cycle head, and the same
	// widening/narrowing machinery used for loops drives it to a fixpoint.
	for (auto it = callGraph->begin(); it != callGraph->end(); ++it) {
		const FunObjVar* fun = it->second->getFunction();
		if (fun->isDeclaration())
			continue;

		NodeID repNodeId = callGraphScc->repNode(it->second->getId());
		const NodeBS& cgSCCNodes = callGraphScc->subNodes(repNodeId);

		// Only SCC-entry functions (with a caller outside the SCC, or no
		// caller at all) own a WTO; intra-SCC members are reached via the
		// entry's interprocedural WTO.
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

	// Record every cycle head (loop heads and recursive-function entries) so
	// handleFunction can dispatch them to handleICFGCycle.
	for (auto fun : funcToWTO) {
		for (const ICFGWTOComp* comp : fun.second->getWTOComponents()) {
			if (const ICFGCycleWTO* cycle = SVFUtil::dyn_cast<ICFGCycleWTO>(comp)) {
				cycleHeadToCycle[cycle->head()->getICFGNode()] = cycle;
			}
		}
	}
}

// updateGepObjOffsetFromBase / hasGepObjOffsetFromBase / getGepObjOffsetFromBase
// / getAccessOffset / updateAbsState / mergeStatesFromPredecessors / branch
// refinement (isCmpBranchFeasible / isSwitchBranchFeasible / isBranchFeasible)
// are all student TODOs this year and live in Assignment_3.cpp.


/// handle global node
void AbstractExecution::handleGlobalNode() {
	AbstractState as;
	const ICFGNode* node = icfg->getGlobalICFGNode();
	analyzedNodes.insert(node);
	postAbsTrace()[node] = preAbsTrace[node];
	// The null pointer carries the dedicated null memory address so that
	// pointer-vs-null comparisons and null dereferences can be detected.
	postAbsTrace()[node][IRGraph::NullPtr] = AddressValue(NullMemAddr);
	// Global Node, we just need to handle addr, load, store, copy and gep
	for (const SVFStmt* stmt : node->getSVFStmts()) {
		updateAbsState(stmt);
	}
}

/// If we have stub calls as ground truths in the program, including svf_assert and OVERFLOW, 
/// make sure they are fully verified/checked.
void AbstractExecution::ensureAllAssertsValidated() {
	u32_t svf_assert_to_be_verified = 0;
	u32_t overflow_assert_to_be_verified = 0;
	for (auto it = svfir->getICFG()->begin(); it != svfir->getICFG()->end(); ++it) {
		const ICFGNode* node = it->second;
		if (const CallICFGNode* call = SVFUtil::dyn_cast<CallICFGNode>(node)) {
			if (const FunObjVar* fun = call->getCalledFunction()) {
				if (fun->getName() == "svf_assert" || fun->getName() == "OVERFLOW") {
					if (fun->getName() == "svf_assert") {
						svf_assert_to_be_verified++;
					}
					else {
						overflow_assert_to_be_verified++;
					}
					if (assert_points.find(call) == assert_points.end()) {
						std::stringstream ss;
						ss << "The stub function calliste (svf_assert or OVERFLOW) has not been checked: "
						   << call->toString();
						std::cerr << ss.str() << std::endl;
						assert(false);
					}
				}
			}
		}
	}

	assert(overflow_assert_to_be_verified <= bugReporter.getBugReporter().getBugSet().size() &&
		       "The number of stub asserts (ground truth) should <= the number of overflow reported");
}


/**
 * @brief The driver program
 *
 * This function conducts the overall analysis of the program by initializing and processing
 * various components of the control flow graph (ICFG) and handling global nodes and WTO cycles.
 * It marks recursive functions, initializes WTOs for each function, and processes the main function.
 */
void AbstractExecution::analyse() {
	// Init WTOs for all functions, and handle Global ICFGNode of SVFModule
	initWTO();
	// AbstractStateManager was folded into AbstractInterpretation upstream; the
	// header AE/Svfexe/AbstractStateManager.h was removed.  Use the singleton
	// AbstractInterpretation; it pulls SVFIR from PAG::getPAG() internally and
	// does not need an explicit Andersen analysis to be passed in.
	ai = &AbstractInterpretation::getAEInstance();
	svfStateMgr = new Ass3StateManager(ai);

	// Handle the global node
	handleGlobalNode();

	// Process the main function if it exists
	if (const FunObjVar* fun = svfir->getFunObjVar("main")) {
		// arguments of main are initialised as \top to represent all possible inputs
		for (u32_t i = 0; i < fun->arg_size(); ++i) {
			AbstractState& as = getAbsStateFromTrace(icfg->getGlobalICFGNode());
			as[fun->getArg(i)->getId()] = IntervalValue::top();
		}
		//assert the main function exist
		assert(svfir->getFunObjVar("main") != nullptr && "Main function not found");
		handleFunction(svfir->getICFG()->getFunEntryICFGNode(svfir->getFunObjVar("main")));
	}
	return;
}

/**
 * @brief Handle a node in the ICFG
 *
 * This function handles a node in the ICFG by merging the abstract states of its predecessors,
 * updating the abstract state based on the node's statements, and handling stub functions.
 * It also checks if the abstract state has reached a fixpoint and returns the result.
 * Return true means the abstract state has changed
 * Return false means the abstract state has reached a fixpoint or is infeasible
 *
 * @param node The node to be handled
 * @return True if the abstract state has changed, false if it has reached a fixpoint or is infeasible
 */
bool AbstractExecution::handleICFGNode(const ICFGNode* node) {
	AbstractState tmpEs;
	bool is_feasible = mergeStatesFromPredecessors(node, tmpEs);
	if (!is_feasible) {
		SVFUtil::errs() << "Infeasible for node " << node->getId() << "\n";
		return false;
	}
	analyzedNodes.insert(node);
	preAbsTrace[node] = tmpEs;
	// Store the last abstract state, used to check if the abstract state has reached a fixpoint
	AbstractState last_as = postAbsTrace()[node]; 
	postAbsTrace()[node] = preAbsTrace[node];
	for (const SVFStmt* stmt : node->getSVFStmts()) {
		updateAbsState(stmt);
	}

	if (const CallICFGNode* callNode = SVFUtil::dyn_cast<CallICFGNode>(node)) {
		// Bug checking for external API calls happens inside handleCallSite,
		// after the API value summary is applied.
		handleCallSite(callNode);
	}
	else {
		// Implicit dereference / GEP overflow checks on ordinary statements.
		nullptrDerefDetection(node);
		bufOverflowDetection(node);
	}
	// If the abstract state is the same as the last abstract state, return false because we have reached fixpoint
	if (postAbsTrace()[node] == last_as) {
		return false;
	}
	return true;
}
// updateAbsState now lives in Assignment_3.cpp (student TODO).

namespace {
/// Harness-only ground-truth check for buffer access safety.  Computed from
/// SVF primitives (base object size + GepObjVar::getConstantFieldIdx) so it
/// never depends on the student's gepObjOffsetFromBase map or
/// canSafelyAccessMemory implementation.
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

/// Harness-only ground-truth check for pointer-dereference safety.
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

/**
 * @brief Handle a call site in the control flow graph
 *
 * This function processes a call site by updating the abstract state, handling the called function,
 * and managing the call stack. It resumes the execution state after the function call.
 *
 * @param node The call site node to be handled
 */
void AbstractExecution::handleCallSite(const CallICFGNode* callNode) {
	// Get the callee function associated with the call site
	const FunObjVar* callee = callNode->getCalledFunction();
	if (!callee)
		return;
	std::string fun_name = callee->getName();
	if (fun_name == "OVERFLOW" || fun_name == "svf_assert" || fun_name == "svf_assert_eq") {
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
		// External API value summaries.  The student implements the memory and
		// string families (memcpy/memset/strcpy/strcat/...) plus the
		// assignment-specific mem_insert/str_insert stubs in updateStateOnExtCall;
		// unmodelled functions fall back to SVF inside that dispatcher.  After
		// propagating values we run the bug checkers on the API's
		// pointer/length arguments.
		updateStateOnExtCall(callNode);
		nullptrDerefDetection(callNode);
		bufOverflowDetection(callNode);
	}
	else {
		// Inline the callee body unconditionally.  handleFunction guards
		// against re-entering a WTO that is already on the stack, so
		// recursive callsites just fall back to the outer WTO cycle.
		handleFunction(svfir->getICFG()->getFunEntryICFGNode(callee));
		const RetICFGNode* retNode = callNode->getRetICFGNode();
		if (postAbsTrace().count(callNode))
			postAbsTrace()[retNode] = postAbsTrace()[callNode];
	}
}

/**
 * @brief Validate the SAFE/UNSAFE checkpoint stub functions.
 *
 * These stubs encode the ground truth for the bug checkers.  Validation uses
 * the harness-only `harnessSafeAccess` / `harnessSafeDeref` helpers above,
 * NOT the student's `canSafelyAccessMemory` / `canSafelyDerefPtr` — so the
 * stub verdict cannot be biased by student bugs.
 */
void AbstractExecution::handleCheckpointStubs(const CallICFGNode* callNode) {
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

void AbstractExecution::handleFunction(const ICFGNode* funEntry) {
	// Iterate the function's interprocedural WTO components in WTO order.
	// Singletons are handled directly; cycles (loop heads and recursive
	// function entries) are driven to a fixpoint by handleICFGCycle.
	//
	// `_funcsInFlight` guards re-entry: if this WTO is already on the call
	// stack (i.e. a recursive callsite tried to inline back into us), return
	// immediately and let the outer cycle's widen/narrow iteration drive the
	// recursion to a fixpoint.  This is the only mechanism for handling
	// recursion — there is no separate "is recursive callsite?" check.
	const FunObjVar* fun = funEntry->getFun();
	auto it = funcToWTO.find(fun);
	if (it == funcToWTO.end())
		return;
	if (!_funcsInFlight.insert(fun).second)
		return;
	for (const ICFGWTOComp* comp : it->second->getWTOComponents()) {
		if (const ICFGSingletonWTO* singleton = SVFUtil::dyn_cast<ICFGSingletonWTO>(comp)) {
			handleICFGNode(singleton->getICFGNode());
		}
		else if (const ICFGCycleWTO* cycle = SVFUtil::dyn_cast<ICFGCycleWTO>(comp)) {
			handleICFGCycle(cycle);
		}
	}
	_funcsInFlight.erase(fun);
}

/**
 * @brief Handle stub functions for verifying abstract interpretation results
 *
 * This function handles specific stub functions (`svf_assert` and `OVERFLOW`) to check whether
 * the abstract interpretation results are as expected. For `svf_assert(expr)`, the expression must hold true.
 * For `OVERFLOW(object, offset_access)`, the size of the object must be less than or equal to the offset access.
 *
 * @param callnode The call node representing the stub function to be handled
 */

void AbstractExecution::handleStubFunctions(const SVF::CallICFGNode* callNode) {
	// Handle the 'svf_assert' stub function
	if (callNode->getCalledFunction()->getName() == "svf_assert") {
		assert_points.insert(callNode);
		// If the condition is false, the program is infeasible
		u32_t arg0 = callNode->getArgument(0)->getId();
		AbstractState& as = getAbsStateFromTrace(callNode);

		// Check if the interval for the argument is infinite
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
		if (as[arg0].getInterval().equals(as[arg1].getInterval()))
		{
			SVFUtil::errs() << SVFUtil::sucMsg("The assertion is successfully verified!!\n");
		}
		else
		{
			SVFUtil::errs() <<"svf_assert_eq Fail. " << callNode->toString() << "\n";
			assert(false);
		}
		return;
	}
	// Handle the 'OVERFLOW' stub function.  Ground truth is computed from SVF
	// primitives only — `GepObjVar::getConstantFieldIdx()` gives the accumulated
	// offset of the sub-object from its base — so the verdict does not depend
	// on the student's gepObjOffsetFromBase map.
	else if (callNode->getCalledFunction()->getName() == "OVERFLOW") {
		assert_points.insert(callNode);
		u32_t arg0 = callNode->getArgument(0)->getId();
		u32_t arg1 = callNode->getArgument(1)->getId();

		AbstractState& as = getAbsStateFromTrace(callNode);
		AbstractValue gepRhsVal = as[arg0];

		if (gepRhsVal.isAddr()) {
			bool overflow = false;
			s64_t access_offset = as[arg1].getInterval().ub().getIntNumeral();
			for (const auto& addr : gepRhsVal.getAddrs()) {
				NodeID objId = as.getIDFromAddr(addr);
				const BaseObjVar* baseObj = svfir->getBaseObject(objId);
				if (!baseObj || !baseObj->isConstantByteSize())
					continue;
				s64_t size = (s64_t)baseObj->getByteSizeOfObj();
				s64_t baseOffset = 0;
				if (auto* gepObj = SVFUtil::dyn_cast<GepObjVar>(svfir->getGNode(objId)))
					baseOffset = (s64_t)gepObj->getConstantFieldIdx();
				if (baseOffset + access_offset >= size)
					overflow = true;
			}
			if (overflow) {
				reportBufOverflow(callNode);
				std::cerr << "Your implementation successfully detected the buffer overflow\n";
			}
			else {
				SVFUtil::errs() << "Your implementation failed to detect the buffer overflow!"
				                << callNode->toString() << "\n";
				assert(false);
			}
		}
		else {
			SVFUtil::errs() << "Your implementation failed to detect the buffer overflow!"
			                << callNode->toString() << "\n";
			assert(false);
		}
	}
}

// ===========================================================================
// Ass3StateManager — narrow facade forwarding only the whitelisted state and
// GEP primitives to the underlying AbstractInterpretation.  Defined here (not
// in the header) so student code never sees AbstractInterpretation/AbsExtAPI.
// ===========================================================================
namespace SVF {

const AbstractValue& Ass3StateManager::getAbsValue(const ValVar* var, const ICFGNode* node) {
	return ai->getAbsValue(var, node);
}
const AbstractValue& Ass3StateManager::getAbsValue(const ObjVar* var, const ICFGNode* node) {
	return ai->getAbsValue(var, node);
}
const AbstractValue& Ass3StateManager::getAbsValue(const SVFVar* var, const ICFGNode* node) {
	return ai->getAbsValue(var, node);
}
void Ass3StateManager::updateAbsValue(const ValVar* var, const AbstractValue& val, const ICFGNode* node) {
	ai->updateAbsValue(var, val, node);
}
void Ass3StateManager::updateAbsValue(const ObjVar* var, const AbstractValue& val, const ICFGNode* node) {
	ai->updateAbsValue(var, val, node);
}
void Ass3StateManager::updateAbsValue(const SVFVar* var, const AbstractValue& val, const ICFGNode* node) {
	ai->updateAbsValue(var, val, node);
}
AbstractValue Ass3StateManager::loadValue(const ValVar* pointer, const ICFGNode* node) {
	return ai->loadValue(pointer, node);
}
void Ass3StateManager::storeValue(const ValVar* pointer, const AbstractValue& val, const ICFGNode* node) {
	ai->storeValue(pointer, val, node);
}
AddressValue Ass3StateManager::getGepObjAddrs(const ValVar* pointer, IntervalValue offset) {
	return ai->getGepObjAddrs(pointer, offset);
}
IntervalValue Ass3StateManager::getGepElementIndex(const GepStmt* gep) {
	return ai->getGepElementIndex(gep);
}
IntervalValue Ass3StateManager::getGepByteOffset(const GepStmt* gep) {
	return ai->getGepByteOffset(gep);
}
u32_t Ass3StateManager::getAllocaInstByteSize(const AddrStmt* addr) {
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
