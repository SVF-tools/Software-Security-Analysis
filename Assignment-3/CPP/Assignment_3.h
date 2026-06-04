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
	/// Abstract Execution class
	class AbstractExecution {
	 public:
		/// Constructor
		explicit AbstractExecution(const AssignmentCaseConfig& config = AssignmentCaseConfig())
		    : bugReporter(config) {
		}

		/// Harness reporter accessor (used by test-ae.cpp for JSON summary).
		AEReporter& getReporter() { return bugReporter; }
		const AEReporter& getReporter() const { return bugReporter; }

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

		/// State-manager primitives (forward to the underlying
		/// AbstractInterpretation).  Use these from the statement transfer
		/// functions and the external-API summaries.
		///@{
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
		///@}

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

		/// Destructor
		virtual ~AbstractExecution() {
			// `ai` is the AbstractInterpretation singleton; SVF owns its lifetime.
		}

	 protected:
		/// SVFIR and ICFG
		SVFIR* svfir;
		ICFG* icfg;

		/// Andersen pointer analysis (owns the call graph + SCC used to drive
		/// the interprocedural WTO); created in initWTO().
		AndersenWaveDiff* ander = nullptr;
		/// Map a function to its corresponding WTO
		Map<const FunObjVar*, ICFGWTO*> funcToWTO;
		/// Abstract trace immediately before an ICFGNode.
		Map<const ICFGNode*, AbstractState> preAbsTrace;
		/// The "post" trace lives inside the manager (defined in the helper).
		Map<const ICFGNode*, AbstractState>& postAbsTrace();

	 private:
		AEReporter bugReporter;

		/// Handle to the underlying state manager.  Used by the merged
		/// `getAbsValue` / `updateAbsValue` / `loadValue` / `storeValue` / GEP
		/// primitives above, and by the harness-only post-trace accessors.
		AbstractInterpretation* ai = nullptr;
	};

} // namespace SVF
