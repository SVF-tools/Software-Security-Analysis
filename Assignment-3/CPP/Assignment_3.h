//===- Assignment-3.h -- Abstract Interpretation --//
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
#include "AEReporter.h"
#include "SVFIR/SVFIR.h"
#include <ostream>

namespace SVF {
	class AbstractInterpretation;
	class AndersenWaveDiff;

	/// Driver class for the Assignment-3 abstract-interpretation pipeline.
	///
	/// The harness (AEHelper.cpp / AEReporter.cpp) owns construction, the
	/// analysis driver, the stub dispatch, the bug-reporting facade, and the
	/// abstract-state helpers that wrap the underlying AbstractInterpretation
	/// singleton.  Students implement the six tasks declared at the bottom of
	/// this class:
	///
	///   General analysis engine
	///     1. Statement transfer functions       (updateAbsState)
	///     2. Branch refinement                  (mergeStatesFromPredecessors)
	///     3. Cycle and recursion fixpoint       (handleICFGCycle and the per-node driver)
	///     4. External-API value summaries       (updateStateOnExtCall)
	///
	///   Bug checkers
	///     5. Buffer-overflow checker            (bufOverflowDetection)
	///     6. Nullptr-dereference checker        (nullptrDerefDetection)
	class AbstractExecution {
	 public:
		// ====================================================================
		// Construction / lifetime (harness)
		// ====================================================================
		explicit AbstractExecution(const AssignmentCaseConfig& config = AssignmentCaseConfig())
		    : bugReporter(config) {
		}

		virtual ~AbstractExecution() {
			// `ai` is the AbstractInterpretation singleton; SVF owns its lifetime.
		}

		static AbstractExecution& getAEInstance()
		{
			static AbstractExecution instance;
			return instance;
		}

		/// Harness reporter accessor (used by test-ae.cpp for the JSON summary).
		AEReporter& getReporter() { return bugReporter; }
		const AEReporter& getReporter() const { return bugReporter; }

		// ====================================================================
		// Harness driver (do not modify)
		//
		// Entry points called by the test driver (test-ae.cpp):
		//   runOnModule         -- build SVFIR + ICFG and kick off analyse().
		//   analyse             -- initWTO, init globals, dispatch into the
		//                          student-implemented handleFunction.
		//   initWTO             -- build one interprocedural WTO per call-graph
		//                          SCC entry function (Andersen-based).
		//   ensureAllAssertsValidated
		//                       -- after analysis, verify every ground-truth
		//                          stub call site was reached and that the
		//                          UNSAFE_* counts match reported bug counts.
		// ====================================================================
		virtual void runOnModule(ICFG* icfg);
		virtual void analyse();
		void initWTO();
		void ensureAllAssertsValidated();

		// ====================================================================
		// Harness dispatch + bug-reporting facade
		//
		// The student's per-node driver (handleICFGNode) calls handleCallSite
		// for each CallICFGNode; the harness then routes svf_assert /
		// SAFE_*/UNSAFE_* stubs to the appropriate checker, and forwards
		// non-extern callees back into handleFunction (with the recursive
		// callsite skip rule documented in AEHelper.cpp).
		// ====================================================================
		void handleCallSite(const CallICFGNode* callnode);
		void handleStubFunctions(const CallICFGNode* call);
		void handleCheckpointStubs(const CallICFGNode* callnode);
		bool isExternalCallForAssignment(const SVF::FunObjVar* func);

		void reportBufOverflow(const ICFGNode* node);
		void reportNullDeref(const ICFGNode* node);

		// ====================================================================
		// Abstract-state helpers
		//
		// Pre-implemented operations on the abstract domain: reading / writing
		// the abstract value of a variable, loading / storing through an
		// abstract pointer, and computing GEP byte / element offsets and
		// alloca byte sizes.  Use these from the transfer functions and the
		// external-API summaries instead of touching the underlying
		// AbstractInterpretation singleton directly.
		// ====================================================================
		const AbstractValue& getAbsValue(const ValVar* var, const ICFGNode* node);
		const AbstractValue& getAbsValue(const ObjVar* var, const ICFGNode* node);
		const AbstractValue& getAbsValue(const SVFVar* var, const ICFGNode* node);

		void updateAbsValue(const ValVar* var, const AbstractValue& val, const ICFGNode* node);
		void updateAbsValue(const ObjVar* var, const AbstractValue& val, const ICFGNode* node);
		void updateAbsValue(const SVFVar* var, const AbstractValue& val, const ICFGNode* node);

		AbstractValue loadValue(const ValVar* pointer, const ICFGNode* node);
		void storeValue(const ValVar* pointer, const AbstractValue& val, const ICFGNode* node);

		AddressValue getGepObjAddrs(const ValVar* pointer, IntervalValue offset);
		IntervalValue getGepElementIndex(const GepStmt* gep);
		IntervalValue getGepByteOffset(const GepStmt* gep);
		u32_t getAllocaInstByteSize(const AddrStmt* addr);

