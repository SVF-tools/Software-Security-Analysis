
from Z3SSEMgr import *


class Assignment2(SSE):
    def __init__(self, svfir: pysvf.SVFIR) -> None:
        super().__init__(svfir)

    '''
    /// TODO: collect each path once this method is called during reachability analysis, and
    /// Collect each program path from the entry to each assertion of the program. In this function,
    /// you will need (1) add each path into the paths set, (2) call translatePath to convert each path into Z3 expressions.
    /// Note that translatePath returns true if the path is feasible, false if the path is infeasible. (3) If a path is feasible,
    /// you will need to call assertchecking to verify the assertion (which is the last ICFGNode of this path).
    '''
    def collect_and_translate_path(self, path: list) -> None:
        assert isinstance(path, list), "path is not a valid list, the type of path is {}".format(type(path))
        pass

    '''
    /// TODO: Implement handling of branch statements inside a function
    /// Return A given branch on the ICFG looks like the following:
    ///       	     ICFGNode1 (condition %cmp)
    ///       	     1	/    \  0
    ///       	  ICFGNode2   ICFGNode3
    /// edge->getCondition() returns the branch condition variable (%cmp) of type SVFValue* (for if/else) or a numeric condition variable (for switch).
    /// Given the condition variable, you could obtain the SVFVar ID via "svfir->getValueNode(edge->getCondition())""
    /// edge->getCondition() returns nullptr if this IntraCFGEdge is not a branch.
    /// edge->getSuccessorCondValue() returns the actual condition value (1/0 for if/else) when this branch/IntraCFGEdge is executed. For example, the successorCondValue is 1 on the edge from ICFGNode1 to ICFGNode2, and 0 on the edge from ICFGNode1 to ICFGNode3
 n true if the path is feasible, false otherwise.
    '''
    def handle_branch(self, edge: pysvf.IntraCFGEdge) -> bool:
        assert isinstance(edge, pysvf.IntraCFGEdge), "edge is not a valid IntraCFGEdge object, the type of edge is {}".format(type(edge))
        assert edge.get_condition() and "not a conditional control-flow transfer?"
        cond = self.z3mgr.get_z3_expr(edge.get_condition().get_id(), self.calling_ctx)
        successor_val = self.z3mgr.get_z3_val(edge.get_successor_cond_value())
        pass

    '''
    /// TODO: Implement handling of function calls
    '''
    def handle_call(self, edge: pysvf.CallCFGEdge) -> None:
        assert isinstance(edge, pysvf.CallCFGEdge), "edge is not a valid CallCFGEdge object, the type of edge is {}".format(type(edge))
        pass


    '''
    /// TODO: Implement handling of function returns
    '''
    def handle_ret(self, edge: pysvf.RetCFGEdge) -> None:
        assert isinstance(edge, pysvf.RetCFGEdge), "edge is not a valid RetCFGEdge object, the type of edge is {}".format(type(edge))
        pass

    '''
    /// TODO: Translate AddrStmt, CopyStmt, LoadStmt, StoreStmt, GepStmt and CmpStmt
    /// Translate AddrStmt, CopyStmt, LoadStmt, StoreStmt, GepStmt, BinaryOPStmt, CmpStmt, SelectStmt, and PhiStmt
    '''
    def handle_intra(self, edge: pysvf.IntraCFGEdge) -> bool:
        assert isinstance(edge, pysvf.IntraCFGEdge), "edge is not a valid IntraCFGEdge object, the type of edge is {}".format(type(edge))
        if edge.get_condition():
            if self.handle_branch(edge) is False:
                return False
        pass

    '''
    /// TODO: Implement your context-sensitive ICFG traversal here to traverse each program path (once for any loop) from
    /// You will need to collect each path from src node to snk node and then add the path to the `paths` set by
    /// calling the `collectAndTranslatePath` method which is then trigger the path translation.
    /// This implementation, slightly different from Assignment-1, requires ICFGNode* as the first argument.
    '''
    def reachability(self, cur_edge: pysvf.ICFGEdge, sink: pysvf.ICFGNode) -> None:
        assert isinstance(cur_edge, pysvf.ICFGEdge) and "cur_edge is not a valid IntraCFGEdge object"
        assert isinstance(sink, pysvf.ICFGNode) and "sink is not a valid ICFGNode object"
        pass





# Example usage
if __name__ == "__main__":
    # check sys.argv and print friendly error message if not enough arguments
    if len(sys.argv) != 2:
        print("Usage: python3 Assignment2.py <path-to-bc-file>")
        sys.exit(1)
    bc_file = sys.argv[1]
    pag = pysvf.getPAG(bc_file)
    ass4 = Assignment2(pag)
    ass4.analyse()
