from Ass3_helper import *
import pysvf

class Assignment3(AbstractExecution):
    def __init__(self, pag: pysvf.SVFIR) -> None:
        super().__init__(pag)


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
    # Find the comparison predicates in "class BinaryOPStmt:OpCode" under SVF/svf/include/SVFIR/SVFStatements.h
    # You are required to handle predicates (The program is assumed to have signed ints and also interger-overflow-free),
    # including Add, FAdd, Sub, FSub, Mul, FMul, SDiv, FDiv, UDiv, SRem, FRem, URem, Xor, And, Or, AShr, Shl, LShr
    def update_state_on_binary(self, store: pysvf.BinaryOpStmt):
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



