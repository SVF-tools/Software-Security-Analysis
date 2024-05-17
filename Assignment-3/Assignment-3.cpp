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
#include "SVFIR/SVFIR.h"
#include "Util/Options.h"
#include "Util/WorkList.h"

using namespace SVF;
using namespace SVFUtil;

/// handle wto cycle (loop)
void AbsExe::handleWTONode(const ICFGNode* node) {
	if (!propagateStateIfFeasible(node)) {
		// No AS on the in edges - Infeasible block
		return;
	}
	else {
		// Has AS on the in edges - Feasible block
		// Get execution state from in edges
		_postAbsTrace[node] = _preAbsTrace[node];
	}

	std::deque<const ICFGNode*> worklist;

	// handle SVF Stmt
	for (const SVFStmt* stmt : node->getSVFStmts()) {
		updateAbsState(stmt);
		bufOverflowDetection(stmt);
	}
	// inlining the callee by calling handleFunc for the callee function
	if (const CallICFGNode* callnode = SVFUtil::dyn_cast<CallICFGNode>(node)) {
		// if callnode name is svf_assert or OVERFLOW
		std::string funName = SVFUtil::getCallee(callnode->getCallSite())->getName();
		if (funName == "OVERFLOW" || funName == "svf_assert") {
			handleStubFunctions(callnode);
		}
		else {
			handleCallSite(callnode);
		}
	}
	else {
	}
}

/// This is to check whether your abstract interpretation results are expected.
/// The oracle (expected results) are embedded by the following stub functions.
/// For svf_assert(expr), the expr must hold.
/// For OVERFLOW(object, offset_access), it represents that the object_sz (i.e., size of the object) is less or equal
/// than offset_access
void AbsExe::handleStubFunctions(const SVF::CallICFGNode* callnode) {
	if (SVFUtil::getCallee(callnode->getCallSite())->getName() == "svf_assert") {
		// if the condition is false, then the program is infeasible
		CallSite cs = callnode->getCallSite();
		const CallICFGNode* callNode =
		    SVFUtil::dyn_cast<CallICFGNode>(_svfir->getICFG()->getICFGNode(cs.getInstruction()));
		_checkpoints.erase(callNode);
		u32_t arg0 = _svfir->getValueNode(cs.getArgument(0));
		AbstractState& as = getAbsState(callNode);
		if (as[arg0].getInterval().is_infinite()) {
			SVFUtil::errs() << "svf_assert Fail. " << cs.getInstruction()->toString() << "\n";
			assert(false);
		}
		else {
			as[arg0].getInterval().meet_with(IntervalValue(1, 1));
			if (as[arg0].getInterval().equals(IntervalValue(1, 1))) {
				SVFUtil::errs() << SVFUtil::sucMsg("Your implementation successfully verified the svf_assert!\n");
			}
			else {
				SVFUtil::errs() << "Your implementation failed to verify the svf_assert!"
				                << cs.getInstruction()->toString() << "\n";
				assert(false);
			}
		}
		return;
	}
	else if (SVFUtil::getCallee(callnode->getCallSite())->getName() == "OVERFLOW") {
		// if the condition is false, then the program is infeasible
		CallSite cs = callnode->getCallSite();
		const CallICFGNode* callNode =
		    SVFUtil::dyn_cast<CallICFGNode>(_svfir->getICFG()->getICFGNode(cs.getInstruction()));
		_checkpoints.erase(callNode);
		u32_t arg0 = _svfir->getValueNode(cs.getArgument(0));
		u32_t arg1 = _svfir->getValueNode(cs.getArgument(1));

		AbstractState& as = getAbsState(callnode);
		AbstractValue gepRhsVal = as[arg0];
		if (gepRhsVal.isAddr()) {
			for (const auto& addr : gepRhsVal.getAddrs()) {
				s64_t baseObj = _svfir->getBaseObjVar(AbstractState::getInternalID(addr));
				IntervalValue object_sz = obj2size[baseObj];
				IntervalValue access_offset = as[arg1].getInterval();
				bool res = object_sz.leq(access_offset) ? false : true;
				if (!res) {
					std::cerr << "Your implementation successfully detected the buffer overflow\n";
				}
				else {
					SVFUtil::errs() << "Your implementation failed to detect the buffer overflow!"
					                << cs.getInstruction()->toString() << "\n";
					assert(false);
				}
			}
		}
		else {
			SVFUtil::errs() << "Your implementation failed to detect the buffer overflow!"
			                << cs.getInstruction()->toString() << "\n";
			assert(false);
		}
	}
}

