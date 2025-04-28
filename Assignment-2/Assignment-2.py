import z3
import pysvf
import sys
from Ass2_helper import Z3Mgr


class Assignment2:
    def __init__(self, svfir: pysvf.SVFIR) -> None:
        assert isinstance(svfir, pysvf.SVFIR), "svfir is not a valid SVFIR object, the type of svfir is {}".format(type(svfir))
        self.svfir = svfir
        self.icfg = self.svfir.get_icfg()
        self.z3mgr = Z3Mgr(svfir)
        self.calling_ctx = []
        self.paths = []

        self.path = []
        self.visited = set()

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

    #=================Don't modify the code below=================


    def get_z3mgr(self) -> Z3Mgr:
        return self.z3mgr

    def identify_source(self) -> list:
        return [self.icfg.get_global_icfg_node()]

    def identify_sink(self) -> list:
        res = []
        cs = self.svfir.get_call_sites()
        for c in cs:
            func_name = c.get_called_function().get_name()
            if func_name == "assert" or func_name == "svf_assert" or func_name == "sink":
                res.append(c)
        return res

    def is_assert_func(self, func_name: str) -> bool:
        assert isinstance(func_name, str), "func_name is not a valid string, the type of func_name is {}".format(type(func_name))
        return func_name == "assert" or func_name == "svf_assert" or func_name == "sink"

    def reset_solver(self) -> None:
        self.z3mgr.reset_solver()
        self.calling_ctx = []



    def translate_path(self, path: list) -> bool:
        assert isinstance(path, list), "path is not a valid list, the type of path is {}".format(type(path))
        for edge in path:
            if edge.is_intra_cfg_edge():
                if not self.handle_intra(edge):
                    return False
            elif edge.is_call_cfg_edge():
                self.handle_call(edge)
            elif edge.is_ret_cfg_edge():
                self.handle_ret(edge)
            else:
                assert False, "what other edges we have?"
        return True


    def assert_checking(self, inode: pysvf.ICFGNode) -> bool:
        assert isinstance(inode, pysvf.ICFGNode), "inode is not a valid ICFGNode object, the type of inode is {}".format(type(inode))
        assert_checked = 0
        callnode = inode
        assert callnode and self.is_assert_func(callnode.get_called_function().get_name()) and "last node is not an assert call?"
        print(f"## Analyzing {callnode}")
        arg0 = self.z3mgr.get_z3_expr(callnode.get_actual_parms()[0].get_id(), self.calling_ctx)
        self.z3mgr.solver.push()
        self.z3mgr.add_to_solver(arg0 == self.z3mgr.get_z3_val(0))

        if self.z3mgr.solver.check() != z3.unsat:
            self.z3mgr.solver.pop()
            ss = f"The assertion is unsatisfiable!! ({inode})"
            ss += f"Counterexample: {self.z3mgr.solver.model()}"
            print(ss)
            print(self.z3mgr.solver)
            assert False
        else:
            self.z3mgr.solver.pop()
            print(self.z3mgr.solver)
            ss = f"The assertion is successfully verified!! ({inode})"
            print(ss)
            return True

    def push_calling_ctx(self, c: pysvf.ICFGNode) -> None:
        assert isinstance(c, pysvf.ICFGNode), "c is not a valid ICFGNode object, the type of c is {}".format(type(c))
        self.calling_ctx.append(c)

    def pop_calling_ctx(self) -> None:
        self.calling_ctx.pop()



    def analyse(self) -> None:
        for src in self.identify_source():
            assert isinstance(src, pysvf.GlobalICFGNode) and "reachability should start with GlobalICFGNode!"
            for sink in self.identify_sink():
                self.reachability(pysvf.IntraCFGEdge(None, src), sink)
                self.reset_solver()




# Example usage
if __name__ == "__main__":
    # check sys.argv and print friendly error message if not enough arguments
    if len(sys.argv) != 2:
        print("Usage: python3 Assignment-2.py <path-to-bc-file>")
        sys.exit(1)
    bc_file = sys.argv[1]
    pag = pysvf.get_pag(bc_file)
    ass4 = Assignment2(pag)
    ass4.analyse()