//===- AEState.cpp -- Assignment-3 abstract-state facade ------------------//

#include "AEState.h"
#include "SVFIR/SVFIR.h"
#include "Util/Options.h"

#include <cassert>
#include <cstdlib>

namespace SVF {

	AbstractValue& AEState::load(u32_t addr) {
		return state.load(addr);
	}

	const AbstractValue& AEState::load(u32_t addr) const {
		return state.load(addr);
	}

	void AEState::store(u32_t addr, const AbstractValue& value) {
		state.store(addr, value);
	}

	const AbstractValue& AEState::getAbsValue(const ValVar* var) const {
		u32_t id = var->getId();
		AbstractState& mutableState = const_cast<AbstractState&>(state);
		if (!mutableState.getVarToVal().count(id))
			mutableState[id] = IntervalValue::top();
		return mutableState[id];
	}

	const AbstractValue& AEState::getAbsValue(const ObjVar* var) const {
		AbstractState& mutableState = const_cast<AbstractState&>(state);
		return mutableState.load(AbstractState::getVirtualMemAddress(var->getId()));
	}

	const AbstractValue& AEState::getAbsValue(const SVFVar* var) const {
		if (const ObjVar* obj = SVFUtil::dyn_cast<ObjVar>(var))
			return getAbsValue(obj);
		if (const ValVar* value = SVFUtil::dyn_cast<ValVar>(var))
			return getAbsValue(value);
		assert(false && "Unknown SVFVar kind");
		std::abort();
	}

	void AEState::updateAbsValue(const ValVar* var, const AbstractValue& value) {
		state[var->getId()] = value;
	}

	void AEState::updateAbsValue(const ObjVar* var, const AbstractValue& value) {
		state.store(AbstractState::getVirtualMemAddress(var->getId()), value);
	}

	void AEState::updateAbsValue(const SVFVar* var, const AbstractValue& value) {
		if (const ObjVar* obj = SVFUtil::dyn_cast<ObjVar>(var))
			updateAbsValue(obj, value);
		else if (const ValVar* val = SVFUtil::dyn_cast<ValVar>(var))
			updateAbsValue(val, value);
		else
			assert(false && "Unknown SVFVar kind");
	}

	const ValVar* AEState::getValVar(NodeID id) const {
		const SVFVar* var = PAG::getPAG()->getGNode(id);
		assert(SVFUtil::isa<ValVar>(var) && "AEState operation requires a ValVar");
		return SVFUtil::cast<ValVar>(var);
	}

	AbstractValue AEState::loadValue(const ValVar* pointer) const {
		AbstractValue result;
		const AbstractValue& ptr = getAbsValue(pointer);
		for (u32_t addr : ptr.getAddrs()) {
			const SVFVar* object = PAG::getPAG()->getSVFVar(state.getIDFromAddr(addr));
			result.join_with(getAbsValue(object));
		}
		return result;
	}

	AbstractValue AEState::loadValue(NodeID pointerId) const {
		return loadValue(getValVar(pointerId));
	}

	void AEState::storeValue(const ValVar* pointer, const AbstractValue& value) {
		const AbstractValue& ptr = getAbsValue(pointer);
		for (u32_t addr : ptr.getAddrs()) {
			const SVFVar* object = PAG::getPAG()->getSVFVar(state.getIDFromAddr(addr));
			updateAbsValue(object, value);
		}
	}

	void AEState::storeValue(NodeID pointerId, const AbstractValue& value) {
		storeValue(getValVar(pointerId), value);
	}

	AddressValue AEState::getGepObjAddrs(const ValVar* pointer, IntervalValue offset) {
		AddressValue addresses;
		APOffset lb = offset.lb().getIntNumeral() < Options::MaxFieldLimit()
		              ? offset.lb().getIntNumeral() : Options::MaxFieldLimit();
		APOffset ub = offset.ub().getIntNumeral() < Options::MaxFieldLimit()
		              ? offset.ub().getIntNumeral() : Options::MaxFieldLimit();
		for (APOffset index = lb; index <= ub; ++index) {
			for (u32_t address : getAbsValue(pointer).getAddrs()) {
				NodeID base = state.getIDFromAddr(address);
				assert(SVFUtil::isa<ObjVar>(PAG::getPAG()->getSVFVar(base)) &&
				       "Fail to get the base object address");
				NodeID gepObj = PAG::getPAG()->getGepObjVar(base, index);
				state[gepObj] = AddressValue(AbstractState::getVirtualMemAddress(gepObj));
				addresses.insert(AbstractState::getVirtualMemAddress(gepObj));
			}
		}
		return addresses;
	}

	AddressValue AEState::getGepObjAddrs(NodeID pointerId, IntervalValue offset) {
		return getGepObjAddrs(getValVar(pointerId), offset);
	}

	const SVFType* AEState::getPointeeElement(const ObjVar* object) const {
		if (!object) return nullptr;
		const AbstractValue& ptr = getAbsValue(object);
		if (!ptr.isAddr()) return nullptr;
		for (u32_t address : ptr.getAddrs()) {
			NodeID objectId = state.getIDFromAddr(address);
			if (objectId != 0)
				return PAG::getPAG()->getBaseObject(objectId)->getType();
		}
		return nullptr;
	}

