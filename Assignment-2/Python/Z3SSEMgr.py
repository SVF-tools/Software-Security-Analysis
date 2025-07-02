import z3
import pysvf
import sys

assert_checked = False

class Z3Mgr:
    def __init__(self, svfir: pysvf.SVFIR) -> None:
        assert isinstance(svfir, pysvf.SVFIR), "svfir is not a valid SVFIR object, the type of svfir is {}".format(type(svfir))
        self.pag = svfir

        self.ctx = z3.Context()
        self.solver = z3.Solver(ctx=self.ctx)
        self.var_id_to_expr_map = {}
        self.max_num_of_expr = svfir.getPAGNodeNum()*10
        self.current_expr_idx = 0

        self.addressMask = 0x7f000000
        self.flippedAddressMask = (self.addressMask^0xffffffff)

        index_sort = z3.IntSort(self.ctx)
        element_sort = z3.IntSort(self.ctx)
        array_sort = z3.ArraySort(index_sort, element_sort)
        self.loc_to_val_map = z3.Const('loc2ValMap', array_sort)

    def getInternalId(self, addr: int) -> int:
        assert isinstance(addr, int), "addr is not a valid integer, the type of addr is {}".format(type(addr))
        return addr & self.flippedAddressMask

    def createExprForObjVar(self, obj_var: pysvf.ObjVar) -> z3.ExprRef:
        assert isinstance(obj_var, pysvf.ObjVar), "obj_var is not a valid ObjVar object, the type of obj_var is {}".format(type(obj_var))
        base_obj_var = self.pag.getBaseObject(obj_var.getId())
        #print("Create Obj",str(base_obj_var))
        if base_obj_var.isConstDataOrAggData() or base_obj_var.isConstantArray() or base_obj_var.isConstantStruct():
            if base_obj_var.isConstIntObjVar():
                obj = base_obj_var.asConstIntObjVar()
                return z3.IntVal(obj.getSExtValue(), self.ctx)
            elif base_obj_var.asConstData().isConstFPObjVar():
                obj = base_obj_var.asConstData().asConstFPObjVar()
                return z3.IntVal(obj.getFPValue(), self.ctx)
            elif base_obj_var.isGlobalObjVar():
                return z3.IntVal(self.getVirtualMemAddress(obj_var.getId()), self.ctx)
            elif base_obj_var.isConstantArray() or base_obj_var.isConstantStruct():
                assert False, "implement this part"
            else:
                assert False, "what other types of values we have?"
        else:
            return z3.IntVal(self.getVirtualMemAddress(obj_var.getId()), self.ctx)


    def storeValue(self, loc: z3.ExprRef, value: z3.ExprRef) -> z3.ExprRef:
        """
        Store a value at a memory location.

        Args:
            loc: Memory location expression
            value: Value expression to store

        Returns:
            Updated memory map expression
        """
        assert isinstance(loc, z3.ExprRef), "loc is not a valid z3 expression, the type of loc is {}".format(type(loc))
        assert isinstance(value, z3.ExprRef), "value is not a valid z3 expression, the type of value is {}".format(type(value))
        deref = self.getEvalExpr(loc)
        assert self.isVirtualMemAddress(deref.as_long()), "Pointer operand is not a physical address"
        self.loc_to_val_map = z3.Store(self.loc_to_val_map, deref, value)
        return self.loc_to_val_map


    def loadValue(self, loc: z3.ExprRef) -> z3.ExprRef:
        """
        Load a value from a memory location.

        Args:
            loc: Memory location expression

        Returns:
            Value stored at the location
        """
        assert isinstance(loc, z3.ExprRef), "loc is not a valid z3 expression, the type of loc is {}".format(type(loc))
        deref = self.getEvalExpr(loc)
        assert self.isVirtualMemAddress(deref.as_long()), "Pointer operand is not a physical address"
        return z3.Select(self.loc_to_val_map, deref)

    def getEvalExpr(self, e: z3.ExprRef) -> z3.ExprRef:
        assert isinstance(e, z3.ExprRef), "e is not a valid z3 expression, the type of e is {}".format(type(e))
        res = self.solver.check()
        assert res != z3.unsat, "unsatisfied constraints! Check your contradictory constraints added to the solver"
        model = self.solver.model()
        return model.eval(e)


    def getZ3Expr(self, idx: int, callingCtx: list) -> z3.ExprRef:
        assert isinstance(idx, int), "idx is not a valid integer, the type of idx is {}".format(type(idx))
        assert isinstance(callingCtx, list), "callingCtx is not a valid list, the type of callingCtx is {}".format(type(callingCtx))
        assert self.getInternalId(idx) == idx, "idx cannot be addressValue > 0x7f000000."
        svf_var = self.pag.getGNode(idx)
        if svf_var.isObjVar():
            return self.createExprForObjVar(svf_var.asObjVar())
        else:
            if not isinstance(svf_var, pysvf.ConstIntValVar) and not isinstance(svf_var, pysvf.ConstIntObjVar):
                pass
                #print(self.callingCtx_to_str(callingCtx))
            else:
                pass
            name = "ValVar" + str(idx)
            return z3.Int(name, self.ctx)


    def callingCtxToStr(self, callingCtx: list) -> str:
        assert isinstance(callingCtx, list), "callingCtx is not a valid list, the type of callingCtx is {}".format(type(callingCtx))
        rawstr = ""
        rawstr += "ctx:[ "
        for node in callingCtx:
            rawstr += str(node.get_id()) + " "
        rawstr += "] "
        return rawstr


    def updateZ3Expr(self, idx: int, target: z3.ExprRef) -> None:
        assert isinstance(idx, int), "idx is not a valid integer, the type of idx is {}".format(type(idx))
        assert isinstance(target, z3.ExprRef), "target is not a valid z3 expression, the type of target is {}".format(type(target))
        if self.max_num_of_expr < idx + 1:
            raise IndexError("idx out of bound for map access, increase map size!")
        self.var_id_to_expr_map[idx] = target

    def getZ3Val(self, val:int) -> z3.ExprRef:
        assert isinstance(val, int), "val is not a valid integer, the type of val is {}".format(type(val))
        return z3.IntVal(val, self.ctx)

    def isVirtualMemAddress(self, val: int) -> bool:
        assert isinstance(val, int), "val is not a valid integer, the type of val is {}".format(type(val))
        return val > 0 and (val & self.addressMask) == self.addressMask

    def getVirtualMemAddress(self, idx: int) -> int:
        assert isinstance(idx, int), "idx is not a valid integer, the type of idx is {}".format(type(idx))
        return self.addressMask + idx

    def getMemobjAddress(self, addr: int) -> z3.ExprRef:
        assert isinstance(addr, int), "addr is not a valid integer, the type of addr is {}".format(type(addr))
        obj_idx = self.getInternalId(addr)
        assert(self.pag.getGNode(obj_idx).isObjVar()), "Invalid memory object index"
        return self.createExprForObjVar(self.pag.getGNode(obj_idx).asObjVar())

    def z3ExprToNumValue(self, expr: z3.ExprRef) -> int:
        assert isinstance(expr, z3.ExprRef), "expr is not a valid z3 expression, the type of expr is {}".format(type(expr))
        val = z3.simplify(self.getEvalExpr(expr))
        if isinstance(val, z3.IntNumRef):
            return val.as_long()
        elif val.is_numeral():
            return val.as_long()
        else:
            assert False, "this expression is not numeral"
            sys.exit(1)

    def getGepobjAddress(self, base_expr: z3.ExprRef, offset: int) -> z3.ExprRef:
        assert isinstance(base_expr, z3.ExprRef), "base_expr is not a valid z3 expression, the type of base_expr is {}".format(type(base_expr))
        assert isinstance(offset, int), "offset is not a valid integer, the type of offset is {}".format(type(offset))

        obj = self.getInternalId(self.z3ExprToNumValue(base_expr))
        assert(self.pag.getGNode(obj).isObjVar()), "Fail to get the base object address!"
        gep_obj = self.pag.getGepObjVar(obj, offset)
        if obj == gep_obj:
            return self.createExprForObjVar(self.pag.getGNode(obj).asObjVar())
        else:
            return self.createExprForObjVar(self.pag.getGNode(gep_obj).asGepObjVar())


    def getGepOffset(self, gep: pysvf.GepStmt, callingCtx: list) -> int:
        assert isinstance(gep, pysvf.GepStmt), "gep is not a valid GepStmt object, the type of gep is {}".format(type(gep))
        assert isinstance(callingCtx, list), "callingCtx is not a valid list, the type of callingCtx is {}".format(type(callingCtx))
        if len(gep.getOffsetVarAndGepTypePairVec()) == 0:
            return gep.getConstantStructFldIdx()
        total_offset = 0
        for i in range(len(gep.getOffsetVarAndGepTypePairVec()) - 1, -1, -1):
            var = gep.getOffsetVarAndGepTypePairVec()[i][0]
            type = gep.getOffsetVarAndGepTypePairVec()[i][1]
            offset = 0
            if var.isConstIntValVar():
                offset = var.asConstIntValVar().getSExtValue()
            else:
                offset = self.getEvalExpr(self.getZ3Expr(var.getId(), callingCtx)).as_long()
            if type is None:
                total_offset += offset
                continue
            if type.isPointerType():
                total_offset += offset * self.pag.getNumOfFlattenElements(gep.getSrcPointeeType())
            else:
                total_offset += self.pag.getFlattenedElemIdx(type, offset)
        return total_offset

    def printExprValues(self, callingCtx: list):
        assert isinstance(callingCtx, list), "callingCtx is not a valid list, the type of callingCtx is {}".format(type(callingCtx))
        print_val_map = {}
        obj_key_map = {}
        val_key_map = {}
        for nIter in self.pag:
            idx = nIter[0]
            node = nIter[1]
            e = self.getEvalExpr(self.getZ3Expr(idx, callingCtx))
            if z3.is_int_value(e) == False:
                continue
            if isinstance(node, pysvf.ValVar):
                if self.isVirtualMemAddress(e.as_long()):
                    valstr = "\t Value: " + hex(e.as_long()) + "\n"
                else:
                    valstr = "\t Value: " + str(e.as_long()) + "\n"
                print_val_map["ValVar" + str(idx) + "("+ node.get_name() +")" ] = valstr
                val_key_map[idx] = "ValVar" + str(idx) + "("+ node.get_name() +")"
            else:
                if self.isVirtualMemAddress(e.as_long()):
                    stored_value = self.getEvalExpr(self.loadValue(e))
                    if z3.is_int_value(stored_value) == False:
                        continue
                    if isinstance(stored_value, z3.ExprRef):
                        if self.isVirtualMemAddress(stored_value.as_long()):
                            valstr = "\t Value: " + hex(stored_value.as_long()) + "\n"
                        else:
                            valstr = "\t Value: " + str(stored_value.as_long()) + "\n"
                    else:
                        valstr = "\t Value: NULL" + "\n"
                else:
                    valstr = "\t Value: NULL" + "\n"
                print_val_map["ObjVar" + str(idx) + " (0x" + hex(idx) + ") "] = valstr
                obj_key_map[idx] = "ObjVar" + str(idx) + " (0x" + hex(idx) + ") "
        print("\n-----------SVFVar and Value-----------")
        for idx, key in obj_key_map.items():
            val = print_val_map[key].strip()
            label = f"ObjVar{idx} (0x{idx:x})"
            print(f"{label:<30} {val}")

        for idx, key in val_key_map.items():
            val = print_val_map[key].strip()
            #label = f"ValVar{idx}"
            label = key
            print(f"{label:<30} {val}")
        print("-----------------------------------------")

    def addToSolver(self, expr: z3.ExprRef) -> None:
        assert isinstance(expr, z3.ExprRef), "expr is not a valid z3 expression, the type of expr is {}".format(type(expr))
        self.solver.add(expr)

    def resetSolver(self) -> None:
        self.solver.reset()
        self.var_id_to_expr_map = {}
        self.current_expr_idx = 0
        self.loc_to_val_map = z3.Const('loc2ValMap', self.loc_to_val_map.sort())