		/// Read-only access to the abstract state at an ICFG node (pulled from
		/// AbstractInterpretation's owned trace).
		AbstractState& getAbsStateFromTrace(const ICFGNode* node);

		// ====================================================================
		// STUDENT TODOs — General analysis engine (Tasks 1–4)
		// ====================================================================
		// The harness's `analyse()` calls `handleGlobalNode()` once and then
		// `handleFunction(main_entry)`.  From there the per-function /
		// per-node flow, the per-statement transfer functions, the branch
		// refinement, the cycle / recursion fixpoint, and the external-API
		// summaries are all your responsibility.
		//
		// A typical layering is:
		//   handleFunction       walks the interprocedural WTO components
		//                        and dispatches singletons to handleICFGNode
		//                        / cycles to handleICFGCycle.
		//   handleICFGNode       merges predecessor states (Task 2), runs
		//                        the per-statement transfer functions (Task
		//                        1), routes call sites via handleCallSite,
		//                        and runs the bug checkers (Tasks 5 / 6).
		// You are free to deviate from this layering as long as the test
		// driver's expectations (covered stubs, reported bugs) hold.
		// ====================================================================

		/// Driver entry points (called from analyse() / handleICFGNode).
		void handleGlobalNode();
		void handleFunction(const ICFGNode* funEntry);
		bool handleICFGNode(const ICFGNode* node);

		// --------------------------------------------------------------------
		// Task 1 — Statement transfer functions
		//
		// Dispatch on the SVFStmt subtype and update the abstract state.
		// Unary and branch statements have no value-flow effect.  Per-kind
		// transfer helpers are yours to design as private methods.
		// --------------------------------------------------------------------
		virtual void updateAbsState(const SVFStmt* stmt);

		// --------------------------------------------------------------------
		// Task 2 — Branch refinement
		//
		// Join predecessor post-states into `as` and report whether at least
		// one incoming edge produced a feasible state.  Conditional intra-CFG
		// edges should be filtered by per-edge branch feasibility so that
		// infeasible paths are pruned during the join.
		// --------------------------------------------------------------------
		bool mergeStatesFromPredecessors(const ICFGNode* curNode, AbstractState& as);

		// --------------------------------------------------------------------
		// Task 3 — Cycle and recursion fixpoint
		//
		// Drive WTO cycles (loop heads and recursive-function entries) to a
		// fixpoint with widening / narrowing.  Recursive callsites are
		// filtered earlier by handleCallSite via `inSameCallGraphSCC`, so a
		// recursive function's WTO is iterated from its outer entry only.
		// --------------------------------------------------------------------
		void handleICFGCycle(const ICFGCycleWTO* cycle);

		// --------------------------------------------------------------------
		// Task 4 — External-API value summaries
		//
		// Model the call's effect on the abstract state for each external
		// function the harness recognises (see `isExternalCallForAssignment`).
		// The full set is:
		//     * memory family    : memcpy, memmove, memset
		//     * string family    : strcpy, strncpy, strcat, strncat,
		//                          strlen, wcslen
		//     * assignment stubs : mem_insert, str_insert
		// Unmodelled functions should conservatively leave the state
		// unchanged.  Length-of-string and related shared primitives are
		// yours to design as private helpers.
		// --------------------------------------------------------------------
		void updateStateOnExtCall(const SVF::CallICFGNode* extCallNode);

		// ====================================================================
		// STUDENT TODO — Task 5: Buffer-overflow checker
		//
		// Detect out-of-bounds accesses on GEP statements and on the
		// pointer/length arguments of external-API calls.  Memory-bounds
		// predicates and any GEP-offset bookkeeping you need are yours to
		// design as private helpers.
		// ====================================================================
		virtual void bufOverflowDetection(const ICFGNode* node);

		// ====================================================================
		// STUDENT TODO — Task 6: Nullptr-dereference checker
		//
		// Detect dereferences whose pointer may resolve to the null memory
		// address.  Null-safety predicates are yours to design as private
		// helpers.
		// ====================================================================
		virtual void nullptrDerefDetection(const ICFGNode* node);

	 protected:
		// Harness state shared with student methods.
		SVFIR* svfir;
		ICFG* icfg;

		/// Andersen pointer analysis (owns the call graph + SCC used to drive
		/// the interprocedural WTO); created in initWTO().
		AndersenWaveDiff* ander = nullptr;
		/// One interprocedural WTO per call-graph-SCC entry function.
		Map<const FunObjVar*, ICFGWTO*> funcToWTO;
		/// Abstract trace immediately before an ICFGNode.
		Map<const ICFGNode*, AbstractState> preAbsTrace;
		/// Post-trace lives inside the AbstractInterpretation singleton; this
		/// accessor exposes it as a Map<ICFGNode*, AbstractState>.
		Map<const ICFGNode*, AbstractState>& postAbsTrace();

	 private:
		AEReporter bugReporter;

		/// Underlying AbstractInterpretation singleton — backs the
		/// abstract-state helpers above and the post-trace accessor.
		AbstractInterpretation* ai = nullptr;
	};

} // namespace SVF
