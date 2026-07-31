from AEHelper import *
import pysvf


# Student-facing Assignment 3 implementation.
class Assignment3(AbstractExecution):
    def __init__(self, pag: pysvf.SVFIR) -> None:
        super().__init__(pag)

    def handleGlobalNode(self):
        # TODO
        pass

    def handleFunction(self, funEntry: pysvf.ICFGNode):
        # TODO
        pass

    def handleICFGNode(self, node: pysvf.ICFGNode):
        # TODO
        return False

    def handleICFGCycle(self, cycle):
        # TODO
        pass

    def handleCallSite(self, node: pysvf.CallICFGNode):
        # TODO
        pass

    def bufOverflowDetection(self, node: pysvf.ICFGNode):
        # TODO
        pass

    def nullptrDerefDetection(self, node: pysvf.ICFGNode):
        # TODO
        pass
