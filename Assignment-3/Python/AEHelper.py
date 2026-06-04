"""Harness for Assignment-3 abstract interpretation.

Owns the AbstractExecution class's harness-side methods — interprocedural
WTO construction (initWto), stub / checkpoint sub-dispatchers
(handleStubFunction, handleCheckpointStubs) invoked from the student's
handleCallSite override in Assignment_3.py, the external-API whitelist
(isExternalCallForAssignment), the abstract-state helpers that wrap
AbstractInterpretation, and the validator (ensureAllAssertsValidated).

The AEReporter class (pure bug reporting + JSON / coverage summary plus
the GEP / strlen / memcpy helpers used by the bug checkers) lives in
AEReporter.py.  The analysis driver (analyse / handleCallSite /
reportBufOverflow / reportNullDeref) and the six student tasks live in
Assignment_3.py.
"""

from AEReporter import AEReporter

from abc import abstractmethod

from pysvf import ICFG, ICFGNode
from typing import List, Dict, Set, Optional
import pysvf
import faulthandler
faulthandler.enable()

import pysvf
from pysvf import IntervalValue, AddressValue, AbstractValue, AbstractState
import sys
from pysvf.enums import OpCode, Predicate
class WTOCycleDepth:
    def __init__(self):
        self._heads: List[ICFGNode] = []

    def add(self, head: ICFGNode):
        self._heads.append(head)

    def __iter__(self):
        return iter(self._heads)

    def __str__(self):
        return f"{self._heads}"

    def __repr__(self):
        return f"{self._heads}"

    def compare(self, other):
        if self == other:
            return 0
        this_it = iter(self)
        other_it = iter(other)
        while this_it:
            if not other_it:
                return 1
            elif this_it == other_it:
                this_it = next(this_it)
                other_it = next(other_it)
            else:
                return 2
        if not other_it:
            return 0
        else:
            return -1

    def __lt__(self, other):
        return self.compare(other) == -1

    def __le__(self, other):
        return self.compare(other) <= 0

    def __eq__(self, other):
        return self.compare(other) == 0

    def __ge__(self, other):
        return self.compare(other) >= 0

    def __gt__(self, other):
        return self.compare(other) == 1



class ICFGWTOComp:
    def __init__(self, node: ICFGNode):
        self.node = node


    def getICFGNode(self) -> ICFGNode:
        return self.node

    @abstractmethod
    def accept(self, visitor):
        pass


class ICFGWTONode(ICFGWTOComp):
    def __init__(self, node: ICFGNode):
        self.node = node

    def accept(self, visitor):
        visitor.visitNode(self)


class ICFGWTOCycle(ICFGWTOComp):
    def __init__(self, head: ICFGWTONode, components: List[ICFGWTOComp]):
        self.head = head
        self.components = components

    def accept(self, visitor):
        visitor.visit(self)


