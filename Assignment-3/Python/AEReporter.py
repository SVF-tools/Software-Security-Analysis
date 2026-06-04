"""Bug-reporter implementations for the Assignment-3 harness.

Owns the AEReporter class — bug accumulation, assertion-point tracking,
GEP / strlen / memcpy helpers shared by the checkers.  Everything else
(AbstractExecution driver, ICFG/WTO traversal, call-site dispatch) lives
in AEHelper.py.
"""

import pysvf
from pysvf import IntervalValue, AddressValue, AbstractValue, AbstractState


class AEReporter:
    """
    Bug reporter and shared helper utilities for the Assignment-3
    abstract-interpretation harness.
    """

    def __init__(self, svfir: pysvf.SVFIR, ai: 'pysvf.AbstractInterpretation' = None):
        # Map ICFGNode -> diagnostic message for each detected bug.
        self.node_to_bug_info = {}
        self.svfir = svfir
        self.ai = ai
        # Harness bookkeeping: stub call sites the analysis actually reached.
        self.assert_points = set()

    def noteAssertionPoint(self, call):
        self.assert_points.add(call)

    def isAssertionPoint(self, call) -> bool:
        return call in self.assert_points

    def getByteOffset(self, abstract_state: pysvf.AbstractState, gep: pysvf.GepStmt) -> pysvf.IntervalValue:
        return self.ai.getGepByteOffset(gep)

    def getGepObjAddrs(self, abstract_state: pysvf.AbstractState, var_id: int, offset: pysvf.IntervalValue) -> pysvf.AddressValue:
        pointer = self.svfir.getGNode(var_id)
        return self.ai.getGepObjAddrs(pointer, offset)

    def getPointeeElement(self, abstract_state: pysvf.AbstractState, var_id: int):
        ptr_val = abstract_state[var_id]
        if not ptr_val.isAddr():
            return None
        for addr in ptr_val.getAddrs():
            obj_id = abstract_state.getIDFromAddr(addr)
            if obj_id == 0:
                continue
            return self.svfir.getBaseObject(obj_id).getType()
        return None

    def getAllocaInstByteSize(self, abstract_state: pysvf.AbstractState, addr: pysvf.AddrStmt) -> int:
        return self.ai.getAllocaInstByteSize(addr)

    def reportBufOverflow(self, node, msg):
        self.node_to_bug_info[node] = msg

    def printReport(self):
        if not self.node_to_bug_info:
            return
        print("###################### Bug Reports ({} total) ######################".format(
            len(self.node_to_bug_info)))
        print("---------------------------------------------")
        for node, msg in self.node_to_bug_info.items():
            print(f"{node}: {msg}\n---------------------------------------------")

    def handleMemcpy(self, abstractState: pysvf.AbstractState, dst: pysvf.SVFVar, src: pysvf.SVFVar, len: pysvf.IntervalValue, start_idx: int):
        assert isinstance(abstractState, pysvf.AbstractState), "abstractState is not a pysvf.AbstractState"
        assert isinstance(dst, pysvf.SVFVar), "dst is not a pysvf.SVFVar"
        assert isinstance(src, pysvf.SVFVar), "src is not a pysvf.SVFVar"
        assert isinstance(len, pysvf.IntervalValue), "len is not a pysvf.IntervalValue"
        assert isinstance(start_idx, int), "start_idx is not an integer"
        dstId = dst.getId()
        srcId = src.getId()
        elemSize = 1
        if isinstance(dst, pysvf.ValVar):
            if dst.getType().isArrayTy():
                elemSize = dst.getType().getTypeOfElement().getByteSize()
            elif dst.getType().isPointerTy():
                elemType = self.getPointeeElement(abstractState, dstId)
                if elemType.isArrayTy():
                    elemSize = elemType.getTypeOfElement().getByteSize()
                else:
                    elemSize = elemType.getByteSize()
            else:
                raise AssertionError("Unsupported type")
        size = len.lb().getNumeral()
        range_val = size / elemSize
        if abstractState.inVarToAddrsTable(dstId) and abstractState.inVarToAddrsTable(srcId):
            for index in range(0, int(range_val)):
                expr_src = self.getGepObjAddrs(abstractState, srcId, pysvf.IntervalValue(index))
                expr_dst = self.getGepObjAddrs(abstractState, dstId, pysvf.IntervalValue(index + start_idx))
                for addr_src in expr_src:
                    for addr_dst in expr_dst:
                        objId = abstractState.getIDFromAddr(addr_src)
                        if objId in abstractState.getLocToVal():
                            lhs = abstractState.load(addr_src)
                            abstractState.store(addr_dst, lhs)

    def getStrlen(self, abstractState, strValue):
        value_id = strValue.getId()
        dst_size = 0
        for addr in abstractState[value_id].getAddrs():
            obj_id = abstractState.getIDFromAddr(addr)
            base_object = self.svfir.getBaseObject(obj_id)
            if base_object.isConstantByteSize():
                dst_size = base_object.getByteSizeOfObj()
            else:
                icfg_node = base_object.getICFGNode()
                for stmt in icfg_node.getSVFStmts():
                    if isinstance(stmt, pysvf.AddrStmt):
                        dst_size = self.getAllocaInstByteSize(abstractState, stmt)
        length = 0
        elem_size = 1
        if abstractState.getVar(value_id).isAddr():
            for index in range(dst_size):
                expr0 = self.getGepObjAddrs(abstractState, value_id, pysvf.IntervalValue(index))
                val = pysvf.AbstractValue()
                for addr in expr0:
                    val.join_with(abstractState.load(addr))
                if val.isInterval() and chr(val.getInterval().getIntNumeral()) == '\0':
                    break
                length += 1
            if strValue.getType().isArrayTy():
                elem_size = strValue.getType().getTypeOfElement().getByteSize()
            elif strValue.getType().isPointerTy():
                elem_type = self.getPointeeElement(abstractState, value_id)
                if elem_type:
                    if elem_type.isArrayTy():
                        elem_size = elem_type.getTypeOfElement().getByteSize()
                    else:
                        elem_size = elem_type.getByteSize()
                else:
                    elem_size = 1
            else:
                raise AssertionError("Unsupported type")
        if length == 0:
            return pysvf.IntervalValue(0, pysvf.Options.max_field_limit())
        else:
            return pysvf.IntervalValue(length * elem_size)
