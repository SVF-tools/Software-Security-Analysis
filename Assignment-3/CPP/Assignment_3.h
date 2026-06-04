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

	/// Student-facing AbstractExecution class.
	///
	/// The harness (AEHelper.cpp / AEReporter.cpp) provides interprocedural
	/// WTO construction, stub / checkpoint sub-dispatch, the external-API
	/// whitelist, the abstract-state helpers wrapping the underlying
	/// AbstractInterpretation singleton, and the assertion-coverage
	/// validator.  Assignment_3.cpp owns the analysis driver
	/// (runOnModule / analyse / report* forwarders, all pre-implemented)
	/// plus five student TODOs: the four driver entry points
	/// (handleGlobalNode / handleFunction / handleICFGNode /
	/// handleICFGCycle) and `handleCallSite`.  You design the rest of the
	/// six tasks and have your handleICFGNode dispatch into them however
	/// you see fit — override the matching no-op virtuals (updateAbsState,
	/// mergeStatesFromPredecessors, updateStateOnExtCall,
	/// bufOverflowDetection, nullptrDerefDetection) if you want your
	/// handleCallSite to call into your code.
	///
	///   General analysis engine
	///     1. Statement transfer functions       -- typically inside handleICFGNode
	///     2. Branch refinement                  -- typically inside handleICFGNode
	///     3. Cycle and recursion fixpoint       -- typically inside handleICFGCycle
	///     4. External-API value summaries       -- typically inside handleCallSite
	///
	///   Bug checkers
	///     5. Buffer-overflow checker
	///     6. Nullptr-dereference checker
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
		// STUDENT TODOs — Driver entry points
		// ====================================================================
		// The harness's `analyse()` calls `handleGlobalNode()` once and then
		// `handleFunction(main_entry)`.  From there the per-function flow,
		// per-node merging + transfer, and cycle / recursion fixpoint are
		// your responsibility.  A typical layering: handleFunction walks the
		// interprocedural WTO, handleICFGNode merges predecessor states and
		// runs the per-statement transfer functions, handleICFGCycle drives
		// loops / recursion to a fixpoint with widening / narrowing.  You
		// are free to deviate as long as the test driver's expectations
		// (covered stubs, reported bugs) hold.
		// ====================================================================
		void handleGlobalNode();
		void handleFunction(const ICFGNode* funEntry);
		bool handleICFGNode(const ICFGNode* node);
		void handleICFGCycle(const ICFGCycleWTO* cycle);

		// ====================================================================
		// Optional hooks for the rest of the six tasks (no-op by default).
		//
		// The pre-implemented handleCallSite (in Assignment_3.cpp) routes
		// ordinary external-API calls through updateStateOnExtCall and then
		// nullptrDerefDetection / bufOverflowDetection.  Override these
		// virtuals if you want your value-summary modelling (Task 4) and
		// your bug checkers (Tasks 5 / 6) to run during the analysis.  How
		// you structure them internally — per-statement transfer helpers
		// for Task 1, predecessor join with branch refinement for Task 2,
		// memory-safety predicates for Tasks 5 / 6, length-of-string
		// helpers shared across summaries and checker — is your design.
		// ====================================================================
		virtual void updateAbsState(const SVFStmt* stmt) {}
		virtual bool mergeStatesFromPredecessors(const ICFGNode* curNode, AbstractState& as) { return false; }
		virtual void updateStateOnExtCall(const SVF::CallICFGNode* extCallNode) {}
		virtual void bufOverflowDetection(const ICFGNode* node) {}
		virtual void nullptrDerefDetection(const ICFGNode* node) {}

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
