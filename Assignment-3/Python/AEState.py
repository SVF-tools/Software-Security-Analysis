"""Assignment-3 abstract-state facade."""

from typing import Any

import pysvf


def unwrap_state(state):
    """Return the PySVF state behind an Assignment-3 AEState."""
    return state.raw if isinstance(state, AEState) else state


class AEState:
    """Assignment-3-owned wrapper around one PySVF AbstractState."""

    isBlackHoleObjAddr = staticmethod(pysvf.AbstractState.isBlackHoleObjAddr)
    isNullMem = staticmethod(pysvf.AbstractState.isNullMem)
    isVirtualMemAddress = staticmethod(pysvf.AbstractState.isVirtualMemAddress)
    getVirtualMemAddress = staticmethod(pysvf.AbstractState.getVirtualMemAddress)

    def __init__(self, state=None) -> None:
        self._state = state if state is not None else pysvf.AbstractState()

    @property
    def raw(self) -> pysvf.AbstractState:
        return self._state

    def __getitem__(self, var_id: int) -> pysvf.AbstractValue:
        return self.raw[var_id]

    def __setitem__(self, var_id: int, value) -> None:
        self.raw[var_id] = value

    def __getattr__(self, name: str) -> Any:
        return getattr(self.raw, name)

    def __eq__(self, other) -> bool:
        return self.raw == unwrap_state(other)

    def __str__(self) -> str:
        return str(self.raw)

    def __repr__(self) -> str:
        return f"AEState(state={self.raw!r})"

    @staticmethod
    def _resolve_var(var):
        return pysvf.getPAG().getGNode(var) if isinstance(var, int) else var

    def getAbsValue(self, var: pysvf.SVFVar) -> pysvf.AbstractValue:
        if isinstance(var, pysvf.ObjVar):
            addr = pysvf.AbstractState.getVirtualMemAddress(var.getId())
            return self.raw.load(addr)
        values = self.raw.getVarToVal()
        if var.getId() not in values:
            self.raw[var.getId()] = pysvf.AbstractValue(pysvf.IntervalValue.top())
        return self.raw[var.getId()]

    def updateAbsValue(self, var: pysvf.SVFVar, value) -> None:
        if isinstance(var, pysvf.ObjVar):
            addr = pysvf.AbstractState.getVirtualMemAddress(var.getId())
            self.raw.store(addr, value)
        else:
            self.raw[var.getId()] = value

    def loadValue(self, pointer) -> pysvf.AbstractValue:
        pointer = self._resolve_var(pointer)
        result = pysvf.AbstractValue()
        value = self.getAbsValue(pointer)
        for address in value.getAddrs():
            obj = pysvf.getPAG().getGNode(self.raw.getIDFromAddr(address))
            result.join_with(self.getAbsValue(obj))
        return result

    def storeValue(self, pointer, value: pysvf.AbstractValue) -> None:
        pointer = self._resolve_var(pointer)
        value_at_pointer = self.getAbsValue(pointer)
        for address in value_at_pointer.getAddrs():
            obj = pysvf.getPAG().getGNode(self.raw.getIDFromAddr(address))
            self.updateAbsValue(obj, value)

    def getGepObjAddrs(
        self,
        pointer,
        offset: pysvf.IntervalValue,
    ) -> pysvf.AddressValue:
        pointer = self._resolve_var(pointer)
        pag = pysvf.getPAG()
        limit = pysvf.Options.max_field_limit()
        lower = min(offset.lb().getNumeral(), limit)
        upper = min(offset.ub().getNumeral(), limit)
        addresses = pysvf.AddressValue(set())
        for index in range(lower, upper + 1):
            for address in self.getAbsValue(pointer).getAddrs():
                base_id = self.raw.getIDFromAddr(address)
                base = pag.getGNode(base_id)
                assert isinstance(base, pysvf.ObjVar), "Fail to get the base object address"
                gep_obj = pag.getGepObjVar(base_id, index)
                gep_addr = pysvf.AbstractState.getVirtualMemAddress(gep_obj)
                self.raw[gep_obj] = pysvf.AbstractValue(pysvf.AddressValue(gep_addr))
                addresses.insert(gep_addr)
        return addresses

    def getGepElementIndex(self, gep: pysvf.GepStmt) -> pysvf.IntervalValue:
        if gep.isConstantOffset():
            return pysvf.IntervalValue(gep.getConstantOffset())
        result = pysvf.IntervalValue(0)
        limit = pysvf.Options.max_field_limit()
        pag = pysvf.getPAG()
        for var, value_type in reversed(gep.getOffsetVarAndGepTypePairVec()):
            if var.isConstIntValVar():
                lower = upper = var.asConstIntValVar().getSExtValue()
            else:
                interval = self.getAbsValue(var).getInterval()
                if interval.isBottom():
                    lower = upper = 0
                else:
                    lower = interval.lb().getNumeral()
                    upper = interval.ub().getNumeral()
            if value_type.isPointerType():
                count = pag.getNumOfFlattenElements(gep.getSrcPointeeType())
                lower = max(0, min(limit, lower * count))
                upper = max(0, min(limit, upper * count))
            else:
                count = pag.getNumOfFlattenElements(value_type)
                if count == 0 or upper >= count or lower < 0:
                    lower = upper = 0
                else:
                    lower = pag.getFlattenedElemIdx(value_type, lower)
                    upper = pag.getFlattenedElemIdx(value_type, upper)
            result = result + pysvf.IntervalValue(lower, upper)
        result.meet_with(pysvf.IntervalValue(0, limit))
        return pysvf.IntervalValue(0) if result.isBottom() else result

    def getElementIndex(self, gep: pysvf.GepStmt) -> pysvf.IntervalValue:
        return self.getGepElementIndex(gep)

    def getGepByteOffset(self, gep: pysvf.GepStmt) -> pysvf.IntervalValue:
        if gep.isConstantOffset():
            return pysvf.IntervalValue(gep.getConstantByteOffset())
        result = pysvf.IntervalValue(0)
        limit = pysvf.Options.max_field_limit()
        for var, value_type in reversed(gep.getOffsetVarAndGepTypePairVec()):
            if value_type.isArrayType() or value_type.isPointerType():
                size = (value_type.asArrayType().getTypeOfElement().getByteSize()
                        if value_type.isArrayType()
                        else gep.getSrcPointeeType().getByteSize())
                if var.isConstIntValVar():
                    value = var.asConstIntValVar().getSExtValue()
                    scaled = value * size if limit / size >= value else limit
                    result = result + pysvf.IntervalValue(scaled)
                else:
                    index = self.getAbsValue(var).getInterval()
                    if index.isBottom():
                        result = result + pysvf.IntervalValue(0)
                    else:
                        def scale(value):
                            if value < 0:
                                return 0
                            return value * size if limit / size >= value else limit
                        result = result + pysvf.IntervalValue(
                            scale(index.lb().getNumeral()),
                            scale(index.ub().getNumeral()))
            elif value_type.isStructType():
                result = result + pysvf.IntervalValue(
                    gep.getStructFieldOffset(var, value_type.asStructType()))
            else:
                raise AssertionError("GEP type must be array, pointer, or struct")
        return result

    def getByteOffset(self, gep: pysvf.GepStmt) -> pysvf.IntervalValue:
        return self.getGepByteOffset(gep)

    def getAllocaInstByteSize(self, addr: pysvf.AddrStmt) -> int:
        obj = addr.getRHSVar()
        assert isinstance(obj, pysvf.ObjVar), "Addr rhs value is not ObjVar"
        base = pysvf.getPAG().getBaseObject(obj.getId())
        if base.isConstantByteSize():
            return base.getByteSizeOfObj()
        result = 1
        limit = pysvf.Options.max_field_limit()
        for size_var in addr.getArrSize():
            interval = self.getAbsValue(size_var).getInterval()
            if interval.isBottom():
                interval = pysvf.IntervalValue(limit)
            size = interval.ub().getNumeral()
            result = limit if result * size > limit else result * size
        return result

    def getPointeeElement(self, pointer_id: int):
        for address in self.raw[pointer_id].getAddrs():
            obj_id = self.raw.getIDFromAddr(address)
            pag = pysvf.getPAG()
            obj = pag.getGNode(obj_id)
            base_object = pag.getBaseObject(obj_id)
            if base_object is not None:
                return base_object.getType()
        return None

    def joinWith(self, other) -> None:
        self.raw.joinWith(unwrap_state(other))

    def widening(self, other) -> pysvf.AbstractState:
        return self.raw.widening(unwrap_state(other))

    def narrowing(self, other) -> pysvf.AbstractState:
        return self.raw.narrowing(unwrap_state(other))

    def equals(self, other) -> bool:
        return self.raw.equals(unwrap_state(other))
