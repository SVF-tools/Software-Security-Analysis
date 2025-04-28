from abc import abstractmethod

from pysvf import ICFG, ICFGNode
from typing import List, Dict, Set, Optional
import pysvf

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


