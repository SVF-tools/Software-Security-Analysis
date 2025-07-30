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

#include "Assignment_3.h"
#include "Util/Options.h"
#include "Util/WorkList.h"

using namespace SVF;
using namespace SVFUtil;

/// TODO : Implement the state updates for Copy, Binary, Store, Load, Gep, Phi
void AbstractExecution::updateStateOnCopy(const CopyStmt* copy) {
	/// TODO: your code starts from here

}

/// Find the comparison predicates in "class BinaryOPStmt:OpCode" under SVF/svf/include/SVFIR/SVFStatements.h
/// You are required to handle predicates (The program is assumed to have signed ints and also interger-overflow-free),
/// including Add, FAdd, Sub, FSub, Mul, FMul, SDiv, FDiv, UDiv, SRem, FRem, URem, Xor, And, Or, AShr, Shl, LShr
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
			AbstractState& as = getAbsStateFromTrace(gep->getICFGNode());
			NodeID lhs = gep->getLHSVarID();
			NodeID rhs = gep->getRHSVarID();
			updateGepObjOffsetFromBase(as, as[lhs].getAddrs(), as[rhs].getAddrs(), as.getByteOffset(gep));

			/// TODO: your code starts from here

		}
	}
}

/**
 * @brief Handle ICFG nodes in a cycle using widening and narrowing operators
 * 
 * This function implements abstract interpretation for cycles in the ICFG using widening and narrowing
 * operators to ensure termination. It processes all ICFG nodes within a cycle and implements
 * widening-narrowing iteration to reach fixed points twice: once for widening (to ensure termination)
 * and once for narrowing (to improve precision).
 * 
 * @param cycle The WTO cycle containing ICFG nodes to be processed
 * @return void
 */
void AbstractExecution::handleICFGCycle(const ICFGCycleWTO* cycle) {
	// Get execution states from in edges
	bool is_feasible = mergeStatesFromPredecessors(cycle->head()->getICFGNode(), preAbsTrace[cycle->head()->getICFGNode()]);
	if (!is_feasible) {
		return;
	} else {
		AbstractState pre_as = preAbsTrace[cycle->head()->getICFGNode()];
		// set -widen-delay
		s32_t widen_delay = Options::WidenDelay();
		bool increasing = true;
		/// TODO: your code starts from here


	}
}


/// Abstract state updates on an AddrStmt
void AbstractExecution::updateStateOnAddr(const AddrStmt* addr) {
	const ICFGNode* node = addr->getICFGNode();
	AbstractState& as = getAbsStateFromTrace(node);
	as.initObjVar(SVFUtil::cast<ObjVar>(addr->getRHSVar()));
	as[addr->getLHSVarID()] = as[addr->getRHSVarID()];
}

/// Abstract state updates on an CmpStmt
void AbstractExecution::updateStateOnCmp(const CmpStmt* cmp) {
	const ICFGNode* node = cmp->getICFGNode();
	AbstractState& as = getAbsStateFromTrace(node);
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
	AbstractState& as = getAbsStateFromTrace(node);
	NodeID lhs = call->getLHSVarID();
	NodeID rhs = call->getRHSVarID();
	as[lhs] = as[rhs];
}

/// Abstract state updates on an RetPE
void AbstractExecution::updateStateOnRet(const RetPE* retPE) {
	const ICFGNode* node = retPE->getICFGNode();
	AbstractState& as = getAbsStateFromTrace(node);
	NodeID lhs = retPE->getLHSVarID();
	NodeID rhs = retPE->getRHSVarID();
	as[lhs] = as[rhs];
}

/// Abstract state updates on an SelectStmt
void AbstractExecution::updateStateOnSelect(const SelectStmt* select) {
	const ICFGNode* node = select->getICFGNode();
	AbstractState& as = getAbsStateFromTrace(node);
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

void AbstractExecution::updateStateOnExtCall(const SVF::CallICFGNode* extCallNode) {
	std::string funcName = extCallNode->getCalledFunction()->getName();
	//TODO: handle external calls
	// void mem_insert(void *buffer, const void *data, size_t data_size, size_t position);
	if (funcName == "mem_insert") {
		// check sizeof(buffer) > position + data_size
		/// TODO: your code starts from here

	}
	// void str_insert(void *buffer, const void *data, size_t position);
	else if (funcName == "str_insert") {
		// check sizeof(buffer) > position + strlen(data)
		/// TODO: your code starts from here

	}
}