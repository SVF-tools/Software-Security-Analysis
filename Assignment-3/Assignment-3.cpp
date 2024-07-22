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
 * Abstract Interpretation and buffer overflow detection
 *
 * Created on: Feb 19, 2024
 */

#include "Assignment-3.h"
#include "Util/Options.h"
#include "Util/WorkList.h"

using namespace SVF;
using namespace SVFUtil;

/// TODO : Implement the state updates for Copy, Binary, Store, Load, Gep, Phi
void AbstractExecution::updateStateOnCopy(const CopyStmt* copy) {
	/// TODO: your code starts from here

}

void AbstractExecution::updateStateOnBinary(const BinaryOPStmt* binary) {
	/// TODO: your code starts from here

}

void AbstractExecution::updateStateOnStore(const StoreStmt* store) {
	/// TODO: your code starts from here

}

void AbstractExecution::updateStateOnLoad(const LoadStmt* load) {
	/// TODO: your code starts from here

}

void AbstractExecution::updateStateOnGep(const GepStmt* gep) {
	/// TODO: your code starts from here

}

void AbstractExecution::updateStateOnPhi(const PhiStmt* phi) {
	/// TODO: your code starts from here

}

/// TODO: handle GepStmt `lhs = rhs + off` and detect buffer overflow
// Step 1: For each `obj \in pts(rhs)`, get the size of allocated baseobj (entire mem object) via `obj_size = svfir->getBaseObj(objId)->getByteSizeOfObj();`
// There is a buffer overflow if `accessOffset.ub() >= obj_size`, where accessOffset is obtained via `getAccessOffset`
// Step 2: invoke `reportBufOverflow` with the current ICFGNode if an overflow is detected
void AbstractExecution::bufOverflowDetection(const SVF::SVFStmt* stmt) {
	if (!SVFUtil::isa<CallICFGNode>(stmt->getICFGNode())) {
		if (const GepStmt* gep = SVFUtil::dyn_cast<GepStmt>(stmt)) {
			AEState& as = getAbsStateFromTrace(gep->getICFGNode());
			NodeID lhs = gep->getLHSVarID();
			NodeID rhs = gep->getRHSVarID();
			updateGepObjOffsetFromBase(as[lhs].getAddrs(), as[rhs].getAddrs(), as.getByteOffset(gep));

			/// TODO: your code starts from here

		}
	}
}

/// TODO : Implement the handleCycleWTO function
void AbstractExecution::handleCycleWTO(const ICFGCycleWTO* cycle) {
	// Get execution states from in edges
	bool is_feasible = mergeStatesFromPredecessors(cycle->head()->node(), preAbsTrace[cycle->head()->node()]);
	if (!is_feasible) {
		return;
	} else {
		AEState pre_as = preAbsTrace[cycle->head()->node()];
		// set -widen-delay
		s32_t widen_delay = Options::WidenDelay();
		bool increasing = true;
		/// TODO: your code starts from here


	}
}


/// Abstract state updates on an AddrStmt
void AbstractExecution::updateStateOnAddr(const AddrStmt* addr) {
	const ICFGNode* node = addr->getICFGNode();
	AEState& as = getAbsStateFromTrace(node);
	as.initObjVar(SVFUtil::cast<ObjVar>(addr->getRHSVar()));
	as[addr->getLHSVarID()] = as[addr->getRHSVarID()];
}

