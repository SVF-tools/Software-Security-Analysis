import sys
import os
import pysvf
from typing import Union
from AndersenPTA import *

class Ass1_Andersen(AndersenPTA):
    def __init__(self, pag: pysvf.SVFIR):
        super().__init__(pag)

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



class Ass1_ICFGTraversal(ICFGTraversal):
    def __init__(self, pag):
        super().__init__(pag)

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





