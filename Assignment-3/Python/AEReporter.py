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



class AEReporter:
    """
    Bug reporter and shared helper utilities for the Assignment-3
    abstract-interpretation harness.
    """

    def __init__(self, svfir: pysvf.SVFIR, svf_state_mgr: 'pysvf.AbstractInterpretation' = None):
        # Map ICFGNode -> diagnostic message for each detected bug.
        self.node_to_bug_info = {}
        self.svfir = svfir
        self.svf_state_mgr = svf_state_mgr
        # Harness bookkeeping: stub call sites the analysis actually reached.
        self.assert_points = set()

    def noteAssertionPoint(self, call):
        self.assert_points.add(call)

    def isAssertionPoint(self, call) -> bool:
        return call in self.assert_points

    def getByteOffset(self, abstract_state: pysvf.AbstractState, gep: pysvf.GepStmt) -> pysvf.IntervalValue:
        return self.svf_state_mgr.getGepByteOffset(gep)

    def getGepObjAddrs(self, abstract_state: pysvf.AbstractState, var_id: int, offset: pysvf.IntervalValue) -> pysvf.AddressValue:
        pointer = self.svfir.getGNode(var_id)
        return self.svf_state_mgr.getGepObjAddrs(pointer, offset)

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
        return self.svf_state_mgr.getAllocaInstByteSize(addr)

    def reportBufOverflow(self, node, msg):
        self.node_to_bug_info[node] = msg

    def printReport(self):
        if len(self.node_to_bug_info) > 0:
            print("######################Buffer Overflow ({} found)######################".format(len(self.node_to_bug_info)))
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
        self.svf_state_mgr = pysvf.AbstractInterpretation.getAEInstance()
        # Alias preserved so existing call-sites `self.post_abs_trace[node]`
        # keep working. The mgr supports __getitem__/__setitem__/__contains__.
        self.post_abs_trace = self.svf_state_mgr
        self.buf_overflow_helper = AEReporter(self.svfir, self.svf_state_mgr)

        self.widen_delay = 3
        self.addressMask = 0x7f000000
        self.flippedAddressMask = (self.addressMask^0xffffffff)


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
    def handleGlobalNode(self):
        self.post_abs_trace[self.icfg.getGlobalICFGNode()] = AbstractState()
        self.pre_abs_trace[self.icfg.getGlobalICFGNode()] = self.post_abs_trace[self.icfg.getGlobalICFGNode()]
        self.post_abs_trace[self.icfg.getGlobalICFGNode()][0]  = AbstractValue(AddressValue(set()))
        for stmt in self.icfg.getGlobalICFGNode().getSVFStmts():
            self.updateAbsState(stmt)


    """
    Iterate a function's interprocedural WTO components.

    Singletons are handled directly; cycles (loop heads AND recursive-function
    entries) are driven to a fixpoint by handleICFGCycle.  Recursive callsites
    are filtered out earlier in handleCallSite via ``inSameCallGraphSCC``, so
    handleFunction never re-enters itself.
    """
    def handleFunction(self, funEntry: pysvf.ICFGNode):
        fun = funEntry.getFun()
        wto = self.func_to_wto.get(fun.getId())
        if wto is None:
            return
        for comp in wto.components:
            if isinstance(comp, ICFGWTOCycle):
                self.handleICFGCycle(comp)
            elif isinstance(comp, ICFGWTONode):
                self.handleICFGNode(comp.getICFGNode())

    """
    Handle a singleton WTO
    This function handles a node in the ICFG by merging the abstract states of its predecessors,
    updating the abstract state based on the node's statements, and handling stub functions.
    It also checks if the abstract state has reached a fixpoint and returns the result.
    Return true means the abstract state has changed
    Return false means the abstract state has reached a fixpoint or is infeasible
    
    """
    def handleICFGNode(self, node: pysvf.ICFGNode):
        is_feasible, self.pre_abs_trace[node] = self.mergeStatesFromPredecessors(node)
        if not is_feasible:
            print(f"Infeasible for node {node.getId()}")
            return False
        
        # Store the last abstract state, used to check if the abstract state has reached a fixpoint
        last_as = self.post_abs_trace[node] if node in self.post_abs_trace else None
        self.post_abs_trace[node] = self.pre_abs_trace[node]
        
        for stmt in node.getSVFStmts():
            self.updateAbsState(stmt)
            self.bufOverflowDetection(stmt)

        if isinstance(node, pysvf.CallICFGNode):
            self.handleCallSite(node)
        
        # If the abstract state is the same as the last abstract state, return false because we have reached fixpoint
        if last_as is not None and self.post_abs_trace[node] == last_as:
            return False
        
        return True

    """
    Handle a call site in the control flow graph
    
    This function processes a call site by updating the abstract state, handling the called function,
    and managing the call stack. It resumes the execution state after the function call.
    return void
    """
    def handleCallSite(self, node: pysvf.CallICFGNode):
        fun_name = node.getCalledFunction().getName()
        print(fun_name)
        if fun_name == "svf_assert" or fun_name == "svf_assert_eq":
            self.handleStubFunction(node)
        elif fun_name in ("UNSAFE_BUFACCESS", "SAFE_BUFACCESS",
                          "UNSAFE_PTRDEREF", "SAFE_PTRDEREF"):
            self.handleCheckpointStubs(node)
        elif fun_name == "nd" or fun_name == "rand":
            lhs_id = node.getRetICFGNode().getActualRet().getId()
            self.post_abs_trace[node][lhs_id] = AbstractValue(IntervalValue.top())
        elif fun_name == "mem_insert" or fun_name == "str_insert": #isExternalCallForAssignment
            self.updateStateOnExtCall(node)
        elif pysvf.isExtCall(node.getCalledFunction()):
            pass
        else:
            # Skip recursive callsites (within the same call-graph SCC): the
            # interprocedural WTO built in initWto already encoded this as a
            # back-edge, so the outer cycle's widen/narrow iteration in
            # handleICFGCycle drives the recursion to a fixpoint.  Mirrors
            # SVF's AbstractInterpretation::skipRecursiveCall.
            callee = node.getCalledFunction()
            caller = node.getCaller()
            if caller is not None and self.inSameCallGraphSCC(caller, callee):
                return
            self.handleFunction(self.svfir.getICFG().getFunEntryICFGNode(callee))

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




    """
    Perform the main analysis of the program.

    This function initializes the Weak Topological Order (WTO) for all functions,
    processes the global ICFG node, and analyzes the main function if it exists.
    It ensures that the abstract states are properly initialized and updated
    throughout the analysis.

    Steps:
    1. Initialize the WTO for all functions in the program.
    2. Process the global ICFG node to initialize its abstract state.
    3. If the main function exists:
       - Initialize its arguments as top to represent all possible inputs.
       - Process its WTO components to analyze its control flow.
    """
    def analyse(self):
        self.initWto()
        self.handleGlobalNode()
        # # Process the main function if it exists
        main_fun = self.svfir.getFunObjVar("main")
        if main_fun:
            # Arguments of main are initialized as top to represent all possible inputs
            for i in range(main_fun.arg_size()):
                as_state = self.pre_abs_trace[self.icfg.getGlobalICFGNode()]
                as_state[main_fun.getArg(i).getId()] = IntervalValue.top()

            self.handleFunction(self.icfg.getFunEntryICFGNode(main_fun))
        else:
            assert False, "Main function not found"
        self.ensureAllAssertsValidated()
        self.buf_overflow_helper.printReport()


    # updateAbsState is a student TODO this year and lives in Assignment_3.py.

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
