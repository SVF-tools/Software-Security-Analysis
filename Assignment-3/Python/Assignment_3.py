from Assignment_3_Helper import *
import pysvf

class Assignment3(AbstractExecution):
    def __init__(self, pag: pysvf.SVFIR) -> None:
        super().__init__(pag)


    """
    TODO : Implement the handleCycleWTO function
    """
    def handleCycleWto(self, cycle: ICFGWTOCycle):
        pass

    #TODO : Implement the state updates for Copy, Binary, Store, Load, Gep, Phi
    # TODO: your code starts from here
    def updateStateOnGep(self, gep: pysvf.GepStmt):
        pass

    #TODO: your code starts from here
    def updateStateOnStore(self, store: pysvf.StoreStmt):
        pass

    #TODO: your code starts from here
    # Find the comparison predicates in "class BinaryOPStmt:OpCode" under SVF/svf/include/SVFIR/SVFStatements.h
    # You are required to handle predicates (The program is assumed to have signed ints and also interger-overflow-free),
    # including Add, FAdd, Sub, FSub, Mul, FMul, SDiv, FDiv, UDiv, SRem, FRem, URem, Xor, And, Or, AShr, Shl, LShr
    def updateStateOnBinary(self, binary: pysvf.BinaryOPStmt):
        pass


    #TODO: your code starts from here
    def updateStateOnLoad(self, load: pysvf.LoadStmt):
        pass

    #TODO: your code starts from here
    def updateStateOnCopy(self, copy: pysvf.CopyStmt):
        pass

    # TODO: your code starts from here
    def updateStateOnPhi(self, phi: pysvf.PhiStmt):
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
    def bufOverflowDetection(self, stmt: pysvf.SVFStmt):
        if not isinstance(stmt.getICFGNode(), pysvf.CallICFGNode):
            if isinstance(stmt, pysvf.GepStmt):
                abstract_state = self.post_abs_trace[stmt.getICFGNode()]
                lhs = stmt.getLHSVarID()
                rhs = stmt.getRHSVarID()

                # Update GEP object offset from base
                self.buf_overflow_helper.updateGepObjOffsetFromBase(abstract_state,
                    abstract_state[lhs].getAddrs(),  abstract_state[rhs].getAddrs(),
                    abstract_state.getByteOffset(stmt)
                )

                # TODO: your code starts from here
                # Check for buffer overflow
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
    def updateStateOnExtCall(self, extCallNode: pysvf.CallICFGNode):
        func_name = extCallNode.getCalledFunction().getName()

        # Handle external calls
        # TODO: handle external calls
        # void mem_insert(void *buffer, const void *data, size_t data_size, size_t position);
        if func_name == "mem_insert":
            # void mem_insert(void *buffer, const void *data, size_t data_size, size_t position);
            # Check sizeof(buffer) >= position + data_size
            pass
        # TODO: handle external calls
        # void str_insert(void *buffer, const void *data, size_t position);
        elif func_name == "str_insert":
            # void str_insert(void *buffer, const void *data, size_t position);
            # Check sizeof(buffer) >= position + strlen(data)
            pass