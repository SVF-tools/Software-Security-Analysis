import pysvf
from typing import Union
class AndersenPTA:
    def __init__(self, pag: pysvf.SVFIR):
        assert isinstance(pag, pysvf.SVFIR), "pag is not a valid SVFIR object, the type of pag is {}".format(type(pag))
        self.pag = pag
        self.ander_base = pysvf.AndersenBase(pag)
        self.consCG = None
        return

    def initialize(self):
        self.ander_base.initialize()
        self.consCG = self.ander_base.getConstraintGraph()

    def initWorklist(self):
        self.ander_base.initWorklist()

    def updateCallGraph(self):
        return self.ander_base.updateCallGraph()

    def pushIntoWorklist(self, node):
        assert isinstance(node, int), "node is not a valid int object, the type of node is {}".format(type(node))
        self.ander_base.pushIntoWorklist(node)

    def finalize(self):
        self.ander_base.finalize()

    def addPts(self, id: int, ptd: int) -> bool:
        assert isinstance(ptd, int), "ptd is not a valid int object, the type of ptd is {}".format(type(ptd))
        assert isinstance(id, int), "id is not a valid int object, the type of id is {}".format(type(id))
        return self.ander_base.addPts(id, ptd)

    def unionPts(self, id: int, ptd: Union[int, pysvf.PointsTo, None]) -> bool:
        assert isinstance(id, int), "id is not a valid int object, the type of id is {}".format(type(id))
        assert isinstance(ptd, (int, pysvf.PointsTo, type(None))), "ptd is not a valid int/PointsTo/None object, the type of ptd is {}".format(type(ptd))
        if isinstance(ptd, pysvf.PointsTo):
            return self.ander_base.unionPts2(id, ptd)
        elif isinstance(ptd, int):
            return self.ander_base.unionPts(id, ptd)
        else:
            raise TypeError("Invalid type for ptd")

    def getPts(self, id: int):
        assert isinstance(id, int), "id is not a valid int object, the type of id is {}".format(type(id))
        return self.ander_base.getPts(id)

    def isWorklistEmpty(self) -> bool:
        return self.ander_base.isWorklistEmpty()

    def popFromWorklist(self):
        return self.ander_base.popFromWorklist()

    def addCopyEdge(self, src_id, dst_id) -> bool:
        assert isinstance(src_id, int), "src_id is not a valid int object, the type of src_id is {}".format(type(src_id))
        assert isinstance(dst_id, int), "dst_id is not a valid int object, the type of dst_id is {}".format(type(dst_id))
        print(f"Adding copy edge from {src_id} to {dst_id}")
        return self.consCG.addCopyCGEdge(src_id, dst_id)

    def alias(self, node1: int, node2: int) -> bool:
        assert isinstance(node1, int), "node1 is not a valid int object, the type of node1 is {}".format(type(node1))
        assert isinstance(node2, int), "node2 is not a valid int object, the type of node2 is {}".format(type(node2))
        return self.ander_base.alias(node1, node2)

    def analyze(self):
        self.initialize()
        self.initWorklist()
        while True:
            reanalyze = False
            self.solveWorklist()
            if self.updateCallGraph():
                reanalyze = True
            if not reanalyze:
                break
        self.finalize()


class ICFGTraversal:
    def __init__(self, pag, ander):
        assert isinstance(pag, pysvf.SVFIR), "pag is not a valid SVFIR object, the type of pag is {}".format(type(pag))
        self.pag = pag
        self.icfg = pag.getICFG()
        self.paths = set()
        self.path = []
        self.visited = set()
        self.callstack = []
        self.sources = set()
        self.sinks = set()
        self.source_names = set()
        self.sink_names = set()
        self.ander = ander


    def identifySources(self):
        for callsite in self.pag.getCallSites():
            fun = callsite.getCalledFunction()
            if fun.getName() in self.source_names:
                self.sources.add(callsite)
        return self.sources

    def identifySinks(self):
        for callsite in self.pag.getCallSites():
            fun = callsite.getCalledFunction()
            if fun.getName() in self.sink_names:
                self.sinks.add(callsite)
        return self.sinks



    '''
    // Start taint checking.
    // There is a tainted flow from p@source to q@sink
    // if (1) alias(p,q)==true and (2) source reaches sink on ICFG.
    '''
    def taintChecking(self):
        self.readSrcSnkFromFile("../Tests/SrcSnk.txt")
        self.ander.analyze()
        for src in self.identifySources():
            for snk in self.identifySinks():
                if self.aliasCheck(src, snk):
                    self.reachability(src, snk)



    def getPaths(self):
        return self.paths