class SSE:
    def __init__(self, svfir: pysvf.SVFIR) -> None:
        assert isinstance(svfir, pysvf.SVFIR), "svfir is not a valid SVFIR object, the type of svfir is {}".format(type(svfir))
        self.svfir = svfir
        self.icfg = self.svfir.getICFG()
        self.z3mgr = Z3Mgr(svfir)
        self.callingCtx = []
        self.paths = []
        self.path = []
        self.visited = set()

    def getZ3Mgr(self) -> Z3Mgr:
        return self.z3mgr

    def identifySource(self) -> list:
        return [self.icfg.getGlobalICFGNode()]

    def identifySink(self) -> list:
        res = []
        cs = self.svfir.getCallSites()
        for c in cs:
            func_name = c.getCalledFunction().getName()
            if func_name == "assert" or func_name == "svf_assert" or func_name == "sink":
                res.append(c)
        return res

    def isAssertFunc(self, func_name: str) -> bool:
        assert isinstance(func_name, str), "func_name is not a valid string, the type of func_name is {}".format(type(func_name))
        return func_name == "assert" or func_name == "svf_assert" or func_name == "sink"

    def resetSolver(self) -> None:
        self.z3mgr.resetSolver()
        self.callingCtx = []



    def translatePath(self, path: list) -> bool:
        assert isinstance(path, list), "path is not a valid list, the type of path is {}".format(type(path))
        for edge in path:
            if edge.isIntraCFGEdge():
                if not self.handleIntra(edge):
                    return False
            elif edge.isCallCFGEdge():
                self.handleCall(edge)
            elif edge.isRetCFGEdge():
                self.handleRet(edge)
            else:
                assert False, "what other edges we have?"
        return True


    def assertChecking(self, inode: pysvf.ICFGNode) -> bool:
        global assert_checked
        assert isinstance(inode, pysvf.ICFGNode), "inode is not a valid ICFGNode object, the type of inode is {}".format(type(inode))
        assert_checked = True
        callnode = inode
        assert callnode and self.isAssertFunc(callnode.getCalledFunction().getName()) and "last node is not an assert call?"
        print(f"## Analyzing {callnode}")
        arg0 = self.z3mgr.getZ3Expr(callnode.getActualParms()[0].getId(), self.callingCtx)
        self.z3mgr.solver.push()
        self.z3mgr.addToSolver(arg0 == self.z3mgr.getZ3Val(0))

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

    def pushCallingCtx(self, c: pysvf.ICFGNode) -> None:
        assert isinstance(c, pysvf.ICFGNode), "c is not a valid ICFGNode object, the type of c is {}".format(type(c))
        self.callingCtx.append(c)

    def popCallingCtx(self) -> None:
        self.callingCtx.pop()



    def analyse(self) -> None:
        global assert_checked
        for src in self.identifySource():
            assert isinstance(src, pysvf.GlobalICFGNode) and "reachability should start with GlobalICFGNode!"
            for sink in self.identifySink():
                self.reachability(pysvf.IntraCFGEdge(None, src), sink)
                self.resetSolver()
        assert assert_checked, "assertion checking is not done!"

