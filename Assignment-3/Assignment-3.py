import faulthandler
faulthandler.enable()

from Ass3_helper import ICFGWTO, ICFGWTONode, ICFGWTOCycle, ICFGWTOComp, AbstractExecutionHelper
import pysvf
from pysvf import IntervalValue, AddressValue, AbstractValue, AbstractState
import sys
from pysvf.enums import OpCode, Predicate

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
    TODO : Implement the handleCycleWTO function
    """
    def handle_cycle_wto(self, cycle: ICFGWTOCycle):
        pass

    #TODO : Implement the state updates for Copy, Binary, Store, Load, Gep, Phi
    # TODO: your code starts from here
    def update_state_on_gep(self, gep: pysvf.GepStmt):
        pass

    #TODO: your code starts from here
    def update_state_on_store(self, store: pysvf.StoreStmt):
        pass


    #TODO: your code starts from here
    def update_state_on_load(self, load: pysvf.LoadStmt):
        pass

    #TODO: your code starts from here
    def update_state_on_copy(self, copy: pysvf.CopyStmt):
        pass

    # TODO: your code starts from here
    def update_state_on_phi(self, phi: pysvf.PhiStmt):
        pass

    """
    Detect buffer overflows in the given statement.

    TODO: handle GepStmt `lhs = rhs + off` and detect buffer overflow
    Step 1: For each `obj \in pts(rhs)`, get the size of allocated baseobj (entire mem object) via `obj_size = svfir->getBaseObj(objId)->getByteSizeOfObj();`
    There is a buffer overflow if `accessOffset.ub() >= obj_size`, where accessOffset is obtained via `getAccessOffset`
    Step 2: invoke `reportBufOverflow` with the current ICFGNode if an overflow is detected

    :param stmt: The statement to analyze for buffer overflows.
    :type stmt: pysvf.SVFStmt
    """
    def buf_overflow_detection(self, stmt: pysvf.SVFStmt):
        if not isinstance(stmt.get_icfg_node(), pysvf.CallICFGNode):
            if isinstance(stmt, pysvf.GepStmt):
                abstract_state = self.post_abs_trace[stmt.get_icfg_node()]
                lhs = stmt.get_lhs_id()
                rhs = stmt.get_rhs_id()

                # Update GEP object offset from base
                self.buf_overflow_helper.update_gep_obj_offset_from_base(
                    abstract_state[lhs].get_addrs(),  abstract_state[rhs].get_addrs(),
                    abstract_state.get_byte_offset(stmt)
                )

                # TODO: your code starts from here
                pass

    """
    Handle external function calls and update the abstract state.

    This function processes specific external function calls, such as `mem_insert` and `str_insert`,
    to ensure that buffer overflows are detected and prevented. It checks the constraints on the
    buffer size and access offsets based on the function arguments.

    TODO: Steps:
    1. For `mem_insert`:
       - Validate that the buffer size is greater than or equal to the sum of the position and data size.
    2. For `str_insert`:
       - Validate that the buffer size is greater than or equal to the sum of the position and the length of the string.

    :param ext_call_node: The call node representing the external function call.
    :type ext_call_node: pysvf.CallICFGNode
    """
    def update_state_on_ext_call(self, ext_call_node: pysvf.CallICFGNode):
        func_name = ext_call_node.get_called_function().get_name()

        # Handle external calls
        # TODO: handle external calls
        # void mem_insert(void *buffer, const void *data, size_t data_size, size_t position);
        if func_name == "mem_insert":
            pass
        # TODO: handle external calls
        # void str_insert(void *buffer, const void *data, size_t position);
        elif func_name == "str_insert":
            pass

    #==============Don't modify the code below==============
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




    """
    Find the comparison predicates in "class BinaryOPStmt:OpCode" under SVF/svf/include/SVFIR/SVFStatements.h
    You are required to handle predicates (The program is assumed to have signed ints and also interger-overflow-free),
    including Add, FAdd, Sub, FSub, Mul, FMul, SDiv, FDiv, UDiv, SRem, FRem, URem, Xor, And, Or, AShr, Shl, LShr
    """
    def update_state_on_binary(self, binary: pysvf.BinaryOPStmt):
        node = binary.get_icfg_node()
        abstract_state = self.post_abs_trace[node]
        lhs = binary.get_res_id()
        op1 = binary.get_op_var(0)
        op2 = binary.get_op_var(1)
        assert abstract_state.is_interval(op1.get_id()) and abstract_state.is_interval(op2.get_id())
        result = IntervalValue(0)
        val1 = abstract_state[op1.get_id()].get_interval()
        val2 = abstract_state[op2.get_id()].get_interval()
        assert(isinstance(val1, IntervalValue) and isinstance(val2, IntervalValue))
        if binary.get_op() == OpCode.Add or binary.get_op() == OpCode.FAdd:
            result = val1 + val2
        elif binary.get_op() == OpCode.Sub or binary.get_op() == OpCode.FSub:
            result = val1 - val2
        elif binary.get_op() == OpCode.Mul or binary.get_op() == OpCode.FMul:
            result = val1 * val2
        elif binary.get_op() == OpCode.UDiv or binary.get_op() == OpCode.SDiv or binary.get_op() == OpCode.FDiv:
            if int(val2.ub())>=0 and int(val2.lb()) <= 0:
                result = IntervalValue.top()
            else:
                result = val1 / val2
        elif binary.get_op() == OpCode.SRem or binary.get_op() == OpCode.FRem or binary.get_op() == OpCode.URem:
            if int(val2.ub())>=0 and int(val2.lb()) <= 0:
                result = IntervalValue.top()
            else:
                result = val1 % val2
        elif binary.get_op() == OpCode.Xor:
            result = val1 ^ val2
        elif binary.get_op() == OpCode.Or:
            result = val1 | val2
        elif binary.get_op() == OpCode.And:
            result = val1 & val2
        elif binary.get_op() == OpCode.Shl:
            result = val1 << val2
        elif binary.get_op() == OpCode.LShr or binary.get_op() == OpCode.AShr:
            result = val1 >> val2
        else:
            result = IntervalValue.top()
        abstract_state[lhs] = AbstractValue(result)



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





if __name__ == "__main__":
    # check sys.argv and print friendly error message if not enough arguments
    if len(sys.argv) != 2:
        print("Usage: python3 Assignment-3.py <path-to-bc-file>")
        sys.exit(1)
    bc_file = sys.argv[1]
    pag = pysvf.get_pag(bc_file)
    pag.get_icfg().dump("icfg")
    ass3 = AbstractExecution(pag)
    ass3.analyse()
    pysvf.release_pag()
