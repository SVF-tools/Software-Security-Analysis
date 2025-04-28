import sys
import os
import pysvf
from typing import Union

class AndersenPTA:
    def __init__(self, pag: pysvf.SVFIR):
        assert isinstance(pag, pysvf.SVFIR), "pag is not a valid SVFIR object, the type of pag is {}".format(type(pag))
        self.pag = pag
        self.ander_base = pysvf.AndersenBase(pag)
        self.consCG = None
        return

    '''
    // TODO: Implement your Andersen's Algorithm here
    /// The solving rules are as follows:
    /// p <--Addr-- o        =>  pts(p) = pts(p) ∪ {o}
    /// q <--COPY-- p        =>  pts(q) = pts(q) ∪ pts(p)
    /// q <--LOAD-- p        =>  for each o ∈ pts(p) : q <--COPY-- o
    /// q <--STORE-- p       =>  for each o ∈ pts(q) : o <--COPY-- p
    /// q <--GEP, fld-- p    =>  for each o ∈ pts(p) : pts(q) = pts(q) ∪ {o.fld}
    /// pts(q) denotes the points-to set of q
    '''
    def solve_worklist(self):
        pass

    # =========== Don't modify the code below in this class ============

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
    def __init__(self, pag):
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
        self.ander = AndersenPTA(pag)

    '''
    /// TODO: Implement your code to parse the two lines to identify sources and sinks from `SrcSnk.txt` for your
    /// reachability analysis The format in SrcSnk.txt is in the form of
    /// line 1 for sources  "{ api1 api2 api3 }"
    /// line 2 for sinks    "{ api1 api2 api3 }"
    '''
    def read_srcsnk_from_file(self, filename):
        pass

    '''
    /// TODO: Implement your context-sensitive ICFG traversal here to traverse each program path
    /// by matching calls and returns while maintaining a `callstack`.
    /// Sources and sinks are identified by implementing and calling `readSrcSnkFromFile`
    /// Each path including loops, qualified by a `callstack`, should only be traversed once using a `visited` set.
    /// You will need to collect each path from src to snk and then add the path to the `paths` set.
    /// Add each path (a sequence of node IDs) as a string into std::set<std::string> paths
    /// in the format "START->1->2->4->5->END", where -> indicate an ICFGEdge connects two ICFGNode IDs
    '''
    def reachability(self, cur_node, sink):
        assert isinstance(cur_node, pysvf.ICFGNode), "cur_node is not a valid ICFGNode object, the type of cur_node is {}".format(type(cur_node))
        assert isinstance(sink, pysvf.ICFGNode), "sink is not a valid ICFGNode object, the type of sink is {}".format(type(sink))
        pass

    '''
    /// TODO: Checking aliases of the two variables at source and sink. For example:
    /// src instruction:  actualRet = source();
    /// snk instruction:  sink(actualParm,...);
    /// return true if actualRet is aliased with any parameter at the snk node (e.g., via ander->alias(..,..))
    '''
    def alias_check(self, src, snk) -> bool:
        assert isinstance(src, pysvf.CallICFGNode), "src is not a valid CallICFGNode object, the type of src is {}".format(type(src))
        assert isinstance(snk, pysvf.CallICFGNode), "snk is not a valid CallICFGNode object, the type of snk is {}".format(type(snk))
        pass

    #=========== Don't modify the code below in this class ============

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
        self.read_srcsnk_from_file("SrcSnk.txt")
        self.ander.analyze()
        for src in self.identify_sources():
            for snk in self.identify_sinks():
                if self.alias_check(src, snk):
                    self.reachability(src, snk)



    def get_paths(self):
        return self.paths



def check_icfg_case(module_name, result, expected):
    assert len(result) == len(expected), f"Wrong paths generated - {module_name} failed!"
    for path in result:
        assert path in expected, f"Wrong paths generated - {module_name} failed!"
    print(f"Test case {module_name} passed!")


