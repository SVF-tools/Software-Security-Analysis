from AEReporter import *
import pysvf


# =============================================================================
# Student TODOs
# =============================================================================
# Implement abstract interpretation for verification and bug detection. The
# harness (AEReporter.py) drives the analysis and calls into the entry points
# below (please do not delete); You are free to add any internal classes and
# helper methods you need within Assignment_3.py.
# =============================================================================


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