/// Abstract state updates on an CmpStmt
void AbstractExecution::updateStateOnCmp(const CmpStmt* cmp) {
	const ICFGNode* node = cmp->getICFGNode();
	AEState& as = getAbsStateFromTrace(node);
	u32_t op0 = cmp->getOpVarID(0);
	u32_t op1 = cmp->getOpVarID(1);
	u32_t res = cmp->getResID();
	if (as.inVarToValTable(op0) && as.inVarToValTable(op1)) {
		IntervalValue resVal;
		IntervalValue &lhs = as[op0].getInterval(), &rhs = as[op1].getInterval();
		// AbstractValue
		auto predicate = cmp->getPredicate();
		switch (predicate) {
		case CmpStmt::ICMP_EQ:
		case CmpStmt::FCMP_OEQ:
		case CmpStmt::FCMP_UEQ: resVal = (lhs == rhs); break;
		case CmpStmt::ICMP_NE:
		case CmpStmt::FCMP_ONE:
		case CmpStmt::FCMP_UNE: resVal = (lhs != rhs); break;
		case CmpStmt::ICMP_UGT:
		case CmpStmt::ICMP_SGT:
		case CmpStmt::FCMP_OGT:
		case CmpStmt::FCMP_UGT: resVal = (lhs > rhs); break;
		case CmpStmt::ICMP_UGE:
		case CmpStmt::ICMP_SGE:
		case CmpStmt::FCMP_OGE:
		case CmpStmt::FCMP_UGE: resVal = (lhs >= rhs); break;
		case CmpStmt::ICMP_ULT:
		case CmpStmt::ICMP_SLT:
		case CmpStmt::FCMP_OLT:
		case CmpStmt::FCMP_ULT: resVal = (lhs < rhs); break;
		case CmpStmt::ICMP_ULE:
		case CmpStmt::ICMP_SLE:
		case CmpStmt::FCMP_OLE:
		case CmpStmt::FCMP_ULE: resVal = (lhs <= rhs); break;
		case CmpStmt::FCMP_FALSE: resVal = IntervalValue(0, 0); break;
		case CmpStmt::FCMP_TRUE: resVal = IntervalValue(1, 1); break;
		default: {
			assert(false && "undefined compare: ");
		}
		}
		as[res] = resVal;
	}
	else if (as.inVarToAddrsTable(op0) && as.inVarToAddrsTable(op1)) {
		IntervalValue resVal;
		AbstractValue &lhs = as[op0], &rhs = as[op1];
		auto predicate = cmp->getPredicate();
		switch (predicate) {
		case CmpStmt::ICMP_EQ:
		case CmpStmt::FCMP_OEQ:
		case CmpStmt::FCMP_UEQ: {
			if (lhs.getAddrs().size() == 1 && rhs.getAddrs().size() == 1) {
				resVal = IntervalValue(lhs.equals(rhs));
			}
			else {
				if (lhs.getAddrs().hasIntersect(rhs.getAddrs())) {
					resVal = IntervalValue::top();
				}
				else {
					resVal = IntervalValue(0);
				}
			}
			break;
		}
		case CmpStmt::ICMP_NE:
		case CmpStmt::FCMP_ONE:
		case CmpStmt::FCMP_UNE: {
			if (lhs.getAddrs().size() == 1 && rhs.getAddrs().size() == 1) {
				resVal = IntervalValue(!lhs.equals(rhs));
			}
			else {
				if (lhs.getAddrs().hasIntersect(rhs.getAddrs())) {
					resVal = IntervalValue::top();
				}
				else {
					resVal = IntervalValue(1);
				}
			}
			break;
		}
		case CmpStmt::ICMP_UGT:
		case CmpStmt::ICMP_SGT:
		case CmpStmt::FCMP_OGT:
		case CmpStmt::FCMP_UGT: {
			if (lhs.getAddrs().size() == 1 && rhs.getAddrs().size() == 1) {
				resVal = IntervalValue(*lhs.getAddrs().begin() > *rhs.getAddrs().begin());
			}
			else {
				resVal = IntervalValue::top();
			}
			break;
		}
		case CmpStmt::ICMP_UGE:
		case CmpStmt::ICMP_SGE:
		case CmpStmt::FCMP_OGE:
		case CmpStmt::FCMP_UGE: {
			if (lhs.getAddrs().size() == 1 && rhs.getAddrs().size() == 1) {
				resVal = IntervalValue(*lhs.getAddrs().begin() >= *rhs.getAddrs().begin());
			}
			else {
				resVal = IntervalValue::top();
			}
			break;
		}
		case CmpStmt::ICMP_ULT:
		case CmpStmt::ICMP_SLT:
		case CmpStmt::FCMP_OLT:
		case CmpStmt::FCMP_ULT: {
			if (lhs.getAddrs().size() == 1 && rhs.getAddrs().size() == 1) {
				resVal = IntervalValue(*lhs.getAddrs().begin() < *rhs.getAddrs().begin());
			}
			else {
				resVal = IntervalValue::top();
			}
			break;
		}
		case CmpStmt::ICMP_ULE:
		case CmpStmt::ICMP_SLE:
		case CmpStmt::FCMP_OLE:
		case CmpStmt::FCMP_ULE: {
			if (lhs.getAddrs().size() == 1 && rhs.getAddrs().size() == 1) {
				resVal = IntervalValue(*lhs.getAddrs().begin() <= *rhs.getAddrs().begin());
			}
			else {
				resVal = IntervalValue::top();
			}
			break;
		}
		case CmpStmt::FCMP_FALSE: resVal = IntervalValue(0, 0); break;
		case CmpStmt::FCMP_TRUE: resVal = IntervalValue(1, 1); break;
		default: {
			assert(false && "undefined compare: ");
		}
		}
		as[res] = resVal;
	}
}

