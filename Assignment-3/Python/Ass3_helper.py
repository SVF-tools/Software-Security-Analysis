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


    def get_icfg_node(self) -> ICFGNode:
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
            head = cycle.head.get_icfg_node()
            previous_cycle_depth = self.wto_cycle_depth
            self.node_to_wto_cycle_depth[head] = self.wto_cycle_depth
            self.wto_cycle_depth = WTOCycleDepth()
            self.wto_cycle_depth.add(head)
            for component in cycle.components:
                component.accept(self)
            self.wto_cycle_depth = previous_cycle_depth

        def visitNode(self, node: ICFGWTONode):
            self.node_to_wto_cycle_depth[node.get_icfg_node()] = self.wto_cycle_depth


    def __init__(self, graph: ICFG, entry: ICFGNode):
        self.graph = graph
        self.entry = entry
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
        if isinstance(node, pysvf.CallICFGNode):
            return [node.get_ret_node()]
        else:
            return [edge.get_dst() for edge in node.get_out_edges()]

    def build_node_to_depth(self):
        builder = self.WTOCycleDepthBuilder(self.node_to_depth)
        for component in self.components:
            component.accept(builder)


    def __str__(self):
        return f"ICFGWTO: {self.components}"



class AbstractExecutionHelper:
    """
    A helper class for abstract execution, providing functionality for bug reporting,
    managing GEP object offsets, and other utilities.
    """

    def __init__(self, svfir: pysvf.SVFIR):
        """
        Initialize member variables.
        """
        # Map a GEP objVar to its offset from the base address
        # Example: alloca [i32*10] x; lhs = gep x, 3
        # gep_obj_offset_from_base[lhs] = [12, 12]
        self.gep_obj_offset_from_base = {}

        # Map to store exception information for each ICFGNode
        self.node_to_bug_info = {}
        self.svfir = svfir

    def report_buf_overflow(self, node, msg):
        """
        Record an overflow node and its associated exception.
        """
        self.node_to_bug_info[node] = msg

    def print_bug_info(self):
        """
        Print the bug information for each node.
        """
        for node, msg in self.node_to_bug_info.items():
            print(f"Node: {node}, Bug Info: {msg}")


    def update_gep_obj_offset_from_base(self, gep_addrs: pysvf.AddressValue, obj_addrs: pysvf.AddressValue, offset: pysvf.IntervalValue):
        """
        Update the GEP object offset from the base address.

        :param gep_addrs: List of GEP address values.
        :param obj_addrs: List of object address values.
        :param offset: IntervalValue representing the offset.
        """
        for obj_addr in obj_addrs:
            obj_id = pysvf.AddressValue.get_internal_id(obj_addr)
            obj = self.svfir.get_gnode(obj_id)
            if isinstance(obj, pysvf.BaseObjVar):
                for gep_addr in gep_addrs:
                    gep_obj = pysvf.AddressValue.get_internal_id(gep_addr)
                    gep_obj_var = self.svfir.get_gnode(gep_obj)
                    self.add_to_gep_obj_offset_from_base(gep_obj_var, offset)
            elif isinstance(obj, pysvf.GepObjVar):
                obj_var = obj
                for gep_addr in gep_addrs:
                    gep_obj = pysvf.AddressValue.get_internal_id(gep_addr)
                    gep_obj_var = self.svfir.get_gnode(gep_obj)
                    if self.has_gep_obj_offset_from_base(obj_var):
                        obj_offset_from_base = self.get_gep_obj_offset_from_base(obj_var)
                        # Ensure gep_obj_var has not been written before
                        if not self.has_gep_obj_offset_from_base(gep_obj_var):
                            self.add_to_gep_obj_offset_from_base(gep_obj_var, obj_offset_from_base + offset)
                    else:
                        raise AssertionError("gepRhsObjVar has no gepObjOffsetFromBase")


    def get_strlen(self, abstract_state, str_value):
        """
        Calculate the length of a string in the abstract state.

        :param abstract_state: The abstract state containing variable information.
        :param str_value: The SVF variable representing the string.
        :return: An IntervalValue representing the string length.
        """
        value_id = str_value.get_id()
        dst_size = 0
        max_limit = 10000  # Prevent infinite or corrupted symbolic memory

        # Determine the size of the destination object
        for addr in abstract_state[value_id].get_addrs():
            obj_id = pysvf.AddressValue.get_internal_id(addr)

            try:
                base_object = self.svfir.get_base_object(obj_id)
            except Exception as e:
                print(f"[warn] failed to get base object for obj_id {obj_id}: {e}")
                continue

            if not base_object:
                continue

            if base_object.is_constant_byte_size():
                dst_size = base_object.get_byte_size_of_obj()
            else:
                icfg_node = base_object.get_icfg_node()
                for stmt in icfg_node.get_svf_stmts():
                    if isinstance(stmt, pysvf.AddrStmt):
                        try:
                            dst_size = abstract_state.get_alloca_inst_byte_size(stmt)
                            break
                        except Exception as e:
                            print(f"[warn] failed to get alloca size: {e}")
                            continue

            # If we've determined a reasonable size, stop
            if dst_size > 0:
                break

        # Safety cap
        if dst_size == 0 or dst_size > max_limit:
            dst_size = max_limit

        length = 0
        elem_size = 1

        # Calculate string length
        if abstract_state.is_addr(value_id):
            for index in range(dst_size):
                try:
                    expr0 = abstract_state.get_gep_obj_addrs(value_id, pysvf.IntervalValue(index))
                except Exception as e:
                    print(f"[warn] get_gep_obj_addrs failed at index {index}: {e}")
                    break

                val = pysvf.AbstractValue()

                for addr in expr0:
                    try:
                        val.join_with(abstract_state.load(addr))
                    except Exception as e:
                        print(f"[warn] load from addr {addr} failed: {e}")
                        continue

                iv = val.get_interval()
                if iv.is_numeral():
                    try:
                        if chr(iv.get_int_numeral()) == '\0':
                            break
                    except:
                        break
                length += 1

            # Determine element size
            try:
                ty = str_value.get_type()
                if ty.is_array_ty():
                    elem_size = ty.get_type_of_element().get_byte_size()
                elif ty.is_pointer_ty():
                    elem_type = abstract_state.get_pointee_element(value_id)
                    if elem_type:
                        if elem_type.is_array_ty():
                            elem_size = elem_type.get_type_of_element().get_byte_size()
                        else:
                            elem_size = elem_type.get_byte_size()
                    else:
                        elem_size = 1
                else:
                    raise AssertionError("Unsupported type")
            except Exception as e:
                print(f"[warn] failed to get element size: {e}")
                elem_size = 1

        # Return as IntervalValue
        if length == 0:
            return pysvf.IntervalValue(0, pysvf.Options.max_field_limit())
        else:
            return pysvf.IntervalValue(length * elem_size)


    def get_strlen(self, abstract_state, str_value):
        """
        Calculate the length of a string in the abstract state.

        :param abstract_state: The abstract state containing variable information.
        :param str_value: The SVF variable representing the string.
        :return: An IntervalValue representing the string length.
        """
        value_id = str_value.get_id()
        dst_size = 0

        # Determine the size of the destination object
        for addr in abstract_state[value_id].get_addrs():
            obj_id = pysvf.AddressValue.get_internal_id(addr)
            base_object = self.svfir.get_base_object(obj_id)

            if base_object.is_constant_byte_size():
                dst_size = base_object.get_byte_size_of_obj()
            else:
                icfg_node = base_object.get_icfg_node()
                for stmt in icfg_node.get_svf_stmts():
                    if isinstance(stmt, pysvf.AddrStmt):
                        dst_size = abstract_state.get_alloca_inst_byte_size(stmt)

        length = 0
        elem_size = 1

        # Calculate the string length
        if abstract_state.is_addr(value_id):
            for index in range(dst_size):
                expr0 = abstract_state.get_gep_obj_addrs(value_id, pysvf.IntervalValue(index))
                val = pysvf.AbstractValue()

                for addr in expr0:
                    val.join_with(abstract_state.load(addr))

                if val.get_interval().is_numeral() and chr(val.get_interval().get_int_numeral()) == '\0':
                    break

                length += 1

            # Determine the size of each element in the string
            if str_value.get_type().is_array_ty():
                elem_size = str_value.get_type().get_type_of_element().get_byte_size()
            elif str_value.get_type().is_pointer_ty():
                elem_type = abstract_state.get_pointee_element_type(value_id)
                if elem_type:
                    if elem_type.is_array_ty():
                        elem_size = elem_type.get_type_of_element().get_byte_size()
                    else:
                        elem_size = elem_type.get_byte_size()
                else:
                    elem_size = 1
            else:
                raise AssertionError("Unsupported type")

        # Return the calculated string length as an IntervalValue
        if length == 0:
            return pysvf.IntervalValue(0, pysvf.Options.max_field_limit())
        else:
            return pysvf.IntervalValue(length * elem_size)

    def add_to_gep_obj_offset_from_base(self, obj, offset):
        """
        Add a GEP object variable and its offset from the base address.

        :param obj: The GEP object variable.
        :param offset: The offset as an IntervalValue.
        """
        self.gep_obj_offset_from_base[obj] = offset

    def has_gep_obj_offset_from_base(self, obj):
        """
        Check if a GEP object variable has an offset from the base address.

        :param obj: The GEP object variable.
        :return: True if the offset exists, False otherwise.
        """
        return obj in self.gep_obj_offset_from_base

    def get_gep_obj_offset_from_base(self, obj):
        """
        Get the offset of a GEP object variable from the base address.

        :param obj: The GEP object variable.
        :return: The offset as an IntervalValue.
        :raises AssertionError: If the object is not found.
        """
        if obj not in self.gep_obj_offset_from_base:
            raise AssertionError(f"Object {obj} not found in gep_obj_offset_from_base")
        else:
            return self.gep_obj_offset_from_base[obj]


