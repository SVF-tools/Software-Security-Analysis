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
 * Abstract Interpretation and buffer overflow detection
 *
 * Created on: Feb 19, 2024
 */
#include "Assignment_3_Helper.h"
#include "SVFIR/SVFIR.h"
#include <ostream>

namespace SVF {
	class AndersenWaveDiff;
	/// Abstract Execution class
	class AbstractExecution {
	 public:
		/// Constructor
		explicit AbstractExecution(const AssignmentCaseConfig& config = AssignmentCaseConfig())
		    : caseConfig(config) {
		}

		virtual void runOnModule(ICFG* icfg);

		static AbstractExecution& getAEInstance()
		{
			static AbstractExecution instance;
			return instance;
		}

		/// Handle global variables and initializations
		void handleGlobalNode();

		/// Driver of the program
		virtual void analyse();

		/// Handle state updates for each type of SVF statement
		virtual void updateAbsState(const SVFStmt* stmt);

		/// Fuction used to implement buffer overflow detection
		virtual void bufOverflowDetection(const ICFGNode* node);
		/// Function used to implement nullptr dereference detection
		virtual void nullptrDerefDetection(const ICFGNode* node);

		/// Report a buffer overflow for a given ICFG node
		void reportBufOverflow(const ICFGNode* node);
		/// Report a nullptr dereference for a given ICFG node
		void reportNullDeref(const ICFGNode* node);

		/// External-API value summaries (student TODO).
		void updateStateOnExtCall(const SVF::CallICFGNode* extCallNode);

		/// Handle stub functions for verifying abstract interpretation results
		void handleStubFunctions(const CallICFGNode* call);

		/// Build the (interprocedural) WTO for each call-graph SCC entry.
		void initWTO();

		/// Merge predecessor states into the current node's pre-state.
		bool mergeStatesFromPredecessors(const ICFGNode* curNode, AbstractState& as);

		/// Handle a call site in the control flow graph
		void handleCallSite(const CallICFGNode* callnode);
		/// Validate the SAFE/UNSAFE checkpoint stub functions
		void handleCheckpointStubs(const CallICFGNode* callnode);
		bool isExternalCallForAssignment(const SVF::FunObjVar* func);

		/// Handle a function in the ICFG
		void handleFunction(const ICFGNode* funEntry);

		bool handleICFGNode(const ICFGNode* node);

		void handleICFGCycle(const ICFGCycleWTO* cycle);

		/// Return its abstract state given an ICFGNode (defined in the helper).
		AbstractState& getAbsStateFromTrace(const ICFGNode* node);

		void ensureAllAssertsValidated();

		/// Case-based grading/reporting helpers. These are intentionally
		/// end-to-end: the grader should score TP/FP/time/coverage per case,
		/// while module tags are only diagnosis hints.
		void writeJsonSummary(std::ostream& os, double wallSeconds, int exitCode,
		                      bool assertsValidated) const;
		u32_t getAnalyzedNodeCount() const;
		u32_t getTotalNodeCount() const;
		double getICFGCoverage() const;
		bool hasTargetReport() const;
		const AssignmentCaseConfig& getCaseConfig() const {
			return caseConfig;
		}

		/// Destructor
		virtual ~AbstractExecution() {
			// svfStateMgr is the AbstractInterpretation singleton; SVF owns its lifetime.
		}

	 protected:
		/// SVFIR and ICFG
		SVFIR* svfir;
		ICFG* icfg;
		/// Narrow state-manager facade used by all student code: it forwards only
		/// the whitelisted state read/write and GEP primitives and exposes no
		/// path to the SVF external-API modeller (see Ass3StateManager).
		Ass3StateManager* svfStateMgr = nullptr;

		/// Andersen pointer analysis (owns the call graph + SCC used to drive
		/// the interprocedural WTO); created in initWTO().
		AndersenWaveDiff* ander = nullptr;
		/// Map a function to its corresponding WTO
		Map<const FunObjVar*, ICFGWTO*> funcToWTO;
		/// Functions whose WTO is currently being iterated; re-entry returns
		/// early so the outer cycle drives the recursion to a fixpoint.
		Set<const FunObjVar*> _funcsInFlight;
		/// Abstract trace immediately before an ICFGNode.
		Map<const ICFGNode*, AbstractState> preAbsTrace;
		/// The "post" trace lives inside the manager (defined in the helper).
		Map<const ICFGNode*, AbstractState>& postAbsTrace();

	 private:
		AssignmentCaseConfig caseConfig;

		AbstractExecutionHelper bugReporter;

		Set<const CallICFGNode*> assert_points;
		Set<const ICFGNode*> analyzedNodes;

		Map<const ICFGNode*, const ICFGCycleWTO*> cycleHeadToCycle;

		/// harness-only raw handle to the underlying state manager; never used by
		/// student code (which only sees the svfStateMgr facade above).
		AbstractInterpretation* ai = nullptr;
	};

} // namespace SVF