/// Handle each type of SVFStmt for state updates
void AbsExe::updateAbsState(const SVFStmt* stmt) {
	if (const AddrStmt* addr = SVFUtil::dyn_cast<AddrStmt>(stmt)) {
		updateStateOnAddr(addr);
	}
	else if (const BinaryOPStmt* binary = SVFUtil::dyn_cast<BinaryOPStmt>(stmt)) {
		updateStateOnBinary(binary);
	}
	else if (const CmpStmt* cmp = SVFUtil::dyn_cast<CmpStmt>(stmt)) {
		updateStateOnCmp(cmp);
	}
	else if (const LoadStmt* load = SVFUtil::dyn_cast<LoadStmt>(stmt)) {
		updateStateOnLoad(load);
	}
	else if (const StoreStmt* store = SVFUtil::dyn_cast<StoreStmt>(stmt)) {
		updateStateOnStore(store);
	}
	else if (const CopyStmt* copy = SVFUtil::dyn_cast<CopyStmt>(stmt)) {
		updateStateOnCopy(copy);
	}
	else if (const GepStmt* gep = SVFUtil::dyn_cast<GepStmt>(stmt)) {
		updateStateOnGep(gep);
	}
	else if (const PhiStmt* phi = SVFUtil::dyn_cast<PhiStmt>(stmt)) {
		updateStateOnPhi(phi);
	}
	else if (const CallPE* callPE = SVFUtil::dyn_cast<CallPE>(stmt)) {
		updateStateOnCall(callPE);
	}
	else if (const RetPE* retPE = SVFUtil::dyn_cast<RetPE>(stmt)) {
		updateStateOnRet(retPE);
	}
	else if (const SelectStmt* select = SVFUtil::dyn_cast<SelectStmt>(stmt)) {
		updateStateOnSelect(select);
	}
	else if (SVFUtil::isa<UnaryOPStmt>(stmt) || SVFUtil::isa<BranchStmt>(stmt)) {
		// nothing needs to be done here as BranchStmt is handled in hasBranchES
	}
	else
		assert(false && "implement this part");
}

/// TODO: handle object allocation and record its size
/// TODO: handle GepStmt and detect buffer overflow
void AbsExe::bufOverflowDetection(const SVF::SVFStmt* stmt) {
	if (!SVFUtil::isa<CallICFGNode>(stmt->getICFGNode())) {
		if (const AddrStmt* addr = SVFUtil::dyn_cast<AddrStmt>(stmt)) {
			/// TODO: your code starts from here
		}
		else if (const GepStmt* gep = SVFUtil::dyn_cast<GepStmt>(stmt)) {
			/// TODO: your code starts from here
		}
		else {
			// nothing to do here
		}
	}
}

/// TODO : Implement the handleCycle function
void AbsExe::handleCycle(const ICFGWTOCycle* cycle) {
	// Get execution states from in edges
	if (!propagateStateIfFeasible(cycle->head())) {
		// No ES on the in edges - Infeasible block
		return;
	}
	AbstractState pre_as = _preAbsTrace[cycle->head()];
	// set -widen-delay
	s32_t widen_delay = Options::WidenDelay();
	bool incresing = true;

	/// TODO: your code starts from here
}

/// TODO : Implement the state updates for Copy, Store, Load and Gep
void AbsExe::updateStateOnCopy(const CopyStmt* copy) {
	/// TODO: your code starts from here
}

void AbsExe::updateStateOnStore(const StoreStmt* store) {
	/// TODO: your code starts from here
}

void AbsExe::updateStateOnLoad(const LoadStmt* load) {
	/// TODO: your code starts from here
}

void AbsExe::updateStateOnGep(const GepStmt* gep) {
	/// TODO: your code starts from here
}

/// Abstract state updates on an AddrStmt
void AbsExe::updateStateOnAddr(const AddrStmt* addr) {
	const ICFGNode* node = addr->getICFGNode();
	AbstractState& as = getAbsState(node);
	initSVFVar(as, addr->getRHSVarID());
	if (as.inVarToValTable(addr->getRHSVarID())) {
		as[addr->getLHSVarID()] = as[addr->getRHSVarID()];
	}
	else if (as.inVarToAddrsTable(addr->getRHSVarID())) {
		as[addr->getLHSVarID()] = as[addr->getRHSVarID()];
	}
	else {
		assert(false && "not number or virtual addrs?");
	}
}

