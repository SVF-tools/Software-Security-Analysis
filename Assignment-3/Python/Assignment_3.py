from Assignment_3_Helper import *
import pysvf


class Assignment3(AbstractExecution):
    def __init__(self, pag: pysvf.SVFIR) -> None:
        super().__init__(pag)

    # Dispatch a single SVF statement to the matching transfer function.
    def updateAbsState(self, stmt: pysvf.SVFStmt):
        # TODO: dispatch on statement subtype and update the abstract state.
        pass

    # Join predecessor post-states (with branch refinement) into the
    # current node's pre-state.
    def mergeStatesFromPredecessors(self, block: pysvf.ICFGNode,
                                    abstract_state: pysvf.AbstractState) -> bool:
        # TODO
        return False

    # Iterate the cycle body to a fixpoint (widening optional).
    def handleICFGCycle(self, cycle):
        # TODO
        pass

    # Detect out-of-bounds memory accesses at `node`.
    def bufOverflowDetection(self, node: pysvf.ICFGNode):
        # TODO
        pass

    # Model external library calls (memory/string families and
    # assignment-specific stubs).
    def updateStateOnExtCall(self, call: pysvf.CallICFGNode):
        # TODO
        pass

    # Handle a call site in the control-flow graph.
    def handleCallSite(self, node: pysvf.CallICFGNode):
        # TODO
        pass