/// Abstract state updates on an CallPE
void AbstractExecution::updateStateOnCall(const CallPE* call) {
	const ICFGNode* node = call->getICFGNode();
	AEState& as = getAbsStateFromTrace(node);
	NodeID lhs = call->getLHSVarID();
	NodeID rhs = call->getRHSVarID();
	as[lhs] = as[rhs];
}

/// Abstract state updates on an RetPE
void AbstractExecution::updateStateOnRet(const RetPE* retPE) {
	const ICFGNode* node = retPE->getICFGNode();
	AEState& as = getAbsStateFromTrace(node);
	NodeID lhs = retPE->getLHSVarID();
	NodeID rhs = retPE->getRHSVarID();
	as[lhs] = as[rhs];
}

/// Abstract state updates on an SelectStmt
void AbstractExecution::updateStateOnSelect(const SelectStmt* select) {
	const ICFGNode* node = select->getICFGNode();
	AEState& as = getAbsStateFromTrace(node);
	u32_t res = select->getResID();
	u32_t tval = select->getTrueValue()->getId();
	u32_t fval = select->getFalseValue()->getId();
	u32_t cond = select->getCondition()->getId();
	if (as[cond].getInterval().is_numeral()) {
		as[res] = as[cond].getInterval().is_zero() ? as[fval] : as[tval];
	}
	else {
		as[res].join_with(as[tval]);
		as[res].join_with(as[fval]);
	}
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

	// Handle the global node
	handleGlobalNode();

	// Process the main function if it exists
	if (const SVFFunction* fun = svfir->getModule()->getSVFFunction("main")) {
		ICFGWTO* wto = funcToWTO[fun];
		handleWTOComponents(wto->getWTOComponents());
	}
}

/**
 * @brief Hanlde two types of WTO components (singleton and cycle)
 */
void AbstractExecution::handleWTOComponents(const std::list<const ICFGWTOComp*>& wtoComps) {
	for (const ICFGWTOComp* wtoNode : wtoComps) {
		if (const ICFGSingletonWTO* node = SVFUtil::dyn_cast<ICFGSingletonWTO>(wtoNode)) {
			handleSingletonWTO(node);
		}
		// Handle WTO cycles
		else if (const ICFGCycleWTO* cycle = SVFUtil::dyn_cast<ICFGCycleWTO>(wtoNode)) {
			handleCycleWTO(cycle);
		}
		// Assert false for unknown WTO types
		else {
			assert(false && "unknown WTO type!");
		}
	}
}

/**
 * @brief Handle a Weak Topological Order (WTO) node in the control flow graph
 *
 * This function processes a WTO node, which typically represents a loop in the control flow graph.
 * It first propagates the state to the node if feasible. Then, it updates the abstract state
 * and performs buffer overflow detection for each statement in the node. If the node is a call
 * node, it handles the call site or specific stub functions.
 *
 * @param node The WTO node to be processed
 */
