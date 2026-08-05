"""Harness for Assignment-3 abstract interpretation.

Owns the AbstractExecution class's harness-side methods — the analysis driver,
interprocedural WTO construction (initWto), stub / checkpoint sub-dispatchers
(handleStubFunction, handleCheckpointStubs) invoked from the student's
handleCallSite override in Assignment_3.py, the external-API whitelist
(isExternalCallForAssignment), access to the Assignment-3-owned
abstract-state trace, and the validator
(ensureAllAssertsValidated).

The AEReporter class (pure bug reporting + JSON / coverage summary plus
the GEP / strlen / memcpy helpers used by the bug checkers) lives in
AEReporter.py. The six assignment features live in Assignment_3.py.
"""

from abc import ABC, abstractmethod
import faulthandler

import pysvf
from pysvf import ICFG, AbstractValue, ICFGNode, IntervalValue
from pysvf.enums import Predicate

from AEReporter import AEReporter
from AEState import AEState, unwrap_state

faulthandler.enable()


class WTOCycleDepth:
    def __init__(self):
        self._heads: list[ICFGNode] = []

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
    def __init__(self, head: ICFGWTONode, components: list[ICFGWTOComp]):
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
        self.components: list[ICFGWTOComp] = []
        self.all_components : set[ICFGWTOComp] = set()
        self.head_ref_to_cycle: dict[ICFGNode, ICFGWTOCycle] = {}
        self.node_to_depth: dict[ICFGNode, int] = {}

        self._num = 0
        self._CDN: dict[ICFGNode, int] = {}
        self._stack: list[ICFGNode] = []

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

    def visit(self, node: ICFGNode, components: list[ICFGWTOComp]):
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

    def get_successors(self, node: ICFGNode) -> list[ICFGNode]:
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


class AbstractExecution(ABC):
    def __init__(self, pag: pysvf.SVFIR):
        self.svfir = pag
        self.icfg = pag.getICFG()
        self.call_site_stack = []
        self.func_to_wto = {}
        self.pre_abs_trace = {}
        self.post_abs_trace = {}
        self.buf_overflow_helper = AEReporter(self.svfir)

        self.widen_delay = 3
        self.addressMask = 0x7f000000
        self.flippedAddressMask = (self.addressMask^0xffffffff)

    # =========================================================================
    # FUNCTION STUBS TO IMPLEMENT FOR ASSIGNMENT 3
    # =========================================================================

    @abstractmethod
    def handleGlobalNode(self):
        # TODO
        pass

    @abstractmethod
    def handleFunction(self, funEntry: pysvf.ICFGNode):
        # TODO
        pass

    @abstractmethod
    def handleICFGNode(self, node: pysvf.ICFGNode) -> bool:
        # TODO
        return False

    @abstractmethod
    def handleICFGCycle(self, cycle):
        # TODO
        pass

    @abstractmethod
    def handleCallSite(self, node: pysvf.CallICFGNode):
        # TODO
        pass

    @abstractmethod
    def bufOverflowDetection(self, node: pysvf.ICFGNode):
        # TODO
        pass

    @abstractmethod
    def nullptrDerefDetection(self, node: pysvf.ICFGNode):
        # TODO
        pass

    # =========================================================================
    # END ASSIGNMENT 3 FUNCTION STUBS
    # =========================================================================

    def getAEState(self, node: pysvf.ICFGNode) -> AEState:
        if node not in self.post_abs_trace:
            self.post_abs_trace[node] = AEState()
        return self.post_abs_trace[node]

    def _storePostState(self, node: pysvf.ICFGNode,
                        state: pysvf.AbstractState):
        self.post_abs_trace[node] = AEState(unwrap_state(state).clone())

    def analyse(self):
        self.initWto()
        self.handleGlobalNode()

        main_fun = self.svfir.getFunObjVar("main")
        assert main_fun, "Main function not found"
        entry = self.icfg.getFunEntryICFGNode(main_fun)
        entry_state = self.getAEState(
            self.icfg.getGlobalICFGNode()).clone()
        for i in range(main_fun.arg_size()):
            entry_state[main_fun.getArg(i).getId()] = IntervalValue.top()
        self.pre_abs_trace[entry] = AEState(entry_state.clone())
        self._storePostState(entry, entry_state)
        self.handleFunction(entry)
        self.ensureAllAssertsValidated()
        self.buf_overflow_helper.printReport()

    def reportBufOverflow(self, node, msg=None):
        self.buf_overflow_helper.reportBufOverflow(
            node, msg if msg is not None else f"buffer-overflow at {node}")

    def reportNullDeref(self, node, msg=None):
        self.buf_overflow_helper.reportBufOverflow(
            node, msg if msg is not None else f"nullptr-deref at {node}")

    def initWto(self):
        """
        Initialize the interprocedural WTO per call-graph SCC entry.
    
        Each (mutually) recursive function's entry node becomes a WTO cycle head
        because intra-SCC call edges are turned into back-edges.  The same
        widening/narrowing machinery used for loops then drives recursion to a
        fixpoint via handleICFGCycle; there is no separate "is recursive?" check.
        """
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

    def getVirtualMemAddress(self, idx: int) -> int:
        return self.addressMask + idx

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

    # Whitelist of external-call names covered by the assignment. Mirrors the
    # C++ side: exact match for assignment-specific and checkpoint stubs;
    # substring match for library APIs whose Clang lowering yields LLVM
    # intrinsics (e.g. `llvm.memcpy.*`).
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

    def handleStubFunction(self, callNode: pysvf.CallICFGNode):
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
        elif callNode.getCalledFunction().getName() == "svf_assert_eq":
            self.buf_overflow_helper.noteAssertionPoint(callNode)
            arg0 = callNode.getArgument(0).getId()
            arg1 = callNode.getArgument(1).getId()
            abstract_state = self.post_abs_trace[callNode]
            if abstract_state[arg0].getInterval().equals(
                    abstract_state[arg1].getInterval()):
                print("The assertion is successfully verified!!")
            else:
                print(f"svf_assert_eq Fail. {callNode}")
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

    def updateStateOnCall(self, call: pysvf.CallPE):
        node = call.getICFGNode()
        abstract_state = self.post_abs_trace[node]
        joined = AbstractValue()
        for index in range(call.getOpVarNum()):
            call_node = call.getOpCallICFGNode(index)
            if call_node not in self.post_abs_trace:
                continue
            joined.join_with(
                self.post_abs_trace[call_node][call.getOpVarId(index)])
        abstract_state[call.getResId()] = joined
