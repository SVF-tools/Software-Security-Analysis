#include "Assignment-3.h"
#include "SVFIR/SVFIR.h"
using namespace SVF;
void AbsExe::initSVFVar(AbstractState& as, u32_t varId) {
	if (as.inVarToValTable(varId))
		return;
	SVFIR* svfir = PAG::getPAG();
	SVFVar* svfVar = svfir->getGNode(varId);
	// write objvar into cache instead of exestate
	if (const ObjVar* objVar = SVFUtil::dyn_cast<ObjVar>(svfVar)) {
		initObjVar(as, objVar, varId);
		return;
	}
	else {
		assert(false && "not an obj var?");
	}
}

void AbsExe::initObjVar(AbstractState& as, const ObjVar* objVar, u32_t varId) {
	if (objVar->hasValue()) {
		const MemObj* obj = objVar->getMemObj();
		/// constant data
		if (obj->isConstDataOrConstGlobal() || obj->isConstantArray() || obj->isConstantStruct()) {
			if (const SVFConstantInt* consInt = SVFUtil::dyn_cast<SVFConstantInt>(obj->getValue())) {
				s64_t numeral = consInt->getSExtValue();
				as[varId] = IntervalValue(numeral, numeral);
			}
			else if (const SVFConstantFP* consFP = SVFUtil::dyn_cast<SVFConstantFP>(obj->getValue()))
				as[varId] = IntervalValue(consFP->getFPValue(), consFP->getFPValue());
			else if (SVFUtil::isa<SVFConstantNullPtr>(obj->getValue()))
				as[varId] = IntervalValue(0, 0);
			else if (SVFUtil::isa<SVFGlobalValue>(obj->getValue())) {
				as[varId] = AddressValue(AbstractState::getVirtualMemAddress(varId));
			}

			else if (obj->isConstantArray() || obj->isConstantStruct())
				as[varId] = IntervalValue::top();
			else
				as[varId] = IntervalValue::top();
		}
		else
			as[varId] = AddressValue(AbstractState::getVirtualMemAddress(varId));
	}
	else
		as[varId] = AddressValue(AbstractState::getVirtualMemAddress(varId));
}

AddressValue AbsExe::getGepObjAddress(AbstractState& as, u32_t pointer, APOffset offset) {
	AbstractValue addrs = as[pointer];
	AddressValue ret;
	for (const auto& addr : addrs.getAddrs()) {
		s64_t baseObj = AbstractState::getInternalID(addr);
		if (baseObj == 0) {
			ret.insert(AbstractState::getVirtualMemAddress(0));
			continue;
		}
		assert(SVFUtil::isa<ObjVar>(_svfir->getGNode(baseObj)) && "Fail to get the base object address!");
		NodeID gepObj = _svfir->getGepObjVar(baseObj, offset);
		initSVFVar(as, gepObj);
		ret.insert(AbstractState::getVirtualMemAddress(gepObj));
	}
	return ret;
}

IntervalValue AbsExe::getByteOffset(const AbstractState& as, const GepStmt* gep) {
	if (gep->isConstantOffset())
		return IntervalValue((s64_t)gep->accumulateConstantByteOffset());
	IntervalValue res(0); // Initialize the result interval 'res' to 0.
	// Loop through the offsetVarAndGepTypePairVec in reverse order.
	for (int i = gep->getOffsetVarAndGepTypePairVec().size() - 1; i >= 0; i--) {
		const SVFVar* idxOperandVar = gep->getOffsetVarAndGepTypePairVec()[i].first;
		const SVFType* idxOperandType = gep->getOffsetVarAndGepTypePairVec()[i].second;
		// calculating Array/Ptr by elemByteSize * indexOperand
		if (SVFUtil::isa<SVFArrayType>(idxOperandType) || SVFUtil::isa<SVFPointerType>(idxOperandType)) {
			u32_t elemByteSize = 1;
			if (const SVFArrayType* arrOperandType = SVFUtil::dyn_cast<SVFArrayType>(idxOperandType))
				elemByteSize = arrOperandType->getTypeOfElement()->getByteSize();
			else if (SVFUtil::isa<SVFPointerType>(idxOperandType))
				elemByteSize = gep->getAccessPath().gepSrcPointeeType()->getByteSize();
			else
				assert(false && "idxOperandType must be ArrType or PtrType");
			if (const SVFConstantInt* op = SVFUtil::dyn_cast<SVFConstantInt>(idxOperandVar->getValue())) {
				s64_t lb = (double)Options::MaxFieldLimit() / elemByteSize >= op->getSExtValue()
				               ? op->getSExtValue() * elemByteSize
				               : Options::MaxFieldLimit();
				res = res + IntervalValue(lb, lb);
			}
			else {
				u32_t idx = _svfir->getValueNode(idxOperandVar->getValue());
				IntervalValue idxVal = as[idx].getInterval();
				if (idxVal.isBottom())
					res = res + IntervalValue(0, 0);
				else {
					// if lb or ub is negative number, set 0.
					// if lb or ub is positive number, guarantee lb/ub * elemByteSize <= MaxFieldLimit
					s64_t ub = (idxVal.ub().getIntNumeral() < 0) ? 0
					           : (double)Options::MaxFieldLimit() / elemByteSize >= idxVal.ub().getIntNumeral()
					               ? elemByteSize * idxVal.ub().getIntNumeral()
					               : Options::MaxFieldLimit();
					s64_t lb = (idxVal.lb().getIntNumeral() < 0) ? 0
					           : ((double)Options::MaxFieldLimit() / elemByteSize >= idxVal.lb().getIntNumeral())
					               ? elemByteSize * idxVal.lb().getIntNumeral()
					               : Options::MaxFieldLimit();
					res = res + IntervalValue(lb, ub);
				}
			}
		}
		// Process struct subtype by calculating the byte offset from beginning to the field of struct
		else if (const SVFStructType* structOperandType = SVFUtil::dyn_cast<SVFStructType>(idxOperandType)) {
			res = res + IntervalValue(gep->getAccessPath().getStructFieldOffset(idxOperandVar, structOperandType));
		}
		else {
			assert(false && "gep type pair only support arr/ptr/struct");
		}
	}
	return res; // Return the resulting byte offset as an IntervalValue.
}