	IntervalValue AEState::getGepElementIndex(const GepStmt* gep) const {
		if (gep->isConstantOffset())
			return IntervalValue((s64_t)gep->accumulateConstantOffset());
		IntervalValue result(0);
		for (int i = (int)gep->getOffsetVarAndGepTypePairVec().size() - 1; i >= 0; --i) {
			const ValVar* var = gep->getOffsetVarAndGepTypePairVec()[i].first;
			const SVFType* type = gep->getOffsetVarAndGepTypePairVec()[i].second;
			s64_t lb, ub;
			if (const ConstIntValVar* constant = SVFUtil::dyn_cast<ConstIntValVar>(var))
				lb = ub = constant->getSExtValue();
			else {
				IntervalValue interval = getAbsValue(var).getInterval();
				if (interval.isBottom()) lb = ub = 0;
				else { lb = interval.lb().getIntNumeral(); ub = interval.ub().getIntNumeral(); }
			}
			if (SVFUtil::isa<SVFPointerType>(type)) {
				u32_t count = gep->getAccessPath().getElementNum(gep->getAccessPath().gepSrcPointeeType());
				lb = (double)Options::MaxFieldLimit() / count < lb ? Options::MaxFieldLimit() : lb * count;
				ub = (double)Options::MaxFieldLimit() / count < ub ? Options::MaxFieldLimit() : ub * count;
			} else if (Options::ModelArrays()) {
				const std::vector<u32_t>& flat = PAG::getPAG()->getTypeInfo(type)->getFlattenedElemIdxVec();
				if (flat.empty() || ub >= (APOffset)flat.size() || lb < 0) lb = ub = 0;
				else { lb = PAG::getPAG()->getFlattenedElemIdx(type, lb); ub = PAG::getPAG()->getFlattenedElemIdx(type, ub); }
			} else lb = ub = 0;
			result = result + IntervalValue(lb, ub);
		}
		result.meet_with(IntervalValue((s64_t)0, (s64_t)Options::MaxFieldLimit()));
		return result.isBottom() ? IntervalValue(0) : result;
	}

	IntervalValue AEState::getGepByteOffset(const GepStmt* gep) const {
		if (gep->isConstantOffset())
			return IntervalValue((s64_t)gep->accumulateConstantByteOffset());
		IntervalValue result(0);
		for (int i = (int)gep->getOffsetVarAndGepTypePairVec().size() - 1; i >= 0; --i) {
			const ValVar* var = gep->getOffsetVarAndGepTypePairVec()[i].first;
			const SVFType* type = gep->getOffsetVarAndGepTypePairVec()[i].second;
			if (SVFUtil::isa<SVFArrayType>(type) || SVFUtil::isa<SVFPointerType>(type)) {
				u32_t size = SVFUtil::isa<SVFArrayType>(type)
				             ? SVFUtil::cast<SVFArrayType>(type)->getTypeOfElement()->getByteSize()
				             : gep->getAccessPath().gepSrcPointeeType()->getByteSize();
				if (const ConstIntValVar* constant = SVFUtil::dyn_cast<ConstIntValVar>(var)) {
					s64_t value = (double)Options::MaxFieldLimit() / size >= constant->getSExtValue()
					              ? constant->getSExtValue() * size : Options::MaxFieldLimit();
					result = result + IntervalValue(value, value);
				} else {
					IntervalValue index = getAbsValue(var).getInterval();
					if (index.isBottom()) result = result + IntervalValue(0, 0);
					else {
						auto scale = [size](s64_t value) {
							if (value < 0) return (s64_t)0;
							return (double)Options::MaxFieldLimit() / size >= value
							       ? (s64_t)size * value : (s64_t)Options::MaxFieldLimit();
						};
						result = result + IntervalValue(scale(index.lb().getIntNumeral()), scale(index.ub().getIntNumeral()));
					}
				}
			} else if (const SVFStructType* structure = SVFUtil::dyn_cast<SVFStructType>(type)) {
				result = result + IntervalValue(gep->getAccessPath().getStructFieldOffset(var, structure));
			} else assert(false && "gep type pair only supports array/pointer/struct");
		}
		return result;
	}

	u32_t AEState::getAllocaInstByteSize(const AddrStmt* addr) const {
		const ObjVar* object = SVFUtil::dyn_cast<ObjVar>(addr->getRHSVar());
		assert(object && "Addr rhs value is not ObjVar");
		const BaseObjVar* base = PAG::getPAG()->getBaseObject(object->getId());
		if (base->isConstantByteSize()) return base->getByteSizeOfObj();
		u64_t result = 1;
		for (const SVFVar* size : addr->getArrSize()) {
			IntervalValue interval = getAbsValue(size).getInterval();
			if (interval.isBottom()) interval = IntervalValue(Options::MaxFieldLimit());
			result = result * interval.ub().getIntNumeral() > Options::MaxFieldLimit()
			         ? Options::MaxFieldLimit() : result * interval.ub().getIntNumeral();
		}
		return (u32_t)result;
	}

	void AEState::joinWith(const AbstractState& other) {
		state.joinWith(other);
	}

	AbstractState AEState::widening(const AbstractState& other) const {
		return const_cast<AbstractState&>(raw()).widening(other);
	}

	AbstractState AEState::narrowing(const AbstractState& other) const {
		return const_cast<AbstractState&>(raw()).narrowing(other);
	}

	bool AEState::equals(const AbstractState& other) const {
		return raw().equals(other);
	}

	void AEState::printAbstractState() const {
		raw().printAbstractState();
	}

} // namespace SVF
