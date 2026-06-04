//===- Assignment-3.cpp -- Abstract Interpretation --//
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
 * Abstract Interpretation and bug detection
 *
 * Created on: Feb 19, 2024
 */

#include "Assignment_3.h"
// Needed by the analysis driver below (runOnModule / analyse / handleCallSite
// reach AbstractInterpretation::getAEInstance and Andersen's SCC check).
#include "AE/Svfexe/AbstractInterpretation.h"
#include "WPA/Andersen.h"

using namespace SVF;
using namespace SVFUtil;

// ===========================================================================
// Student TODOs
// ===========================================================================
// Implement abstract interpretation for verification and bug detection. The
// harness (AEReporter.cpp) drives the analysis and calls into the six entry
// points below (please do not delete); You are free to add any internal
// classes and helper methods you need within Assignment_3.h and
// Assignment_3.cpp.
// ===========================================================================

void AbstractExecution::updateAbsState(const SVFStmt* stmt) {
	// TODO: dispatch on the statement subtype and update the abstract state.
}

bool AbstractExecution::mergeStatesFromPredecessors(const ICFGNode* block, AbstractState& as) {
	// TODO: join predecessor post-states (with branch refinement) into `as`.
	return false;
}

void AbstractExecution::handleICFGCycle(const ICFGCycleWTO* cycle) {
	// TODO: iterate the cycle body to a fixpoint (widening optional).
}

void AbstractExecution::bufOverflowDetection(const ICFGNode* node) {
	// TODO: detect out-of-bounds memory accesses at `node`.
}

void AbstractExecution::nullptrDerefDetection(const ICFGNode* node) {
	// TODO: detect nullptr dereferences at `node`.
}

void AbstractExecution::updateStateOnExtCall(const SVF::CallICFGNode* call) {
	// TODO: model memory/string library calls and assignment-specific stubs.
}


// ===========================================================================
// Analysis driver entry points (student TODO this year).
//
// `analyse()` (below) calls `handleGlobalNode()` once for the SVFModule's
// global ICFG node and `handleFunction(main_entry)` to start the per-function
// analysis.  From there it is up to you: a typical design has
// `handleFunction` iterate the function's interprocedural WTO components,
// dispatching singletons to `handleICFGNode` and cycles to `handleICFGCycle`.
// `handleICFGNode` then merges predecessor states, runs the per-statement
// transfer functions, and dispatches call sites via `handleCallSite`.  You
// are free to deviate from this skeleton as long as the test driver's
// expectations hold.
// ===========================================================================

void AbstractExecution::handleGlobalNode() {
	// TODO: initialise the global ICFG node's state and replay the global
	// statements through updateAbsState.
}

void AbstractExecution::handleFunction(const ICFGNode* funEntry) {
	// TODO: walk the function's interprocedural WTO components (singletons
	// vs. cycles) and dispatch to handleICFGNode / handleICFGCycle.
}

bool AbstractExecution::handleICFGNode(const ICFGNode* node) {
	// TODO: merge predecessor states, run the per-statement transfer
	// functions, handle call sites (delegating to handleCallSite for call
	// nodes), and return whether the post-state changed.
	return false;
}
// ===========================================================================
// Analysis driver.  These methods sit on the student side of the harness/
// student split because they orchestrate the per-node / per-call flow that
// students design.  They are pre-implemented so a working pipeline is in
// place from the moment the rest of the tasks come online; you may rewrite
// them if your design needs a different shape.
// ===========================================================================

/// Entry point invoked from test-ae.cpp.  Records the SVFIR and the ICFG
/// pointer, runs the analysis, and (unless the case is in JSON mode) prints
/// the bug-reporter summary.
void AbstractExecution::runOnModule(SVF::ICFG* _icfg) {
	svfir = PAG::getPAG();
	icfg = _icfg;
	analyse();
	if (!getReporter().getCaseConfig().emitJson)
		getReporter().printReport();
}

/// Build the interprocedural WTO, initialise the AbstractInterpretation
/// singleton, replay the global ICFG node, then start the analysis at main.
void AbstractExecution::analyse() {
	initWTO();
	// AbstractStateManager was folded into AbstractInterpretation upstream
	// (the AE/Svfexe/AbstractStateManager.h header was removed).  Use the
	// AbstractInterpretation singleton; it pulls SVFIR from PAG::getPAG()
	// internally and does not need an explicit Andersen analysis to be
	// passed in.
	ai = &AbstractInterpretation::getAEInstance();

	handleGlobalNode();

	if (const FunObjVar* fun = svfir->getFunObjVar("main")) {
		// Arguments of main are initialised as \top to represent all
		// possible inputs.
		for (u32_t i = 0; i < fun->arg_size(); ++i) {
			AbstractState& as = getAbsStateFromTrace(icfg->getGlobalICFGNode());
			as[fun->getArg(i)->getId()] = IntervalValue::top();
		}
		assert(svfir->getFunObjVar("main") != nullptr && "Main function not found");
		handleFunction(svfir->getICFG()->getFunEntryICFGNode(svfir->getFunObjVar("main")));
	}
}

/// Dispatch a call ICFG node.  The harness's sub-dispatchers
/// (handleStubFunctions / handleCheckpointStubs in AEHelper.cpp) take care of
/// the svf_assert and SAFE_/UNSAFE_ stubs; ordinary external API calls go
/// through the student-implemented updateStateOnExtCall + bug checkers; all
/// other calls are inlined via handleFunction, skipping recursive callsites
/// (same call-graph SCC) so the outer cycle's widening/narrowing drives the
/// recursion to a fixpoint.
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
		// External-API value summary (Task 4) followed by the bug checkers
		// on the API's pointer / length arguments (Tasks 5 / 6).
		updateStateOnExtCall(callNode);
		nullptrDerefDetection(callNode);
		bufOverflowDetection(callNode);
	}
	else {
		// Recursive callsites (same call-graph SCC) are skipped: the
		// interprocedural WTO built in initWTO() already encoded this call
		// edge as a back-edge, so the outer cycle's widen/narrow iteration
		// in handleICFGCycle drives the recursion to a fixpoint.  Mirrors
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

/// Record a buffer-overflow bug.
void AbstractExecution::reportBufOverflow(const ICFGNode* node) {
	AEException bug(node->toString());
	getReporter().addBugToReporter("buffer-overflow", bug, node);
}

/// Record a nullptr-dereference bug.
void AbstractExecution::reportNullDeref(const ICFGNode* node) {
	AEException bug(node->toString());
	getReporter().addBugToReporter("nullptr-deref", bug, node);
}