IntervalValue AbsExe::getElementIndex(const AbstractState& as, const GepStmt* gep) {
	if (gep->isConstantOffset())
		return IntervalValue((s64_t)gep->accumulateConstantOffset());
	IntervalValue res(0);
	for (int i = gep->getOffsetVarAndGepTypePairVec().size() - 1; i >= 0; i--) {
		AccessPath::IdxOperandPair IdxVarAndType = gep->getOffsetVarAndGepTypePairVec()[i];
		const SVFValue* value = gep->getOffsetVarAndGepTypePairVec()[i].first->getValue();
		const SVFType* type = IdxVarAndType.second;
		// idxLb/Ub is the flattened offset generated by the current OffsetVarAndGepTypePair
		s64_t idxLb;
		s64_t idxUb;
		// get lb and ub of the index value
		if (const SVFConstantInt* constInt = SVFUtil::dyn_cast<SVFConstantInt>(value))
			idxLb = idxUb = constInt->getSExtValue();
		else {
			IntervalValue idxItv = as[_svfir->getValueNode(value)].getInterval();
			if (idxItv.isBottom())
				idxLb = idxUb = 0;
			else {
				idxLb = idxItv.lb().getIntNumeral();
				idxUb = idxItv.ub().getIntNumeral();
			}
		}
		// for pointer type, flattened index = elemNum * idx
		if (SVFUtil::isa<SVFPointerType>(type)) {
			u32_t elemNum = gep->getAccessPath().getElementNum(gep->getAccessPath().gepSrcPointeeType());
			idxLb = (double)Options::MaxFieldLimit() / elemNum < idxLb ? Options::MaxFieldLimit() : idxLb * elemNum;
			idxUb = (double)Options::MaxFieldLimit() / elemNum < idxUb ? Options::MaxFieldLimit() : idxUb * elemNum;
		}
		// for array or struct, get flattened index from SymbolTable Info
		else {
			if (Options::ModelArrays()) {
				const std::vector<u32_t>& so = SymbolTableInfo::SymbolInfo()->getTypeInfo(type)->getFlattenedElemIdxVec();
				if (so.empty() || idxUb >= (APOffset)so.size() || idxLb < 0) {
					idxLb = idxUb = 0;
				}
				else {
					idxLb = SymbolTableInfo::SymbolInfo()->getFlattenedElemIdx(type, idxLb);
					idxUb = SymbolTableInfo::SymbolInfo()->getFlattenedElemIdx(type, idxUb);
				}
			}
			else
				idxLb = idxUb = 0;
		}
		res = res + IntervalValue(idxLb, idxUb);
	}
	res.meet_with(IntervalValue((s64_t)0, (s64_t)Options::MaxFieldLimit()));
	if (res.isBottom()) {
		res = IntervalValue(0);
	}
	return res;
}

void AbsExe::addBugToReporter(const Assign3Exception& e, const ICFGNode* node) {
	const SVFInstruction* inst = nullptr;
	if (const CallICFGNode* call = SVFUtil::dyn_cast<CallICFGNode>(node)) {
		inst = call->getCallSite();
	}
	else {
		inst = node->getSVFStmts().back()->getInst();
	}
	GenericBug::EventStack eventStack;
	SVFBugEvent sourceInstEvent(SVFBugEvent::EventType::SourceInst, inst);
	for (const auto& callsite : _callSiteStack) {
		SVFBugEvent callSiteEvent(SVFBugEvent::EventType::CallSite, callsite->getCallSite());
		eventStack.push_back(callSiteEvent);
	}
	eventStack.push_back(sourceInstEvent);
	if (eventStack.size() == 0)
		return;
	std::string loc = eventStack.back().getEventLoc();
	if (_bugLoc.find(loc) != _bugLoc.end()) {
		return;
	}
	else {
		_bugLoc.insert(loc);
	}
	_recoder.addAbsExecBug(GenericBug::FULLBUFOVERFLOW, eventStack, 0, 0, 0, 0);
	_nodeToBugInfo[node] = e.what();
}