class AbstractExecution:
    def __init__(self, pag: pysvf.SVFIR):
        self.svfir = pag
        self.icfg = pag.get_icfg()
        self.call_site_stack = []
        self.func_to_wto = {}
        self.recursive_funs = set()
        self.pre_abs_trace = {}
        self.post_abs_trace = {}
        self.buf_overflow_helper = AbstractExecutionHelper(self.svfir)
        self.assert_points = set()
        self.widen_delay = 3
        self.addressMask = 0x7f000000
        self.flippedAddressMask = (self.addressMask^0xffffffff)


    """
    Initialize the WTO (Weak topological order) for each function.
    """
    def init_wto(self):
        for node in self.svfir.get_call_graph().get_nodes():
            fun = node.get_function()
            assert isinstance(fun, pysvf.FunObjVar)
            if fun.is_declaration():
                continue
            wto = ICFGWTO(self.icfg, self.icfg.get_fun_entry_icfg_node(fun))
            wto.init()
            self.func_to_wto[fun] = wto


    """
    Placeholder for additional documentation or functionality.
    """
    def get_virtual_mem_address(self, idx: int) -> int:
        return self.addressMask + idx


    """
    Handle the global ICFG node by initializing its abstract state and updating it based on its statements.

    This function performs the following steps:
    1. Initializes the abstract state for the global ICFG node in both pre- and post-abstract traces.
    2. Sets the initial value of variable 0 to an address value of 0.
    3. Iterates through all statements in the global ICFG node and updates the abstract state accordingly.
    """
    def handle_global_node(self):
        self.post_abs_trace[self.icfg.get_global_icfg_node()] = AbstractState()
        self.pre_abs_trace[self.icfg.get_global_icfg_node()] = self.post_abs_trace[self.icfg.get_global_icfg_node()]
        self.post_abs_trace[self.icfg.get_global_icfg_node()][0]  = AbstractValue(AddressValue(set()))
        for stmt in self.icfg.get_global_icfg_node().get_svf_stmts():
            self.update_abs_state(stmt)


    """
    Handle the WTO (Weak Topological Order) components.
    This function iterates through the WTO components and handles them based on their type.
    It calls the appropriate helper function for each component, such as handle_singleton_wto or handle_cycle_wto.
    """
    def handle_wto_components(self, wto_comps):
        for comp in wto_comps:
            if isinstance(comp, ICFGWTONode):
                self.handle_singleton_wto(comp)
            elif isinstance(comp, ICFGWTOCycle):
                self.handle_cycle_wto(comp)


    """
    Handle a singleton WTO
    """
    def handle_singleton_wto(self, singleWto: ICFGWTONode):
        node = singleWto.node
        is_feasible, self.pre_abs_trace[node] = self.merge_states_from_predecessors(node)
        if is_feasible:
            self.post_abs_trace[node] = self.pre_abs_trace[node]
        else:
            return

        for stmt in node.get_svf_stmts():
            self.update_abs_state(stmt)
            self.buf_overflow_detection(stmt)

        if isinstance(node, pysvf.CallICFGNode):
            fun_name = node.get_called_function().get_name()
            if fun_name == 'OVERFLOW' or fun_name == 'svf_assert' or fun_name == 'svf_assert_eq':
                pass
                self.handle_stub_function(node)
            else:
                self.handle_call_site(node)


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
    def handle_stub_function(self, call_node: pysvf.CallICFGNode):
        # Get the callee function associated with the call site
        if call_node.get_called_function().get_name() == "svf_assert":
            self.assert_points.add(call_node)
            # If the condition is false, the program is infeasible
            arg0 = call_node.get_arg(0).get_id()
            abstract_state = self.post_abs_trace[call_node]

            # Check if the interval for the argument is infinite
            if abstract_state[arg0].get_interval().is_top():
                print(f"svf_assert Fail. {call_node}")
                assert False
            else:
                if (abstract_state[arg0].get_interval().equals(IntervalValue(1, 1)) or
                        abstract_state[arg0].get_interval().equals(IntervalValue(-1, -1))):
                    print(f"The assertion ({call_node}) is successfully verified!!")
                else:
                    print(f"The assertion ({call_node}) is unsatisfiable!!")
                    assert False

        elif call_node.get_called_function().get_name() == "OVERFLOW":
            # If the condition is false, the program is infeasible
            self.assert_points.add(call_node)
            arg0 = call_node.get_arg(0).get_id()
            arg1 = call_node.get_arg(1).get_id()

            abstract_state = self.post_abs_trace[call_node]
            gep_rhs_val = abstract_state[arg0]

            # Check if the RHS value is an address
            if gep_rhs_val.is_addr():
                overflow = False
                for addr in gep_rhs_val.get_addrs():
                    access_offset = abstract_state[arg1].get_interval().get_int_numeral()
                    obj_id = pysvf.AddressValue.get_internal_id(addr)
                    gep_lhs_obj_var = self.svfir.get_gnode(obj_id).as_gep_obj_var()
                    size = self.svfir.get_base_object(obj_id).get_byte_size_of_obj()

                    if self.buf_overflow_helper.has_gep_obj_offset_from_base(gep_lhs_obj_var):
                        overflow = (
                                int(self.buf_overflow_helper.get_gep_obj_offset_from_base(gep_lhs_obj_var).ub())
                                + access_offset >= size
                        )
                        if overflow:
                            print("obj len: {}, you want to access: {}.".format(size, access_offset))
                    else:
                        raise AssertionError("Pointer not found in gepObjOffsetFromBase")

                if overflow:
                    print("Your implementation successfully detected the buffer overflow")
                else:
                    print(f"Your implementation failed to detect the buffer overflow! {call_node}")
                    assert False
            else:
                print(f"Your implementation failed to detect the buffer overflow! {call_node}")
                assert False



    """
    Handle a call site in the interprocedural control flow graph (ICFG).

    This function processes a call node in the ICFG by determining the callee function
    and taking appropriate actions based on its type. It handles external calls, 
    recursive calls, and function calls with weak topological order (WTO) components.

    Steps:
    1. Identify the callee function associated with the call site.
    2. If the callee is an external function (e.g., 'mem_insert' or 'str_insert'), 
       update the abstract state accordingly and return.
    3. For other functions:
       - Push the call node onto the call site stack to track the call hierarchy.
       - If the callee has WTO components, process them recursively.
       - If the callee does not have WTO components, merge the abstract states 
         from its predecessors.
    4. Pop the call node from the call site stack after processing.

    :param call_node: The call node representing the function call in the ICFG.
    :type call_node: pysvf.CallICFGNode
    """
    def handle_call_site(self, call_node: pysvf.CallICFGNode):

        # Get the callee function associated with the call site
        callee = call_node.get_called_function()

        if callee.get_name() == 'mem_insert' or callee.get_name() == 'str_insert':
            self.update_state_on_ext_call(call_node)
            return
        else:
            self.call_site_stack.append(call_node)

            if callee in self.func_to_wto:
                # Handle the callee function
                wto = self.func_to_wto[callee]
                self.handle_wto_components(wto.components)

                # Pop the call node from the call site stack
                self.call_site_stack.pop()
            else:
                res, self.post_abs_trace[call_node] = self.merge_states_from_predecessors(call_node)
                pass



    """
    Merge abstract states from the predecessors of a given ICFG node.

    This function collects and combines the abstract states from all incoming edges
    of the specified ICFG node. It ensures that the abstract state of the current node
    is consistent with the states of its predecessors. The merging process involves
    joining the abstract states from all valid incoming edges.

    Steps:
    1. Initialize an empty abstract state and a counter for valid incoming edges.
    2. Iterate through all incoming edges of the given block:
       - If the source node of the edge has a post-abstract state, process the edge.
       - For intra-procedural edges with conditions, check branch feasibility.
       - If the branch is feasible, join the source's abstract state with the current state.
       - For inter-procedural or unconditional edges, directly join the source's state.
    3. If no valid incoming edges are found, print an error and return failure.
    4. Return a tuple indicating whether the merge was successful and the merged abstract state.

    :param block: The ICFG node whose predecessors' states are to be merged.
    :type block: pysvf.ICFGNode
    :return: A tuple (is_feasible, merged_state), where:
             - is_feasible (bool): True if at least one predecessor exists, False otherwise.
             - merged_state (AbstractState): The resulting merged abstract state.
    """
    def merge_states_from_predecessors(self, block: pysvf.ICFGNode):
        in_edge_num = 0
        abstract_state = pysvf.AbstractState()
        for edge in block.get_in_edges():
            if edge.get_src() in self.post_abs_trace:
                if isinstance(edge, pysvf.IntraCFGEdge):
                    if edge.get_condition():
                        tmp_es = self.post_abs_trace[edge.get_src()].clone()
                        if self.is_branch_feasible(edge, tmp_es):
                            abstract_state.join_with(tmp_es)
                            in_edge_num += 1
                        else:
                            pass
                    else:
                        abstract_state.join_with(self.post_abs_trace[edge.get_src()])
                        in_edge_num += 1
                else:
                    abstract_state.join_with(self.post_abs_trace[edge.get_src()])
                    in_edge_num += 1
            else:
                pass
        if in_edge_num == 0:
            print(f"Error: No predecessors for block {block.get_id()}")
            return (False, None)
        return (True, abstract_state)


    def is_branch_feasible(self, intra_edge: pysvf.IntraCFGEdge, abstract_state:  pysvf.AbstractState) -> bool :
        cmp_var = intra_edge.get_condition()
        cmp_in_edges = cmp_var.get_in_edges()
        if len(cmp_in_edges) == 0:
            return pysvf.AbstractState.is_switch_branch_feasible(self.svfir, cmp_var, intra_edge.get_successor_cond_value(), abstract_state)
        else:
            cmp = cmp_in_edges[0]
            if isinstance(cmp, pysvf.CmpStmt):
                return pysvf.AbstractState.is_cmp_branch_feasible(self.svfir, cmp, intra_edge.get_successor_cond_value(), abstract_state)
            else:
                return pysvf.AbstractState.is_switch_branch_feasible(self.svfir, cmp_var, intra_edge.get_successor_cond_value(), abstract_state)





    def ensure_all_asserts_validated(self):
        svf_assert_count = 0
        overflow_count = 0

        for node in self.svfir.get_icfg().get_nodes():
            if isinstance(node, pysvf.CallICFGNode):
                called_function = node.get_called_function()
                if called_function:
                    function_name = called_function.get_name()
                    if function_name in ["svf_assert", "OVERFLOW"]:
                        if function_name == "svf_assert":
                            svf_assert_count += 1
                        elif function_name == "OVERFLOW":
                            overflow_count += 1
                        else:
                            pass

                        if node not in self.assert_points:
                            raise AssertionError(
                                f"The stub function callsite (svf_assert or OVERFLOW) has not been checked: {node}"
                            )
        if len(self.assert_points) == svf_assert_count + overflow_count:
            print("All svf_assert and OVERFLOW are validated.")
        else:
            raise AssertionError(
                f"Not all svf_assert and OVERFLOW are validated. {len(self.assert_points)} svf_assert and OVERFLOW are validated, but {svf_assert_count + overflow_count} svf_assert and OVERFLOW are found."
            )





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
        self.init_wto()
        self.handle_global_node()
        # # Process the main function if it exists
        main_fun = self.svfir.get_fun_obj_var("main")
        if main_fun:
            # Arguments of main are initialized as top to represent all possible inputs
            for i in range(main_fun.arg_size()):
                as_state = self.pre_abs_trace[self.icfg.get_global_icfg_node()]
                as_state[main_fun.get_arg(i).get_id()] = IntervalValue.top()

            wto = self.func_to_wto[main_fun]
            self.handle_wto_components(wto.components)
        self.ensure_all_asserts_validated()


    """
    Update the abstract state based on the given statement.
    This function updates the abstract state based on the given statement.
    """
    def update_abs_state(self, stmt:pysvf.SVFStmt):
        if isinstance(stmt, pysvf.AddrStmt):
            self.update_state_on_addr(stmt)
        elif isinstance(stmt, pysvf.BinaryOPStmt):
            self.update_state_on_binary(stmt)
        elif isinstance(stmt, pysvf.CmpStmt):
            self.update_state_on_cmp(stmt)
        elif isinstance(stmt, pysvf.LoadStmt):
            self.update_state_on_load(stmt)
        elif isinstance(stmt, pysvf.StoreStmt):
            self.update_state_on_store(stmt)
        elif isinstance(stmt, pysvf.CopyStmt):
            self.update_state_on_copy(stmt)
        elif isinstance(stmt, pysvf.GepStmt):
            self.update_state_on_gep(stmt)
        #phi
        elif isinstance(stmt, pysvf.PhiStmt):
            self.update_state_on_phi(stmt)
        # callpe
        elif isinstance(stmt, pysvf.CallPE):
            self.update_state_on_call(stmt)
        # retpe
        elif isinstance(stmt, pysvf.RetPE):
            self.update_state_on_ret(stmt)
        #select
        elif isinstance(stmt, pysvf.SelectStmt):
            self.update_state_on_select(stmt)
        elif isinstance(stmt, pysvf.UnaryOPStmt) or isinstance(stmt, pysvf.BranchStmt):
            pass
        else:
            assert False , "Unhandled statement type"

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
    def init_obj_var(self, obj_var: pysvf.ObjVar):
        var_id = obj_var.get_id()
        obj = self.svfir.get_base_object(var_id).as_base_obj_var()
        if obj.is_const_data_obj_var() or obj.is_constant_array() or obj.is_constant_struct():
            if isinstance(obj_var, pysvf.ConstIntObjVar):
                numeral = obj_var.get_sext_value()
                return IntervalValue(numeral, numeral)

            elif isinstance(obj_var, pysvf.ConstFPObjVar):
                return IntervalValue(obj_var.get_fp_value(), obj_var.get_fp_value())

            elif isinstance(obj_var, pysvf.ConstNullPtrObjVar):
                return IntervalValue(0,0)

            elif isinstance(obj_var, pysvf.GlobalObjVar):
                return AddressValue(self.get_virtual_mem_address(var_id))

            elif obj.is_constant_array() or obj.is_constant_struct():
                return IntervalValue.top()
            else:
                return IntervalValue.top()
        else:
            return AddressValue(self.get_virtual_mem_address(var_id))


    def update_state_on_addr(self, addr: pysvf.AddrStmt):
        node = addr.get_icfg_node()
        abstract_state = self.post_abs_trace[node]
        assert isinstance(abstract_state, AbstractState)
        abstract_state[addr.get_rhs_id()] = AbstractValue(self.init_obj_var(addr.get_rhs_var().as_obj_var()))
        abstract_state[addr.get_lhs_id()] = abstract_state[addr.get_rhs_id()]







    def update_state_on_cmp(self, cmp: pysvf.CmpStmt):
        node = cmp.get_icfg_node()
        abstract_state = self.post_abs_trace[node]
        assert isinstance(abstract_state, AbstractState)
        op0 = cmp.get_op_var(0)
        op1 = cmp.get_op_var(1)
        res = cmp.get_res_id()
        if abstract_state.is_interval(op0.get_id()) and abstract_state.is_interval(op1.get_id()):
            res_val = IntervalValue(0)
            lhs = abstract_state[op0.get_id()].get_interval()
            rhs = abstract_state[op1.get_id()].get_interval()
            predicate = cmp.get_predicate()
            if predicate == Predicate.ICMP_EQ or predicate == Predicate.FCMP_OEQ or predicate == Predicate.FCMP_UEQ:
                res_val = lhs.eq_interval(rhs)
            elif predicate == Predicate.ICMP_NE or predicate == Predicate.FCMP_ONE or predicate == Predicate.FCMP_UNE:
                res_val = lhs.ne_interval(rhs)
            elif predicate == Predicate.ICMP_SGT or predicate == Predicate.FCMP_OGT or predicate == Predicate.FCMP_UGT:
                res_val = (lhs  > rhs)
            elif predicate == Predicate.ICMP_SGE or predicate == Predicate.FCMP_OGE or predicate == Predicate.FCMP_UGE:
                res_val = (lhs >= rhs)
            elif predicate == Predicate.ICMP_SLT or predicate == Predicate.FCMP_OLT or predicate == Predicate.FCMP_ULT:
                res_val = (lhs < rhs)
            elif predicate == Predicate.ICMP_SLE or predicate == Predicate.FCMP_OLE or predicate == Predicate.FCMP_ULE:
                res_val = (lhs <= rhs)
            elif predicate == Predicate.FCMP_FALSE:
                res_val = IntervalValue(0,0)
            elif predicate == Predicate.FCMP_TRUE:
                res_val = IntervalValue(1,1)
            abstract_state[res] = AbstractValue(res_val)
        if abstract_state.is_addr(op0.get_id()) and abstract_state.is_addr(op1.get_id()):
            res_val = None
            lhs = abstract_state[op0.get_id()]
            rhs = abstract_state[op1.get_id()]
            predicate = cmp.get_predicate()

            if predicate in [Predicate.ICMP_EQ, Predicate.FCMP_OEQ, Predicate.FCMP_UEQ]:
                if len(lhs.get_addrs()) == 1 and len(rhs.get_addrs()) == 1:
                    res_val = IntervalValue(lhs.equals(rhs))
                else:
                    if lhs.get_addrs().has_intersect(rhs.get_addrs()):
                        res_val = IntervalValue.top()
                    else:
                        res_val = IntervalValue(0)

            elif predicate in [Predicate.ICMP_NE, Predicate.FCMP_ONE, Predicate.FCMP_UNE]:
                if len(lhs.get_addrs()) == 1 and len(rhs.get_addrs()) == 1:
                    res_val = IntervalValue(not lhs.equals(rhs))
                else:
                    if lhs.get_addrs().has_intersect(rhs.get_addrs()):
                        res_val = IntervalValue.top()
                    else:
                        res_val = IntervalValue(1)

            elif predicate in [Predicate.ICMP_UGT, Predicate.ICMP_SGT, Predicate.FCMP_OGT, Predicate.FCMP_UGT]:
                if len(lhs.get_addrs()) == 1 and len(rhs.get_addrs()) == 1:
                    res_val = IntervalValue(next(iter(lhs.get_addrs())) > next(iter(rhs.get_addrs())))
                else:
                    res_val = IntervalValue.top()

            elif predicate in [Predicate.ICMP_UGE, Predicate.ICMP_SGE, Predicate.FCMP_OGE, Predicate.FCMP_UGE]:
                if len(lhs.get_addrs()) == 1 and len(rhs.get_addrs()) == 1:
                    res_val = IntervalValue(next(iter(lhs.get_addrs())) >= next(iter(rhs.get_addrs())))
                else:
                    res_val = IntervalValue.top()

            elif predicate in [Predicate.ICMP_ULT, Predicate.ICMP_SLT, Predicate.FCMP_OLT, Predicate.FCMP_ULT]:
                if len(lhs.get_addrs()) == 1 and len(rhs.get_addrs()) == 1:
                    res_val = IntervalValue(next(iter(lhs.get_addrs())) < next(iter(rhs.get_addrs())))
                else:
                    res_val = IntervalValue.top()

            elif predicate in [Predicate.ICMP_ULE, Predicate.ICMP_SLE, Predicate.FCMP_OLE, Predicate.FCMP_ULE]:
                if len(lhs.get_addrs()) == 1 and len(rhs.get_addrs()) == 1:
                    res_val = IntervalValue(next(iter(lhs.get_addrs())) <= next(iter(rhs.get_addrs())))
                else:
                    res_val = IntervalValue.top()

            elif predicate == Predicate.FCMP_FALSE:
                res_val = IntervalValue(0, 0)

            elif predicate == Predicate.FCMP_TRUE:
                res_val = IntervalValue(1, 1)

            else:
                assert False, "undefined compare"

            abstract_state[res] = res_val



    def update_state_on_call(self, call: pysvf.CallPE):
        node = call.get_icfg_node()
        abstract_state = self.post_abs_trace[node]
        lhs = call.get_lhs_id()
        rhs = call.get_rhs_id()
        abstract_state[lhs] = abstract_state[rhs]


    def update_state_on_ret(self, ret: pysvf.RetPE):
        node = ret.get_icfg_node()
        abstract_state = self.post_abs_trace[node]
        abstract_state[ret.get_lhs_id()] = abstract_state[ret.get_rhs_id()]



    def update_state_on_select(self, select: pysvf.SelectStmt):
        node = select.get_icfg_node()
        abstract_state = self.post_abs_trace[node]
        assert isinstance(abstract_state, AbstractState)
        res = select.get_res_id()
        tval = select.get_true_value().get_id()
        fval = select.get_false_value().get_id()
        cond = select.get_condition().get_id()
        if abstract_state[cond].get_interval().is_numeral():
            if abstract_state[cond].get_interval().is_zero():
                abstract_state[res] = abstract_state[fval]
            else:
                abstract_state[res] = abstract_state[tval]
        else:
            abstract_state[res].join_with(abstract_state[tval])
            abstract_state[res].join_with(abstract_state[fval])




    """
    Calculate the access offset for a given object ID and GEP statement.

    This function determines the offset of a memory access relative to the base address
    of an object. It handles different types of objects, including base objects, sub-objects
    of aggregate objects, and dummy objects. The offset is calculated using the abstract
    state and helper functions.

    :param obj_id: The ID of the object being accessed.
    :type obj_id: int
    :param gep: The GEP (GetElementPtr) statement representing the memory access.
    :type gep: pysvf.GepStmt
    :return: The calculated access offset as an IntervalValue.
    :rtype: pysvf.IntervalValue
    """
    def get_access_offset(self, obj_id: int, gep: pysvf.GepStmt) -> pysvf.IntervalValue:
        obj = self.svfir.get_gnode(obj_id)
        abstract_state = self.post_abs_trace[gep.get_icfg_node()]

        # Field-insensitive base object
        if isinstance(obj, pysvf.BaseObjVar):
            # Get base size
            access_offset = abstract_state.get_byte_offset(gep)
            return access_offset

        # A sub-object of an aggregate object
        elif isinstance(obj, pysvf.GepObjVar):
            access_offset = (
                    self.buf_overflow_helper.get_gep_obj_offset_from_base(obj)
                    + abstract_state.get_byte_offset(gep)
            )
            return access_offset

        else:
            assert isinstance(obj, pysvf.DummyObjVar), "What other types of object?"
            return pysvf.IntervalValue.top()