/// Abstract state updates on an CmpStmt
void AbsExe::updateStateOnCmp(const CmpStmt* cmp) {
	const ICFGNode* node = cmp->getICFGNode();
	AbstractState& as = getAbsState(node);
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
		case CmpStmt::FCMP_UEQ:
			resVal = (lhs == rhs);
			// resVal = (lhs.getInterval() == rhs.getInterval());
			break;
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
		AbstractValue &lhs = as.getAddrs(op0), &rhs = as.getAddrs(op1);
		assert(!lhs.getAddrs().empty() && !rhs.getAddrs().empty() && "empty address?");
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
void AbsExe::updateStateOnCall(const CallPE* call) {
	const ICFGNode* node = call->getICFGNode();
	AbstractState& as = getAbsState(node);
	NodeID lhs = call->getLHSVarID();
	NodeID rhs = call->getRHSVarID();
	if (as.inVarToValTable(rhs) || as.inVarToAddrsTable(rhs)) {
		as[lhs] = as[rhs];
	}
}

/// Abstract state updates on an RetPE
void AbsExe::updateStateOnRet(const RetPE* retPE) {
	const ICFGNode* node = retPE->getICFGNode();
	AbstractState& as = getAbsState(node);
	NodeID lhs = retPE->getLHSVarID();
	NodeID rhs = retPE->getRHSVarID();
	if (as.inVarToValTable(rhs) || as.inVarToAddrsTable(rhs)) {
		as[lhs] = as[rhs];
	}
}

/// Abstract state updates on an PhiStmt
void AbsExe::updateStateOnPhi(const PhiStmt* phi) {
	const ICFGNode* node = phi->getICFGNode();
	AbstractState& as = getAbsState(node);
	u32_t res = phi->getResID();
	AbstractValue rhs(AbstractValue::UnknownType);
	for (u32_t i = 0; i < phi->getOpVarNum(); i++) {
		NodeID curId = phi->getOpVarID(i);

		if (as.inVarToValTable(curId) || as.inVarToAddrsTable(curId)) {
			rhs.join_with(as[curId]);
		}
	}
	if (!rhs.isUnknown())
		as[res] = rhs;
}

/// Abstract state updates on an BinaryOPStmt
void AbsExe::updateStateOnBinary(const BinaryOPStmt* binary) {
	const ICFGNode* node = binary->getICFGNode();
	AbstractState& as = getAbsState(node);
	u32_t op0 = binary->getOpVarID(0);
	u32_t op1 = binary->getOpVarID(1);
	u32_t res = binary->getResID();
	if (!as.inVarToValTable(op0))
		as[op0] = IntervalValue::top();
	if (!as.inVarToValTable(op1))
		as[op1] = IntervalValue::top();
	if (as.inVarToValTable(op0) && as.inVarToValTable(op1)) {
		AbstractValue &lhs = as[op0], &rhs = as[op1];
		AbstractValue resVal;
		switch (binary->getOpcode()) {
			// TODO: write your code here to handle binary operations
		}
		as[res] = resVal;
	}
}

/// Abstract state updates on an SelectStmt
void AbsExe::updateStateOnSelect(const SelectStmt* select) {
	const ICFGNode* node = select->getICFGNode();
	AbstractState& as = getAbsState(node);
	u32_t res = select->getResID();
	u32_t tval = select->getTrueValue()->getId();
	u32_t fval = select->getFalseValue()->getId();
	u32_t cond = select->getCondition()->getId();
	if (as.inVarToValTable(tval) && as.inVarToValTable(fval) && as.inVarToValTable(cond)) {
		if (as[cond].getInterval().is_numeral()) {
			as[res] = as[cond].getInterval().is_zero() ? as[fval] : as[tval];
		}
		else {
			as[res] = as[cond];
		}
	}
	else if (as.inVarToAddrsTable(tval) && as.inVarToAddrsTable(fval) && as.inVarToValTable(cond)) {
		if (as[cond].getInterval().is_numeral()) {
			as.getAddrs(res) = as[cond].getInterval().is_zero() ? as.getAddrs(fval) : as.getAddrs(tval);
		}
	}
}

void AbsExe::analyse() {
	// handle Global ICFGNode of SVFModule
	handleGlobalNode();
	getAbsState(_icfg->getGlobalICFGNode())[PAG::getPAG()->getBlkPtr()] = IntervalValue::top();
	if (const SVFFunction* fun = _svfir->getModule()->getSVFFunction("main")) {
		ICFGWTO* wto = _funcToWTO[fun];
		// set function entry ES
		for (auto it = wto->begin(); it != wto->end(); ++it) {
			const ICFGWTOComp* cur = *it;
			if (const ICFGWTONode* vertex = SVFUtil::dyn_cast<ICFGWTONode>(cur)) {
				handleWTONode(vertex->node());
			}
			else if (const ICFGWTOCycle* cycle = SVFUtil::dyn_cast<ICFGWTOCycle>(cur)) {
				handleCycle(cycle);
			}
			else {
				assert(false && "unknown WTO type!");
			}
		}
	}
}
