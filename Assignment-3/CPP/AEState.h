//===- AEState.h -- Assignment-3 abstract-state facade --------------------//

#ifndef ASSIGNMENT_3_AE_STATE_H
#define ASSIGNMENT_3_AE_STATE_H

#include "AE/Core/AbstractState.h"

namespace SVF {

	class AddrStmt;
	class GepStmt;
	class ObjVar;
	class SVFVar;
	class SVFType;
	class ValVar;

	/// Assignment 3's own state value.  The abstract domain is reused from SVF,
	/// but the state and its trace are owned by Assignment 3.
	class AEState final {
	 public:
		AEState() = default;
		explicit AEState(const AbstractState& state) : state(state) {}

		AbstractState& raw() { return state; }
		const AbstractState& raw() const { return state; }
		operator AbstractState&() { return state; }
		operator const AbstractState&() const { return state; }
		AEState& operator=(const AbstractState& value) {
			state = value;
			return *this;
		}
		AbstractState* operator->() { return &state; }
		const AbstractState* operator->() const { return &state; }

		AbstractValue& operator[](NodeID varId) { return state[varId]; }
		const AbstractValue& operator[](NodeID varId) const { return state[varId]; }

		AbstractValue& load(u32_t addr);
		const AbstractValue& load(u32_t addr) const;
		void store(u32_t addr, const AbstractValue& value);

		u32_t getIDFromAddr(u32_t addr) const {
			return raw().getIDFromAddr(addr);
		}
		bool inVarToAddrsTable(u32_t id) const {
			return raw().inVarToAddrsTable(id);
		}
		bool inVarToValTable(u32_t id) const {
			return raw().inVarToValTable(id);
		}
		bool inAddrToAddrsTable(u32_t id) const {
			return raw().inAddrToAddrsTable(id);
		}
		bool inAddrToValTable(u32_t id) const {
			return raw().inAddrToValTable(id);
		}
		bool isFreedMem(u32_t addr) const {
			return raw().isFreedMem(addr);
		}
		const AbstractState::AddrToAbsValMap& getLocToVal() const {
			return raw().getLocToVal();
		}

		void initObjVar(const ObjVar* objVar) {
			raw().initObjVar(objVar);
		}

		const AbstractValue& getAbsValue(const ValVar* var) const;
		const AbstractValue& getAbsValue(const ObjVar* var) const;
		const AbstractValue& getAbsValue(const SVFVar* var) const;

		void updateAbsValue(const ValVar* var, const AbstractValue& value);
		void updateAbsValue(const ObjVar* var, const AbstractValue& value);
		void updateAbsValue(const SVFVar* var, const AbstractValue& value);

		AbstractValue loadValue(const ValVar* pointer) const;
		AbstractValue loadValue(NodeID pointerId) const;
		void storeValue(const ValVar* pointer, const AbstractValue& value);
		void storeValue(NodeID pointerId, const AbstractValue& value);

		AddressValue getGepObjAddrs(const ValVar* pointer, IntervalValue offset);
		AddressValue getGepObjAddrs(NodeID pointerId, IntervalValue offset);
		const SVFType* getPointeeElement(const ObjVar* object) const;
		IntervalValue getGepElementIndex(const GepStmt* gep) const;
		IntervalValue getElementIndex(const GepStmt* gep) const {
			return getGepElementIndex(gep);
		}
		IntervalValue getGepByteOffset(const GepStmt* gep) const;
		IntervalValue getByteOffset(const GepStmt* gep) const {
			return getGepByteOffset(gep);
		}
		u32_t getAllocaInstByteSize(const AddrStmt* addr) const;

		void joinWith(const AbstractState& other);
		AbstractState widening(const AbstractState& other) const;
		AbstractState narrowing(const AbstractState& other) const;
		bool equals(const AbstractState& other) const;
		bool operator==(const AbstractState& other) const {
			return equals(other);
		}
		void printAbstractState() const;

	 private:
		const ValVar* getValVar(NodeID id) const;

		AbstractState state;
	};

} // namespace SVF

#endif // ASSIGNMENT_3_AE_STATE_H