class ICFGWTO:

    class WTOCycleDepthBuilder:
        def __init__(self, node_to_wto_cycle_depth):
            self.wto_cycle_depth = WTOCycleDepth()
            self.node_to_wto_cycle_depth = node_to_wto_cycle_depth

        def visit(self, cycle: ICFGWTOCycle):
            head = cycle.head.getICFGNode()
            previous_cycle_depth = self.wto_cycle_depth
            self.node_to_wto_cycle_depth[head] = self.wto_cycle_depth
            self.wto_cycle_depth = WTOCycleDepth()
            self.wto_cycle_depth.add(head)
            for component in cycle.components:
                component.accept(self)
            self.wto_cycle_depth = previous_cycle_depth

        def visitNode(self, node: ICFGWTONode):
            self.node_to_wto_cycle_depth[node.getICFGNode()] = self.wto_cycle_depth


    def __init__(self, graph: ICFG, entry: ICFGNode, scc=None):
        self.graph = graph
        self.entry = entry
        # Interprocedural WTO: `scc` is the set of FunObjVar *ids* in this
        # call-graph SCC.  Call edges into a callee in the same SCC are then
        # followed (becoming back-edges), so a (mutually) recursive function's
        # entry shows up as a WTO cycle head -- exactly like the C++ ICFGWTO.
        # If no SCC is given, the SCC is the entry's own function.
        if scc:
            self.scc_fun_ids = set(scc)
        else:
            self.scc_fun_ids = {entry.getFun().getId()}
        self.components: List[ICFGWTOComp] = []
        self.all_components : Set[ICFGWTOComp] = set()
        self.head_ref_to_cycle: Dict[ICFGNode, ICFGWTOCycle] = {}
        self.node_to_depth: Dict[ICFGNode, int] = {}

        self._num = 0
        self._CDN: Dict[ICFGNode, int] = {}
        self._stack: List[ICFGNode] = []

    def init(self):
        self.visit(self.entry, self.components)
        self._CDN.clear()
        self._stack.clear()
        self.build_node_to_depth()


    def component(self, node: ICFGNode) -> ICFGWTOCycle:
        partition = []
        for succ in self.get_successors(node):
            if self._CDN.get(succ, 0) == 0:
                self.visit(succ, partition)
        head = ICFGWTONode(node)
        ptr = ICFGWTOCycle(head, partition)
        self.head_ref_to_cycle[node] = ptr
        return ptr


    def visit(self, node: ICFGNode, components: List[ICFGWTOComp]):
        head = 0 # CycleDepthNumber head(0)
        min = 0 # CycleDepthNumber min(0)
        loop = False # bool loop
        self._stack.append(node)    # push(node)
        self._num += 1  # _num += CycleDepthNumber(1)
        head = self._num # head = _num
        self._CDN[node] = head # setCDN(node, head)
        for succ in self.get_successors(node): # forEachSuccessor(node, [&](const NodeT* succ)
            succ_dfn = self._CDN.get(succ, 0) # CycleDepthNumber succ_dfn = getCDN(succ)
            if succ_dfn == 0: # if (succ_dfn == CycleDepthNumber(0))
                min = self.visit(succ, components) # min = visit(succ, partition)
            else:
                min = succ_dfn # min = succ_dfn
            if min <= head: # if (min <= head)
                head = min # head = min
                loop = True # loop = true

        if head == self._CDN[node]: # if (head == getCDN(node))
            self._CDN[node] = 0x7fffffff # setCDN(node, UINT_MAX)
            element = self._stack.pop()
            if loop:
                while element != node:
                    self._CDN[element] = 0 # setCDN(element, 0)
                    element = self._stack.pop() # element = pop()
                components.insert(0, self.component(node)) # partition.push_front(component(node))
            else:
                components.insert(0, ICFGWTONode(node)) # partition.push_front(newNode(node))
        return head


    def get_successors(self, node: ICFGNode) -> List[ICFGNode]:
        # Interprocedural successor relation, mirroring C++ ICFGWTO::getSuccessors.
        successors = []
        if isinstance(node, pysvf.CallICFGNode):
            for e in node.getOutEdges():
                callee_entry = e.getDstNode()
                if callee_entry.getFun().getId() in self.scc_fun_ids:
                    # caller & callee in the same SCC -> follow the call edge
                    successors.append(callee_entry)
                else:
                    # different SCC -> shortcut to the local return node
                    successors.append(node.getRetICFGNode())
        else:
            for e in node.getOutEdges():
                succ = e.getDstNode()
                # Only stay within the SCC (intra edges, and return edges back
                # into an SCC function).
                if succ.getFun().getId() in self.scc_fun_ids:
                    successors.append(succ)
        return successors


    def build_node_to_depth(self):
        builder = self.WTOCycleDepthBuilder(self.node_to_depth)
        for component in self.components:
            component.accept(builder)


    def __str__(self):
        return f"ICFGWTO: {self.components}"