void AbstractExecution::handleSingletonWTO(const ICFGSingletonWTO* singletonWTO) {
	const ICFGNode* node = singletonWTO->node();
	// Propagate the states from predecessors to the current node and return true if the control-flow is feasible
	bool is_feasible = mergeStatesFromPredecessors(node, preAbsTrace[node]);
	if (is_feasible) {
		postAbsTrace[node] = preAbsTrace[node];
	}
	else {
		return;
	}

	std::deque<const ICFGNode*> worklist;

	// Handle each SVF statement in the node
	for (const SVFStmt* stmt : node->getSVFStmts()) {
		updateAbsState(stmt); // Update the abstract state with the current statement
		bufOverflowDetection(stmt); // Perform buffer overflow detection for the statement
	}

	// Handle call nodes by inlining the callee function
	if (const CallICFGNode* callnode = SVFUtil::dyn_cast<CallICFGNode>(node)) {
		// Get the name of the callee function
		std::string funName = SVFUtil::getCallee(callnode->getCallSite())->getName();
		if (funName == "OVERFLOW" || funName == "svf_assert") {
			handleStubFunctions(callnode); // Handle specific stub functions
		}
		else {
			handleCallSite(callnode); // Handle general call sites
		}
	}
}

/**
 * @brief Handle state updates for each type of SVF statement
 *
 * This function updates the abstract state based on the type of SVF statement provided.
 * It dispatches the handling of each specific statement type to the corresponding update function.
 *
 * @param stmt The SVF statement for which the state needs to be updated
 */
void AbstractExecution::updateAbsState(const SVFStmt* stmt) {
	// Handle address statements
	if (const AddrStmt* addr = SVFUtil::dyn_cast<AddrStmt>(stmt)) {
		updateStateOnAddr(addr);
	}
	// Handle binary operation statements
	else if (const BinaryOPStmt* binary = SVFUtil::dyn_cast<BinaryOPStmt>(stmt)) {
		updateStateOnBinary(binary);
	}
	// Handle comparison statements
	else if (const CmpStmt* cmp = SVFUtil::dyn_cast<CmpStmt>(stmt)) {
		updateStateOnCmp(cmp);
	}
	// Handle load statements
	else if (const LoadStmt* load = SVFUtil::dyn_cast<LoadStmt>(stmt)) {
		updateStateOnLoad(load);
	}
	// Handle store statements
	else if (const StoreStmt* store = SVFUtil::dyn_cast<StoreStmt>(stmt)) {
		updateStateOnStore(store);
	}
	// Handle copy statements
	else if (const CopyStmt* copy = SVFUtil::dyn_cast<CopyStmt>(stmt)) {
		updateStateOnCopy(copy);
	}
	// Handle GEP (GetElementPtr) statements
	else if (const GepStmt* gep = SVFUtil::dyn_cast<GepStmt>(stmt)) {
		updateStateOnGep(gep);
	}
	// Handle phi statements
	else if (const PhiStmt* phi = SVFUtil::dyn_cast<PhiStmt>(stmt)) {
		updateStateOnPhi(phi);
	}
	// Handle call procedure entries
	else if (const CallPE* callPE = SVFUtil::dyn_cast<CallPE>(stmt)) {
		updateStateOnCall(callPE);
	}
	// Handle return procedure entries
	else if (const RetPE* retPE = SVFUtil::dyn_cast<RetPE>(stmt)) {
		updateStateOnRet(retPE);
	}
	// Handle select statements
	else if (const SelectStmt* select = SVFUtil::dyn_cast<SelectStmt>(stmt)) {
		updateStateOnSelect(select);
	}
	// Handle unary operations and branch statements (no action needed)
	else if (SVFUtil::isa<UnaryOPStmt>(stmt) || SVFUtil::isa<BranchStmt>(stmt)) {
		// Nothing needs to be done here as BranchStmt is handled in hasBranchES
	}
	// Assert false for unsupported statement types
	else {
		assert(false && "implement this part");
	}
}


/**
 * @brief Handle a call site in the control flow graph
 *
 * This function processes a call site by updating the abstract state, handling the called function,
 * and managing the call stack. It resumes the execution state after the function call.
 *
 * @param node The call site node to be handled
 */
