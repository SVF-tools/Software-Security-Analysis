"""Bug-reporter implementations for the Assignment-3 harness.

Owns the AEReporter class — bug accumulation, assertion-point tracking,
GEP / strlen / memcpy helpers shared by the checkers.  Everything else
(AbstractExecution driver, ICFG/WTO traversal, call-site dispatch) lives
in AEHelper.py.
"""

import pysvf
from pysvf import IntervalValue, AddressValue, AbstractValue, AbstractState
from AEState import AEState, unwrap_state


class AEReporter:
    """
    Bug reporter and shared helper utilities for the Assignment-3
    abstract-interpretation harness.
    """

    def __init__(self, svfir: pysvf.SVFIR):
        # Map (kind, ICFGNode) -> diagnostic message. A single access may
        # legitimately have reports of different kinds.
        self.node_to_bug_info = {}
        self.svfir = svfir
        # Harness bookkeeping: stub call sites the analysis actually reached.
        self.assert_points = set()
        self.expected_report_counts = {}

    def noteAssertionPoint(self, call):
        self.assert_points.add(call)

    def isAssertionPoint(self, call) -> bool:
        return call in self.assert_points

    def noteExpectedReport(self, kind: str):
        self.expected_report_counts[kind] = (
            self.expected_report_counts.get(kind, 0) + 1)

    def getExpectedReportCount(self, kind: str) -> int:
        return self.expected_report_counts.get(kind, 0)

    def getByteOffset(self, abstract_state: pysvf.AbstractState, gep: pysvf.GepStmt) -> pysvf.IntervalValue:
        return AEState(unwrap_state(abstract_state)).getGepByteOffset(gep)

    def getGepObjAddrs(self, abstract_state: pysvf.AbstractState, var_id: int, offset: pysvf.IntervalValue) -> pysvf.AddressValue:
        pointer = self.svfir.getGNode(var_id)
        return AEState(unwrap_state(abstract_state)).getGepObjAddrs(pointer, offset)

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
        return AEState(unwrap_state(abstract_state)).getAllocaInstByteSize(addr)

    def reportBug(self, kind: str, node, msg):
        self.node_to_bug_info.setdefault((kind, node), msg)

    def reportBufOverflow(self, node, msg):
        self.reportBug("buffer-overflow", node, msg)

    def reportNullDeref(self, node, msg):
        self.reportBug("nullptr-deref", node, msg)

    def getReportCount(self, kind=None) -> int:
        if kind is None:
            return len(self.node_to_bug_info)
        return sum(1 for report_kind, _ in self.node_to_bug_info
                   if report_kind == kind)

    def printReport(self):
        if not self.node_to_bug_info:
            return
        print("###################### Bug Reports ({} total) ######################".format(
            len(self.node_to_bug_info)))
        print("---------------------------------------------")
        for (kind, node), msg in self.node_to_bug_info.items():
            print(f"[{kind}] {node}: {msg}\n"
                  "---------------------------------------------")

    def getElementSize(self, var: pysvf.SVFVar) -> int:
        if var.getType().isArrayTy():
            return var.getType().getTypeOfElement().getByteSize()
        return 1

    def hasLocValue(self, abstractState: pysvf.AbstractState, objId: int) -> bool:
        if hasattr(abstractState, "inAddrToValTable"):
            return (abstractState.inAddrToValTable(objId) or
                    abstractState.inAddrToAddrsTable(objId))
        if abstractState.inAddrToAddrsTable(objId):
            return True
        locToVal = abstractState.getLocToVal()
        val = locToVal.get(objId)
        return val is not None and (val.isInterval() or val.isAddr())

    def handleMemcpy(self, abstractState: pysvf.AbstractState, dst: pysvf.SVFVar, src: pysvf.SVFVar, len: pysvf.IntervalValue, start_idx: int):
        abstractState = unwrap_state(abstractState)
        assert isinstance(abstractState, pysvf.AbstractState), "abstractState is not a pysvf.AbstractState"
        assert isinstance(dst, pysvf.SVFVar), "dst is not a pysvf.SVFVar"
        assert isinstance(src, pysvf.SVFVar), "src is not a pysvf.SVFVar"
        assert isinstance(len, pysvf.IntervalValue), "len is not a pysvf.IntervalValue"
        assert isinstance(start_idx, int), "start_idx is not an integer"
        dstId = dst.getId()
        srcId = src.getId()
        elemSize = self.getElementSize(dst)
        if len.isBottom() or len.lb().is_minus_infinity:
            return
        size = len.lb().getNumeral()
        range_val = size / elemSize
        if abstractState.inVarToAddrsTable(dstId) and abstractState.inVarToAddrsTable(srcId):
            for index in range(0, int(range_val)):
                expr_src = self.getGepObjAddrs(abstractState, srcId, pysvf.IntervalValue(index))
                expr_dst = self.getGepObjAddrs(abstractState, dstId, pysvf.IntervalValue(index + start_idx))
                for addr_src in expr_src:
                    if (pysvf.AbstractState.isNullMem(addr_src) or
                            pysvf.AbstractState.isBlackHoleObjAddr(addr_src)):
                        continue
                    for addr_dst in expr_dst:
                        if (pysvf.AbstractState.isNullMem(addr_dst) or
                                pysvf.AbstractState.isBlackHoleObjAddr(addr_dst)):
                            continue
                        objId = abstractState.getIDFromAddr(addr_src)
                        if self.hasLocValue(abstractState, objId):
                            lhs = abstractState.load(addr_src)
                            abstractState.store(addr_dst, lhs)

    def getStrlen(self, abstractState, strValue):
        value_id = strValue.getId()
        dst_size = 0
        for addr in abstractState[value_id].getAddrs():
            if (pysvf.AbstractState.isNullMem(addr) or
                    pysvf.AbstractState.isBlackHoleObjAddr(addr)):
                continue
            obj_id = abstractState.getIDFromAddr(addr)
            base_object = self.svfir.getBaseObject(obj_id)
            if base_object is None or base_object.isBlackHoleObj():
                continue
            if base_object.isConstantByteSize():
                dst_size = base_object.getByteSizeOfObj()
            else:
                icfg_node = base_object.getICFGNode()
                if icfg_node is None:
                    continue
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
                    if (pysvf.AbstractState.isNullMem(addr) or
                            pysvf.AbstractState.isBlackHoleObjAddr(addr)):
                        continue
                    val.join_with(abstractState.load(addr))
                if val.isInterval() and chr(val.getInterval().getIntNumeral()) == '\0':
                    break
                length += 1
            elem_size = self.getElementSize(strValue)
        if length == 0:
            return pysvf.IntervalValue(0, pysvf.Options.max_field_limit())
        else:
            return pysvf.IntervalValue(length * elem_size)
