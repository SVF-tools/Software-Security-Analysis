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
#include "Assignment-3-Helper.h"
#include "AE/Svfexe/AbsExtAPI.h"
#include "SVFIR/SVFIR.h"

namespace SVF {
	/// Abstract Execution class
	class AbstractExecution {
	 public:
		/// Constructor
		AbstractExecution() {
		}

		virtual void runOnModule(ICFG* icfg);

		static AbstractExecution& getAEInstance()
		{
			static AbstractExecution instance;
			return instance;
		}

		/// Handle a Singleton Weak Topological Order (WTO) node in the control flow graph
		void handleSingletonWTO(const ICFGSingletonWTO* node);
		/// Handle a WTO which involves control-flow cycle(s)
		void handleCycleWTO(const ICFGCycleWTO* cycle);

		/// Handle global variables and initializations
		void handleGlobalNode();

		/// Driver of the program
		virtual void analyse();

		/// Handle state updates for each type of SVF statement
		virtual void updateAbsState(const SVFStmt* stmt);

		/// Fuction used to implement buffer overflow detection
		virtual void bufOverflowDetection(const SVFStmt* stmt);

		/// Report a buffer overflow for a given ICFG node
		void reportBufOverflow(const ICFGNode* node);

		// handle SVF Statements
		///@{
		void updateStateOnAddr(const AddrStmt* addr);
		void updateStateOnGep(const GepStmt* gep);
		void updateStateOnStore(const StoreStmt* store);
		void updateStateOnLoad(const LoadStmt* load);
		void updateStateOnCmp(const CmpStmt* cmp);
		void updateStateOnCall(const CallPE* call);
		void updateStateOnRet(const RetPE* retPE);
		void updateStateOnCopy(const CopyStmt* copy);
		void updateStateOnPhi(const PhiStmt* phi);
		void updateStateOnBinary(const BinaryOPStmt* binary);
		void updateStateOnSelect(const SelectStmt* select);
		void updateStateOnExtCall(const SVF::CallICFGNode* extCallNode);
		///@}

		/// Handle stub functions for verifying abstract interpretation results
		void handleStubFunctions(const CallICFGNode* call);

		void handleWTOComponents(const std::list<const ICFGWTOComp*>& wtoComps);

		/// Mark recursive functions in the call graph
		void initWTO();

		/// Path feasiblity handling
		///@{
		bool mergeStatesFromPredecessors(const ICFGNode* curNode, AbstractState& as);

		bool isCmpBranchFeasible(const CmpStmt* cmpStmt, s64_t succ, AbstractState& as);
		bool isSwitchBranchFeasible(const SVFVar* var, s64_t succ, AbstractState& as);
		bool isBranchFeasible(const IntraCFGEdge* intraEdge, AbstractState& as);
		///@}

		/// Handle a call site in the control flow graph
		void handleCallSite(const CallICFGNode* callnode);
		bool isExternalCallForAssignment(const SVF::FunObjVar* func);

		    /// Return its abstract state given an ICFGNode
		AbstractState& getAbsStateFromTrace(const ICFGNode* node) {
			const ICFGNode* repNode = icfg->getRepNode(node);
			return postAbsTrace[repNode];
		}

		/// Update the offset of a GEP (GetElementPtr) object from its base address
		void updateGepObjOffsetFromBase(AbstractState& as, AddressValue gepAddrs, AddressValue objAddrs, IntervalValue offset);

		/// Return the accessing offset of an object at a GepStmt
		IntervalValue getAccessOffset(NodeID objId, const GepStmt* gep);

		void ensureAllAssertsValidated();

		/// Destructor
		virtual ~AbstractExecution() {
		}

	 protected:
		/// SVFIR and ICFG
		SVFIR* svfir;
		ICFG* icfg;

		/// callstack
		std::vector<const CallICFGNode*> callSiteStack;
		/// Map a function to its corresponding WTO
		Map<const FunObjVar*, ICFGWTO*> funcToWTO;
		/// A set of functions which are involved in recursions
		Set<const FunObjVar*> recursiveFuns;
		/// Abstract trace immediately before an ICFGNode.
		Map<const ICFGNode*, AbstractState> preAbsTrace;
		/// Abstract trace immediately after an ICFGNode.
		Map<const ICFGNode*, AbstractState> postAbsTrace;

	 private:
		AbstractExecutionHelper bufOverflowHelper;

		Set<const CallICFGNode*> assert_points;

		AbsExtAPI* utils;
	};

} // namespace SVF
