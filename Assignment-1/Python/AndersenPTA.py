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
        self.consCG = self.ander_base.get_constraint_graph()

    def init_worklist(self):
        self.ander_base.init_worklist()

    def update_call_graph(self):
        return self.ander_base.update_call_graph()


    def push_into_worklist(self, node):
        assert isinstance(node, int), "node is not a valid int object, the type of node is {}".format(type(node))
        self.ander_base.push_into_worklist(node)

    def finalize(self):
        self.ander_base.finalize()

    def add_pts(self, id: int, ptd: int) -> bool:
        assert isinstance(ptd, int), "ptd is not a valid int object, the type of ptd is {}".format(type(ptd))
        assert isinstance(id, int), "id is not a valid int object, the type of id is {}".format(type(id))
        return self.ander_base.add_pts(id, ptd)

    def union_pts(self, id:int, ptd: Union[int, pysvf.PointsTo, None]) -> bool:
        assert isinstance(id, int), "id is not a valid int object, the type of id is {}".format(type(id))
        assert isinstance(ptd, (int, pysvf.PointsTo,type(None))), "ptd is not a valid int/PointsTo/None object, the type of ptd is {}".format(type(ptd))
        if isinstance(ptd, pysvf.PointsTo):
            return self.ander_base.union_pts_2(id, ptd)
        elif isinstance(ptd, int):
            return self.ander_base.union_pts(id, ptd)
        else:
            raise TypeError("Invalid type for ptd")


    def get_pts(self, id:int):
        assert isinstance(id, int), "id is not a valid int object, the type of id is {}".format(type(id))
        return self.ander_base.get_pts(id)

    def is_worklist_empty(self) -> bool:
        return self.ander_base.is_worklist_empty()

    def pop_from_worklist(self):
        return self.ander_base.pop_from_worklist()

    def push_into_worklist(self, node_id):
        assert isinstance(node_id, int), "node_id is not a valid int object, the type of node_id is {}".format(type(node_id))
        self.ander_base.push_into_worklist(node_id)


    def add_copy_edge(self, src_id, dst_id) -> bool:
        assert isinstance(src_id, int), "src_id is not a valid int object, the type of src_id is {}".format(type(src_id))
        assert isinstance(dst_id, int), "dst_id is not a valid int object, the type of dst_id is {}".format(type(dst_id))
        print(f"Adding copy edge from {src_id} to {dst_id}")
        return self.consCG.add_copy_edge(src_id, dst_id)

    def alias(self, node1: int, node2: int) -> bool:
        assert isinstance(node1, int), "node1 is not a valid int object, the type of node1 is {}".format(type(node1))
        assert isinstance(node2, int), "node2 is not a valid int object, the type of node2 is {}".format(type(node2))
        return self.ander_base.alias(node1, node2)


    def analyze(self):
        self.initialize()
        self.init_worklist()
        while 1:
            reanalyze = False
            self.solve_worklist()
            if self.update_call_graph():
                reanalyze = True
            if reanalyze is False:
                break
        self.finalize()


class ICFGTraversal:
    def __init__(self, pag, ander):
        assert isinstance(pag, pysvf.SVFIR), "pag is not a valid SVFIR object, the type of pag is {}".format(type(pag))
        self.pag = pag
        self.icfg = pag.get_icfg()
        self.paths = set()
        self.path = []
        self.visited = set()
        self.callstack = []
        self.sources = set()
        self.sinks = set()
        self.source_names = set()
        self.sink_names = set()
        self.ander = ander


    def identify_sources(self):
        for callsite in self.pag.get_call_sites():
            fun = callsite.get_called_function()
            if fun.get_name() in self.source_names:
                self.sources.add(callsite)
        return self.sources

    def identify_sinks(self):
        for callsite in self.pag.get_call_sites():
            fun = callsite.get_called_function()
            if fun.get_name() in self.sink_names:
                self.sinks.add(callsite)
        return self.sinks



    '''
    // Start taint checking.
    // There is a tainted flow from p@source to q@sink
    // if (1) alias(p,q)==true and (2) source reaches sink on ICFG.
    '''
    def taint_checking(self):
        self.read_srcsnk_from_file("../Tests/SrcSnk.txt")
        self.ander.analyze()
        for src in self.identify_sources():
            for snk in self.identify_sinks():
                if self.alias_check(src, snk):
                    self.reachability(src, snk)



    def get_paths(self):
        return self.paths



