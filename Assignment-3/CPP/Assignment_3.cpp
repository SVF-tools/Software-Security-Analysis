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
// Needed by the analysis driver below (runOnModule / analyse reach
// AbstractInterpretation::getAEInstance; handleCallSite uses Andersen's SCC
// check).
#include "AE/Svfexe/AbstractInterpretation.h"
#include "WPA/Andersen.h"

using namespace SVF;
using namespace SVFUtil;

// ===========================================================================
// Analysis driver entry points (pre-implemented).
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

// ===========================================================================
// Student TODOs — driver entry points
// ===========================================================================
// The harness's `analyse()` (above) calls `handleGlobalNode()` once for the
// SVFModule's global ICFG node and `handleFunction(main_entry)` to start the
// per-function analysis.  A typical layering is:
//   handleFunction  walks the interprocedural WTO components and dispatches
//                   singletons to handleICFGNode / cycles to handleICFGCycle.
//   handleICFGNode  merges predecessor states (Task 2), runs the per-statement
//                   transfer functions (Task 1), routes call sites via
//                   handleCallSite, and runs the bug checkers (Tasks 5 / 6).
//   handleICFGCycle iterates the cycle body to a fixpoint with widening /
//                   narrowing (Task 3).
// You are free to deviate from this skeleton as long as the test driver's
// expectations hold.  Helper methods for Tasks 1, 2, 4, 5, 6 are yours to
// design — override the matching no-op virtuals (updateAbsState,
// mergeStatesFromPredecessors, updateStateOnExtCall, bufOverflowDetection,
// nullptrDerefDetection) if you want handleCallSite to drive into them.
// ===========================================================================

void AbstractExecution::handleGlobalNode() {
	// TODO: initialise the global ICFG node's state and replay the global
	// statements through your statement transfer functions.
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

void AbstractExecution::handleICFGCycle(const ICFGCycleWTO* cycle) {
	// TODO: iterate the cycle body to a fixpoint (widening / narrowing).
}

// ===========================================================================
// Student TODO — handleCallSite.
//
// Dispatch a call ICFG node based on its callee:
//
//   * svf_assert / svf_assert_eq                 -> handleStubFunctions(call)
//   * SAFE_/UNSAFE_BUFACCESS, SAFE_/UNSAFE_PTRDEREF
//                                                -> handleCheckpointStubs(call)
//   * nd / rand                                  -> nondeterministic return:
//                                                   set the actual-return
//                                                   variable to TOP.
//   * other external callees (SVFUtil::isExtCall)
//                                                -> updateStateOnExtCall, then
//                                                   run the bug checkers
//                                                   (nullptrDerefDetection +
//                                                   bufOverflowDetection) on
//                                                   the call's arguments.
//   * non-extern callees                         -> skip recursive callsites
//                                                   using Andersen's
//                                                   inSameCallGraphSCC, then
//                                                   inline by calling
//                                                   handleFunction on the
//                                                   callee's entry ICFG node
//                                                   and forward the call
//                                                   node's post-state to the
//                                                   return ICFG node.
// ===========================================================================
void AbstractExecution::handleCallSite(const CallICFGNode* callNode) {
	// TODO
}