def test_icfg(module_name_vec):
    pag = pysvf.get_pag(module_name_vec)  # Build Program Assignment Graph (SVFIR)
    icfg = pag.get_icfg()  # Get ICFG
    gt = ICFGTraversal(pag)  # Create ICFG Traversal object

    config_path = os.path.join(os.path.dirname(__file__), "./SrcSnk.txt")
    gt.read_srcsnk_from_file(config_path)

    for src in gt.identify_sources():
        for snk in gt.identify_sinks():
            gt.reachability(src, snk)

    module_name = os.path.basename(module_name_vec)
    if module_name == "test1.ll":
        expected = {"START->3->4->5->END"}
        check_icfg_case(module_name, gt.get_paths(), expected)
    elif module_name == "test2.ll":
        expected = {
            "START->3->4->5->6->7->8->9->END",
            "START->3->4->5->6->7->END",
            "START->5->6->7->8->9->END",
            "START->5->6->7->END"
        }
        check_icfg_case(module_name, gt.get_paths(), expected)
    elif module_name == "test3.ll":
        expected = {"START->6->7->8->1->5->2->9->10->END"}
        check_icfg_case(module_name, gt.get_paths(), expected)
    elif module_name == "test4.ll":
        expected = {"START->12->13->14->3->8->9->1->7->2->10->11->4->15->16->END"}
        check_icfg_case(module_name, gt.get_paths(), expected)
    # Add further test cases as needed...
    elif module_name == "test5.ll":
        expected = {
            "START->6->7->8->9->10->1->5->2->11->14->END",
            "START->6->7->8->9->12->1->5->2->13->16->END",
        }
        check_icfg_case(module_name, gt.get_paths(), expected)
    elif module_name == "test6.ll":
        expected = {
            "START->12->13->14->15->16->3->8->9->1->7->2->10->11->4->17->20->END",
            "START->12->13->14->15->18->3->8->9->1->7->2->10->11->4->19->22->END",
        }
        check_icfg_case(module_name, gt.get_paths(), expected)
    elif  module_name == "test7.ll":
        expected = {"START->17->1->7->END"}
        check_icfg_case(module_name, gt.get_paths(), expected)
    elif module_name == "test8.ll":
        expected = {
            "START->6->7->8->9->10->1->5->2->11->14->END",
            "START->6->7->8->9->12->1->5->2->13->16->END",
        }
        check_icfg_case(module_name, gt.get_paths(), expected)
    elif module_name == "test9.ll":
        expected = {"START->7->8->9->10->11->14->END"}
        check_icfg_case(module_name, gt.get_paths(), expected)
    elif module_name == "test10.ll":
        expected = {
            "START->3->4->5->6->7->9->11->END",
            "START->3->4->5->6->8->10->14->17->END",
        }
        check_icfg_case(module_name, gt.get_paths(), expected)

    else:
        print(f"Test case {module_name} not found!")


def test_pta(module_name_vec):
    pag = pysvf.get_pag(module_name_vec)  # Build Program Assignment Graph (SVFIR)
    andersen_pta = AndersenPTA(pag)
    andersen_pta.analyze()  # Run Andersen pointer analysis
    del andersen_pta


def test_taint(module_name_vec):
    pag = pysvf.get_pag(module_name_vec)  # Build Program Assignment Graph (SVFIR)

    taint = ICFGTraversal(pag)
    taint.taint_checking()  # Perform taint analysis

    module_name_vec = os.path.basename(module_name_vec)
    print(taint.get_paths())
    if module_name_vec == "test1.ll":
        expected = {"START->6->1->5->2->7->8->9->10->END"}
        assert taint.get_paths() == expected, " \n wrong paths generated - test1 failed !"
        print("\n test1 passed !")
    elif module_name_vec == "test4.ll":
        expected = {"START->6->1->5->2->7->8->9->10->11->13->14->END"}
        assert taint.get_paths() == expected, " \n wrong paths generated - test4 failed !"
        print("\n test2 passed !")
    elif module_name_vec == "test2.ll" or module_name_vec == "test3.ll":
        expected = set()
        assert taint.get_paths() == expected, " \n wrong paths generated - test2 or test3 failed !"
        print("\n test2 or test3 passed !")


    print(f"###################### Tainted Information Flow ({len(taint.get_paths())} found) ######################")
    print("---------------------------------------------")
    for path in taint.get_paths():
        origin_path = path
        prefix = "START->"
        suffix = "->END"

        if path.startswith(prefix):
            path = path[len(prefix):]
        if path.endswith(suffix):
            path = path[:-len(suffix)]

        tokens = path.split("->")
        src_id = int(tokens[0])
        dst_id = int(tokens[-1])
        src_node = pag.get_icfg().get_gnode(src_id)
        dst_node = pag.get_icfg().get_gnode(dst_id)

        print(
            f"{origin_path}\nSource: {src_node.to_string()}\nSink: {dst_node.to_string()}\n---------------------------------------------")

    if not taint.get_paths():
        print("No tainted information flow found")


def main():
    pta_enabled = False
    taint_enabled = False
    icfg_enabled = False
    module_name_vec = ""

    args = sys.argv[1:]

    for arg in args:
        if arg == "-pta":
            pta_enabled = True
        elif arg == "-taint":
            taint_enabled = True
        elif arg == "-icfg":
            icfg_enabled = True
        else:
            module_name_vec = arg

    # Default to taint analysis if none specified
    if not (pta_enabled or taint_enabled or icfg_enabled):
        assert False, "No analysis specified. Please specify -pta, -taint, or -icfg."

    assert (pta_enabled + taint_enabled + icfg_enabled) == 1, "Only one analysis can be enabled."

    if module_name_vec == "":
        assert False, "No module specified. Please specify a module to analyze."

    if pta_enabled:
        test_pta(module_name_vec)
    elif taint_enabled:
        test_taint(module_name_vec)
    elif icfg_enabled:
        test_icfg(module_name_vec)


if __name__ == "__main__":
    main()