void AbstractExecution::handleCallSite(const CallICFGNode* callNode) {
	// Get the abstract state associated with the call node
	AEState& as = getAbsStateFromTrace(callNode);

	// Get the callee function associated with the call site
	const SVFFunction* callee = SVFUtil::getCallee(callNode->getCallSite());

	if (recursiveFuns.find(callee) != recursiveFuns.end()) {
		// skip recursive functions
		return;
	}
	else {
		// Push the call node onto the call site stack
		callSiteStack.push_back(callNode);
		// Handle the callee function
		ICFGWTO* wto = funcToWTO[callee];
		handleWTOComponents(wto->getWTOComponents());

		// Pop the call node from the call site stack
		callSiteStack.pop_back();
	}
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

void AbstractExecution::handleStubFunctions(const SVF::CallICFGNode* callnode) {
	// Handle the 'svf_assert' stub function
	if (SVFUtil::getCallee(callnode->getCallSite())->getName() == "svf_assert") {
		assert_points.insert(callnode);
		// If the condition is false, the program is infeasible
		CallSite cs = callnode->getCallSite();
		const CallICFGNode* callNode =
		    SVFUtil::dyn_cast<CallICFGNode>(svfir->getICFG()->getICFGNode(cs.getInstruction()));
		u32_t arg0 = svfir->getValueNode(cs.getArgument(0));
		AEState& as = getAbsStateFromTrace(callNode);

		// Check if the interval for the argument is infinite
		if (as[arg0].getInterval().is_infinite()) {
			SVFUtil::errs() << "svf_assert Fail. " << cs.getInstruction()->toString() << "\n";
			assert(false);
		}
		else {
			as[arg0].getInterval().meet_with(IntervalValue(1, 1));
			if (as[arg0].getInterval().equals(IntervalValue(1, 1))) {
				std::stringstream ss;
				ss << "The assertion (" << callnode->toString() << ")"
				   << " is successfully verified!!\n";
				SVFUtil::outs() << ss.str() << std::endl;
			}
			else {
				std::stringstream ss;
				ss << "The assertion (" << callnode->toString() << ")"
				   << " is unsatisfiable!!\n";
				SVFUtil::outs() << ss.str() << std::endl;
				assert(false);
			}
		}
		return;
	}
	// Handle the 'OVERFLOW' stub function
	else if (SVFUtil::getCallee(callnode->getCallSite())->getName() == "OVERFLOW") {
		// If the condition is false, the program is infeasible
		assert_points.insert(callnode);
		CallSite cs = callnode->getCallSite();
		const CallICFGNode* callNode =
		    SVFUtil::dyn_cast<CallICFGNode>(svfir->getICFG()->getICFGNode(cs.getInstruction()));
		u32_t arg0 = svfir->getValueNode(cs.getArgument(0));
		u32_t arg1 = svfir->getValueNode(cs.getArgument(1));

		AEState& as = getAbsStateFromTrace(callnode);
		AbstractValue gepRhsVal = as[arg0];

		// Check if the RHS value is an address
		if (gepRhsVal.isAddr()) {
			bool overflow = false;
			for (const auto& addr : gepRhsVal.getAddrs()) {
				u64_t access_offset = as[arg1].getInterval().getIntNumeral();
				NodeID objId = AEState::getInternalID(addr);
				const GepObjVar* gepLhsObjVar = SVFUtil::cast<GepObjVar>(svfir->getGNode(objId));
				auto size = svfir->getBaseObj(objId)->getByteSizeOfObj();
				if (bufOverflowHelper.hasGepObjOffsetFromBase(gepLhsObjVar)) {
					overflow = (bufOverflowHelper.getGepObjOffsetFromBase(gepLhsObjVar).ub().getIntNumeral() + access_offset
					            >= size);
				}
				else {
					assert(false && "pointer not found in gepObjOffsetFromBase");
				}
			}
			if (overflow) {
				std::cerr << "Your implementation successfully detected the buffer overflow\n";
			}
			else {
				SVFUtil::errs() << "Your implementation failed to detect the buffer overflow!"
				                << cs.getInstruction()->toString() << "\n";
				assert(false);
			}
		}
		else {
			SVFUtil::errs() << "Your implementation failed to detect the buffer overflow!"
			                << cs.getInstruction()->toString() << "\n";
			assert(false);
		}
	}
}