class AbstractExecution:
    def __init__(self, pag: pysvf.SVFIR):
        self.svfir = pag
        self.icfg = pag.getICFG()
        self.call_site_stack = []
        self.func_to_wto = {}
        self.pre_abs_trace = {}
        # Owns the post-trace and is the backing store for AbsExtAPI as well
        # as the GEP/load/store helpers (getGepByteOffset etc.). Replaces
        # the old `self.post_abs_trace` dict so reads/writes on
        # `self.post_abs_trace[node]` go through the mgr's trace.
        # AbstractStateManager was folded into AbstractInterpretation upstream
        # (the AbstractStateManager.h header was removed).  Use the
        # AbstractInterpretation singleton; it pulls SVFIR from PAG::getPAG()
        # internally and does not need an explicit Andersen instance.
        self.ai = pysvf.AbstractInterpretation.getAEInstance()
        # Alias preserved so existing call-sites `self.post_abs_trace[node]`
        # keep working. The mgr supports __getitem__/__setitem__/__contains__.
        self.post_abs_trace = self.ai
        self.buf_overflow_helper = AEReporter(self.svfir, self.ai)

        self.widen_delay = 3
        self.addressMask = 0x7f000000
        self.flippedAddressMask = (self.addressMask^0xffffffff)

    # ------------------------------------------------------------------
    # Optional hooks for Tasks 1, 2, 4, 5, 6.  The pre-implemented
    # handleCallSite (in Assignment_3.py) routes ordinary external-API
    # calls through updateStateOnExtCall and then nullptrDerefDetection /
    # bufOverflowDetection.  Override these on your Assignment3 subclass
    # if you want your value-summary modelling and bug checkers to run.
    # ------------------------------------------------------------------
    def updateAbsState(self, stmt):
        pass

    def mergeStatesFromPredecessors(self, block):
        return False, AbstractState()

    def updateStateOnExtCall(self, call):
        pass

    def bufOverflowDetection(self, node):
        pass

    def nullptrDerefDetection(self, node):
        pass


    """
    Initialize the interprocedural WTO per call-graph SCC entry.

    Each (mutually) recursive function's entry node becomes a WTO cycle head
    because intra-SCC call edges are turned into back-edges.  The same
    widening/narrowing machinery used for loops then drives recursion to a
    fixpoint via handleICFGCycle; there is no separate "is recursive?" check.
    """
    def initWto(self):
        callgraphScc = pysvf.getCallGraphSCC()
        self._callgraph_scc = callgraphScc
        callgraph = self.svfir.getCallGraph()
        self._callgraph = callgraph

        # SCC membership comes from pysvf: CallGraphSCC.subNodes(rep) returns
        # the call-graph node IDs in the SCC represented by 'rep'.  We only
        # need it to feed ICFGWTO so intra-SCC call edges become back-edges.
        cgid_to_fun = {}
        for node in callgraph.getNodes():
            cgid_to_fun[node.getId()] = node.getFunction()

        # Build one interprocedural WTO per call-graph-SCC entry function.  An
        # SCC entry is a member with a caller outside the SCC (or no caller).
        # Intra-SCC members are reached via the entry's interprocedural WTO.
        self.func_to_wto = {}
        for node in callgraph.getNodes():
            fun = node.getFunction()
            if fun.isDeclaration():
                continue
            cgid = node.getId()
            rep = callgraphScc.repNode(cgid)
            scc_cgids = set(callgraphScc.subNodes(rep))

            in_edges = list(node.getInEdges())
            is_entry = (len(in_edges) == 0)
            for e in in_edges:
                if e.getSrcID() not in scc_cgids:   # caller outside the SCC
                    is_entry = True
            if not is_entry:
                continue

            func_scc_ids = {cgid_to_fun[c].getId() for c in scc_cgids}
            wto = ICFGWTO(self.icfg, self.icfg.getFunEntryICFGNode(fun), func_scc_ids)
            wto.init()
            # Key by function id: pybind FunObjVar wrappers are not guaranteed to
            # hash consistently across calls, so don't use the object as a key.
            self.func_to_wto[fun.getId()] = wto



    """
    Placeholder for additional documentation or functionality.
    """
    def getVirtualMemAddress(self, idx: int) -> int:
        return self.addressMask + idx


    """
    Handle the global ICFG node by initializing its abstract state and updating it based on its statements.

    This function performs the following steps:
    1. Initializes the abstract state for the global ICFG node in both pre- and post-abstract traces.
    2. Sets the initial value of variable 0 to an address value of 0.
    3. Iterates through all statements in the global ICFG node and updates the abstract state accordingly.
    """
    # handleGlobalNode / handleFunction / handleICFGNode are student TODOs
    # this year and live in Assignment_3.py.

    # handleCallSite is part of the analysis driver and lives in
    # Assignment_3.py.

    def inSameCallGraphSCC(self, fun1, fun2) -> bool:
        scc = getattr(self, "_callgraph_scc", None)
        cg = getattr(self, "_callgraph", None)
        if scc is None or cg is None:
            return False
        n1 = cg.getCallGraphNodeByFunObj(fun1)
        n2 = cg.getCallGraphNodeByFunObj(fun2)
        if n1 is None or n2 is None:
            return False
        return scc.repNode(n1.getId()) == scc.repNode(n2.getId())

    # Whitelist of external-call names the assignment expects students to
    # model in `updateStateOnExtCall`.  Mirrors the C++ side: exact match for
    # assignment-specific and checkpoint stubs; substring match for library
    # APIs whose Clang lowering yields LLVM intrinsics (e.g. `llvm.memcpy.*`).
    _EXT_EXACT_STUBS = frozenset({
        "mem_insert", "str_insert",
        "UNSAFE_BUFACCESS", "SAFE_BUFACCESS",
        "UNSAFE_PTRDEREF", "SAFE_PTRDEREF",
    })
    _EXT_API_SUBSTRINGS = (
        "memcpy", "memmove", "memset",
        "strcpy", "strncpy", "strcat", "strncat",
        "strlen", "wcslen",
    )

    def isExternalCallForAssignment(self, func) -> bool:
        name = func.getName()
        if name in self._EXT_EXACT_STUBS:
            return True
        return any(key in name for key in self._EXT_API_SUBSTRINGS)


    """
    Handle stub functions such as 'svf_assert' and 'OVERFLOW'.

    This function processes specific stub functions in the program's control flow graph (CFG) 
    to validate assertions or detect buffer overflows. It performs the following tasks:

    1. For 'svf_assert':
       - Adds the call node to the set of assertion points.
       - Checks the abstract state of the argument to determine if the assertion is valid.
       - If the assertion is invalid or unsatisfiable, raises an error.

    2. For 'OVERFLOW':
       - Adds the call node to the set of assertion points.
       - Checks if the right-hand side (RHS) value is an address.
       - Iterates through the addresses to calculate the access offset and compare it 
         with the object size to detect buffer overflows.
       - If a buffer overflow is detected, records the overflow node and prints a success message.
       - If no overflow is detected, raises an error.

    :param call_node: The call node representing the stub function in the CFG.
    :type call_node: pysvf.CallICFGNode
    """
    def handleStubFunction(self, callNode: pysvf.CallICFGNode):
        # Get the callee function associated with the call site
        if callNode.getCalledFunction().getName() == "svf_assert":
            self.buf_overflow_helper.noteAssertionPoint(callNode)
            # If the condition is false, the program is infeasible
            arg0 = callNode.getArgument(0).getId()
            abstract_state = self.post_abs_trace[callNode]

            # Check if the interval for the argument is infinite
            if abstract_state[arg0].getInterval().isTop():
                print(f"svf_assert Fail. {callNode}")
                assert False
            else:
                if (abstract_state[arg0].getInterval().equals(IntervalValue(1, 1)) or
                        abstract_state[arg0].getInterval().equals(IntervalValue(-1, -1))):
                    print(f"The assertion ({callNode}) is successfully verified!!")
                else:
                    print(f"The assertion ({callNode}) is unsatisfiable!!")
                    assert False


    def handleCheckpointStubs(self, callNode: pysvf.CallICFGNode):
        """SAFE_/UNSAFE_ checkpoints: ground-truth bug markers.

        Records the call site in ``assert_points`` so
        :py:meth:`ensureAllAssertsValidated` can verify the student's control
        flow reached it.  The harness reports a bug iff its independent
        ground-truth check (bypassing the student's predicates) sees one.
        """
        self.buf_overflow_helper.noteAssertionPoint(callNode)
        fun_name = callNode.getCalledFunction().getName()
        abstract_state = self.post_abs_trace[callNode]
        if fun_name in ("SAFE_BUFACCESS", "UNSAFE_BUFACCESS"):
            if callNode.arg_size() < 2:
                return
            length = abstract_state[callNode.getArgument(1).getId()].getInterval()
            if length.isBottom():
                length = IntervalValue(0)
            ptr = callNode.getArgument(0)
            if not self._harnessSafeAccess(abstract_state, ptr, length - IntervalValue(1)):
                self.buf_overflow_helper.reportBufOverflow(
                    callNode, f"buffer-overflow at {callNode}")
        elif fun_name in ("SAFE_PTRDEREF", "UNSAFE_PTRDEREF"):
            if callNode.arg_size() < 1:
                return
            ptr = callNode.getArgument(0)
            if not self._harnessSafeDeref(abstract_state, ptr):
                self.buf_overflow_helper.reportBufOverflow(
                    callNode, f"nullptr-deref at {callNode}")

    def _harnessSafeAccess(self, abstract_state, value, length: IntervalValue) -> bool:
        ptr_val = abstract_state[value.getId()]
        if not ptr_val.isAddr():
            return True
        for addr in ptr_val.getAddrs():
            if pysvf.AbstractState.isBlackHoleObjAddr(addr) or pysvf.AbstractState.isNullMem(addr):
                continue
            obj_id = abstract_state.getIDFromAddr(addr)
            base_obj = self.svfir.getBaseObject(obj_id)
            if base_obj is None or base_obj.isBlackHoleObj() or not base_obj.isConstantByteSize():
                continue
            size = base_obj.getByteSizeOfObj()
            gnode = self.svfir.getGNode(obj_id)
            base_offset = IntervalValue(gnode.getConstantFieldIdx()) if isinstance(gnode, pysvf.GepObjVar) else IntervalValue(0)
            offset = base_offset + length
            if int(offset.ub()) >= size:
                return False
        return True

    def _harnessSafeDeref(self, abstract_state, value) -> bool:
        if value is None or isinstance(value, pysvf.ConstNullPtrValVar):
            return False
        abs_val = abstract_state[value.getId()]
        if not abs_val.isAddr():
            return True
        for addr in abs_val.getAddrs():
            if pysvf.AbstractState.isBlackHoleObjAddr(addr):
                continue
            if pysvf.AbstractState.isNullMem(addr):
                return False
            if abstract_state.isFreedMem(addr):
                return False
        return True


    # mergeStatesFromPredecessors is a student TODO this year and lives in
    # Assignment_3.py.

    def isBranchFeasible(self, intraEdge: pysvf.IntraCFGEdge, abstractState:  pysvf.AbstractState) -> bool :
        cmp_var = intraEdge.getCondition()
        cmp_in_edges = cmp_var.getInEdges()
        if len(cmp_in_edges) == 0:
            return pysvf.AbstractState.isSwitchBranchFeasible(self.svfir, cmp_var, intraEdge.getSuccessorCondValue(), abstractState)
        else:
            cmp = cmp_in_edges[0]
            if isinstance(cmp, pysvf.CmpStmt):
                return pysvf.AbstractState.isCmpBranchFeasible(self.svfir, cmp, intraEdge.getSuccessorCondValue(), abstractState)
            else:
                return pysvf.AbstractState.isSwitchBranchFeasible(self.svfir, cmp_var, intraEdge.getSuccessorCondValue(), abstractState)





    def ensureAllAssertsValidated(self):
        """Verify the student's control flow reached every ground-truth stub.

        Recognised stubs:
          * ``svf_assert`` / ``svf_assert_eq`` -- abstract-state assertions
          * ``UNSAFE_PTRDEREF`` / ``SAFE_PTRDEREF`` -- null-deref ground truth
          * ``UNSAFE_BUFACCESS`` / ``SAFE_BUFACCESS`` -- buffer-access ground truth

        A missed stub site means the student's control-flow logic skipped a
        place the grader cares about.  Additionally requires that the number
        of reported bugs is at least the number of ``UNSAFE_*`` stubs.
        """
        assert_stubs = {"svf_assert", "svf_assert_eq"}
        checkpoint_stubs = {"UNSAFE_PTRDEREF", "SAFE_PTRDEREF",
                            "UNSAFE_BUFACCESS", "SAFE_BUFACCESS"}
        unsafe_to_be_verified = 0
        for node in self.svfir.getICFG().getNodes():
            if not isinstance(node, pysvf.CallICFGNode):
                continue
            called_function = node.getCalledFunction()
            if not called_function:
                continue
            name = called_function.getName()
            if name not in assert_stubs and name not in checkpoint_stubs:
                continue
            if name.startswith("UNSAFE_"):
                unsafe_to_be_verified += 1
            if not self.buf_overflow_helper.isAssertionPoint(node):
                raise AssertionError(
                    f"The stub function callsite ({name}) was not reached by "
                    f"the student's control flow: {node}"
                )
        assert unsafe_to_be_verified <= len(self.buf_overflow_helper.node_to_bug_info), \
            "The number of UNSAFE_* stubs (ground truth) should <= the number of bugs reported"




    # analyse / updateAbsState / handleCallSite / reportBufOverflow /
    # reportNullDeref live on the student side in Assignment_3.py.

    """
    Initialize an object variable in the abstract state.

    This function determines the initial abstract value for a given object variable
    based on its type and properties. It handles various types of object variables,
    including constants, global variables, and complex structures, and assigns
    appropriate abstract values such as intervals or addresses.

    Steps:
    1. Retrieve the base object associated with the given object variable.
    2. Check the type of the object variable:
       - For constant integer or floating-point variables, return their exact value as an interval.
       - For null pointers, return an interval representing zero.
       - For global variables, return an address value based on a virtual memory address.
       - For constant arrays or structures, return a top interval to represent unknown values.
    3. For other types of object variables, return an address value based on a virtual memory address.

    :param obj_var: The object variable to initialize.
    :type obj_var: pysvf.ObjVar
    :return: The initialized abstract value for the object variable.
    :rtype: pysvf.AbstractValue
    """
    def initObjVar(self, objVar: pysvf.ObjVar):
        var_id = objVar.getId()
        obj = self.svfir.getBaseObject(var_id).asBaseObjVar()
        if obj.isConstDataObjVar() or obj.isConstantArray() or obj.isConstantStruct():
            if isinstance(objVar, pysvf.ConstIntObjVar):
                numeral = objVar.getSExtValue()
                return IntervalValue(numeral, numeral)

            elif isinstance(objVar, pysvf.ConstFPObjVar):
                return IntervalValue(objVar.getFPValue(), objVar.getFPValue())

            elif isinstance(objVar, pysvf.ConstNullPtrObjVar):
                return IntervalValue(0,0)

            elif isinstance(objVar, pysvf.GlobalObjVar):
                return AddressValue(self.getVirtualMemAddress(var_id))

            elif obj.isConstantArray() or obj.isConstantStruct():
                return IntervalValue.top()
            else:
                return IntervalValue.top()
        else:
            return AddressValue(self.getVirtualMemAddress(var_id))


    def updateStateOnAddr(self, addr: pysvf.AddrStmt):
        node = addr.getICFGNode()
        abstract_state = self.post_abs_trace[node]
        assert isinstance(abstract_state, AbstractState)
        abstract_state[addr.getRHSVarID()] = AbstractValue(self.initObjVar(addr.getRHSVar().asObjVar()))
        abstract_state[addr.getLHSVarID()] = abstract_state[addr.getRHSVarID()]







    def updateStateOnCmp(self, cmp: pysvf.CmpStmt):
        node = cmp.getICFGNode()
        abstract_state = self.post_abs_trace[node]
        assert isinstance(abstract_state, AbstractState)
        op0 = cmp.getOpVar(0)
        op1 = cmp.getOpVar(1)
        res = cmp.getResId()
        if abstract_state.getVar(op0.getId()).isInterval() and abstract_state.getVar(op0.getId()).isInterval():
            res_val = IntervalValue(0)
            lhs = abstract_state[op0.getId()].getInterval()
            rhs = abstract_state[op1.getId()].getInterval()
            predicate = cmp.getPredicate()
            if predicate == Predicate.ICMP_EQ or predicate == Predicate.FCMP_OEQ or predicate == Predicate.FCMP_UEQ:
                res_val = lhs.eq_interval(rhs)
            elif predicate == Predicate.ICMP_NE or predicate == Predicate.FCMP_ONE or predicate == Predicate.FCMP_UNE:
                res_val = lhs.ne_interval(rhs)
            elif predicate == Predicate.ICMP_SGT or  predicate == Predicate.FCMP_UGT or predicate == Predicate.FCMP_OGT or predicate == Predicate.FCMP_UGT:
                res_val = (lhs  > rhs)
            elif predicate == Predicate.ICMP_SGE or  predicate == Predicate.FCMP_UGE or predicate == Predicate.FCMP_OGE or predicate == Predicate.FCMP_UGE:
                res_val = (lhs >= rhs)
            elif predicate == Predicate.ICMP_SLT or  predicate == Predicate.ICMP_ULT or predicate == Predicate.FCMP_OLT or predicate == Predicate.FCMP_ULT:
                res_val = (lhs < rhs)
            elif predicate == Predicate.ICMP_SLE or predicate == Predicate.ICMP_ULE or  predicate == Predicate.FCMP_OLE or predicate == Predicate.FCMP_ULE:
                res_val = (lhs <= rhs)
            elif predicate == Predicate.FCMP_FALSE:
                res_val = IntervalValue(0,0)
            elif predicate == Predicate.FCMP_TRUE:
                res_val = IntervalValue(1,1)
            abstract_state[res] = AbstractValue(res_val)
        if abstract_state.getVar(op0.getId()).isAddr() and abstract_state.getVar(op0.getId()).isAddr():
            res_val = None
            lhs = abstract_state[op0.getId()]
            rhs = abstract_state[op1.getId()]
            predicate = cmp.getPredicate()

            if predicate in [Predicate.ICMP_EQ, Predicate.FCMP_OEQ, Predicate.FCMP_UEQ]:
                if len(lhs.getAddrs()) == 1 and len(rhs.getAddrs()) == 1:
                    res_val = IntervalValue(lhs.equals(rhs))
                else:
                    if lhs.getAddrs().hasIntersect(rhs.getAddrs()):
                        res_val = IntervalValue.top()
                    else:
                        res_val = IntervalValue(0)

            elif predicate in [Predicate.ICMP_NE, Predicate.FCMP_ONE, Predicate.FCMP_UNE]:
                if len(lhs.getAddrs()) == 1 and len(rhs.getAddrs()) == 1:
                    res_val = IntervalValue(not lhs.equals(rhs))
                else:
                    if lhs.getAddrs().hasIntersect(rhs.getAddrs()):
                        res_val = IntervalValue.top()
                    else:
                        res_val = IntervalValue(1)

            elif predicate in [Predicate.ICMP_UGT, Predicate.ICMP_SGT, Predicate.FCMP_OGT, Predicate.FCMP_UGT]:
                if len(lhs.getAddrs()) == 1 and len(rhs.getAddrs()) == 1:
                    res_val = IntervalValue(next(iter(lhs.getAddrs())) > next(iter(rhs.getAddrs())))
                else:
                    res_val = IntervalValue.top()

            elif predicate in [Predicate.ICMP_UGE, Predicate.ICMP_SGE, Predicate.FCMP_OGE, Predicate.FCMP_UGE]:
                if len(lhs.getAddrs()) == 1 and len(rhs.getAddrs()) == 1:
                    res_val = IntervalValue(next(iter(lhs.getAddrs())) >= next(iter(rhs.getAddrs())))
                else:
                    res_val = IntervalValue.top()

            elif predicate in [Predicate.ICMP_ULT, Predicate.ICMP_SLT, Predicate.FCMP_OLT, Predicate.FCMP_ULT]:
                if len(lhs.getAddrs()) == 1 and len(rhs.getAddrs()) == 1:
                    res_val = IntervalValue(next(iter(lhs.getAddrs())) < next(iter(rhs.getAddrs())))
                else:
                    res_val = IntervalValue.top()

            elif predicate in [Predicate.ICMP_ULE, Predicate.ICMP_SLE, Predicate.FCMP_OLE, Predicate.FCMP_ULE]:
                if len(lhs.getAddrs()) == 1 and len(rhs.getAddrs()) == 1:
                    res_val = IntervalValue(next(iter(lhs.getAddrs())) <= next(iter(rhs.getAddrs())))
                else:
                    res_val = IntervalValue.top()

            elif predicate == Predicate.FCMP_FALSE:
                res_val = IntervalValue(0, 0)

            elif predicate == Predicate.FCMP_TRUE:
                res_val = IntervalValue(1, 1)

            else:
                assert False, "undefined compare"

            abstract_state[res] = res_val



    def updateStateOnCall(self, call: pysvf.CallPE):
        node = call.getICFGNode()
        abstract_state = self.post_abs_trace[node]
        lhs = call.getLHSVarID()
        rhs = call.getRHSVarID()
        abstract_state[lhs] = abstract_state[rhs]


    def updateStateOnRet(self, ret: pysvf.RetPE):
        node = ret.getICFGNode()
        abstract_state = self.post_abs_trace[node]
        abstract_state[ret.getLHSVarID()] = abstract_state[ret.getRHSVarID()]



    def updateStateOnSelect(self, select: pysvf.SelectStmt):
        node = select.getICFGNode()
        abstract_state = self.post_abs_trace[node]
        assert isinstance(abstract_state, AbstractState)
        res = select.get_res_id()
        tval = select.get_true_value().getId()
        fval = select.get_false_value().getId()
        cond = select.getCondition().getId()
        if abstract_state[cond].getInterval().isInterval():
            if abstract_state[cond].getInterval().is_zero():
                abstract_state[res] = abstract_state[fval]
            else:
                abstract_state[res] = abstract_state[tval]
        else:
            abstract_state[res].joinWith(abstract_state[tval])
            abstract_state[res].joinWith(abstract_state[fval])




    # getAccessOffset is a student TODO this year and lives in Assignment_3.py.
