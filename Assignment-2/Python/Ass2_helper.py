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
        self.max_num_of_expr = svfir.get_pag_node_num()*10
        self.current_expr_idx = 0

        self.addressMask = 0x7f000000
        self.flippedAddressMask = (self.addressMask^0xffffffff)

        index_sort = z3.IntSort(self.ctx)
        element_sort = z3.IntSort(self.ctx)
        array_sort = z3.ArraySort(index_sort, element_sort)
        self.loc_to_val_map = z3.Const('loc2ValMap', array_sort)

    def get_internal_id(self, addr: int) -> int:
        assert isinstance(addr, int), "addr is not a valid integer, the type of addr is {}".format(type(addr))
        return addr & self.flippedAddressMask

    def create_expr_for_obj_var(self, obj_var: pysvf.ObjVar) -> z3.ExprRef:
        assert isinstance(obj_var, pysvf.ObjVar), "obj_var is not a valid ObjVar object, the type of obj_var is {}".format(type(obj_var))
        base_obj_var = self.pag.get_base_object(obj_var.get_id())
        #print("Create Obj",str(base_obj_var))
        if base_obj_var.is_const_data_or_agg_data() or base_obj_var.is_constant_array() or base_obj_var.is_constant_struct():
            if base_obj_var.is_const_int_obj_var():
                obj = base_obj_var.as_const_int_obj_var()
                return z3.IntVal(obj.get_sext_value(), self.ctx)
            elif base_obj_var.as_const_data().is_const_fp_obj_var():
                obj = base_obj_var.as_const_data().as_const_fp_obj_var()
                return z3.IntVal(obj.get_fp_value(), self.ctx)
            elif base_obj_var.is_global_obj_var():
                return z3.IntVal(self.get_virtual_mem_address(obj_var.get_id()), self.ctx)
            elif base_obj_var.is_constant_array() or base_obj_var.is_constant_struct():
                assert False, "implement this part"
            else:
                assert False, "what other types of values we have?"
        else:
            return z3.IntVal(self.get_virtual_mem_address(obj_var.get_id()), self.ctx)


    def store_value(self, loc: z3.ExprRef, value: z3.ExprRef) -> z3.ExprRef:
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
        deref = self.get_eval_expr(loc)
        assert self.is_virtual_mem_address(deref.as_long()), "Pointer operand is not a physical address"
        self.loc_to_val_map = z3.Store(self.loc_to_val_map, deref, value)
        return self.loc_to_val_map


    def load_value(self, loc: z3.ExprRef) -> z3.ExprRef:
        """
        Load a value from a memory location.

        Args:
            loc: Memory location expression

        Returns:
            Value stored at the location
        """
        assert isinstance(loc, z3.ExprRef), "loc is not a valid z3 expression, the type of loc is {}".format(type(loc))
        deref = self.get_eval_expr(loc)
        assert self.is_virtual_mem_address(deref.as_long()), "Pointer operand is not a physical address"
        return z3.Select(self.loc_to_val_map, deref)

    def get_eval_expr(self, e: z3.ExprRef) -> z3.ExprRef:
        assert isinstance(e, z3.ExprRef), "e is not a valid z3 expression, the type of e is {}".format(type(e))
        res = self.solver.check()
        assert res != z3.unsat, "unsatisfied constraints! Check your contradictory constraints added to the solver"
        model = self.solver.model()
        return model.eval(e)


    def get_z3_expr(self, idx: int, calling_ctx: list) -> z3.ExprRef:
        assert isinstance(idx, int), "idx is not a valid integer, the type of idx is {}".format(type(idx))
        assert isinstance(calling_ctx, list), "calling_ctx is not a valid list, the type of calling_ctx is {}".format(type(calling_ctx))
        assert self.get_internal_id(idx) == idx, "idx cannot be addressValue > 0x7f000000."
        svf_var = self.pag.get_gnode(idx)
        if svf_var.is_obj_var():
            return self.create_expr_for_obj_var(svf_var.as_obj_var())
        else:
            if not isinstance(svf_var, pysvf.ConstIntValVar) and not isinstance(svf_var, pysvf.ConstIntObjVar):
                pass
                #print(self.calling_ctx_to_str(calling_ctx))
            else:
                pass
            name = "ValVar" + str(idx)
            return z3.Int(name, self.ctx)


    def calling_ctx_to_str(self, calling_ctx: list) -> str:
        assert isinstance(calling_ctx, list), "calling_ctx is not a valid list, the type of calling_ctx is {}".format(type(calling_ctx))
        rawstr = ""
        rawstr += "ctx:[ "
        for node in calling_ctx:
            rawstr += str(node.get_id()) + " "
        rawstr += "] "
        return rawstr


    def update_z3_expr(self, idx: int, target: z3.ExprRef) -> None:
        assert isinstance(idx, int), "idx is not a valid integer, the type of idx is {}".format(type(idx))
        assert isinstance(target, z3.ExprRef), "target is not a valid z3 expression, the type of target is {}".format(type(target))
        if self.max_num_of_expr < idx + 1:
            raise IndexError("idx out of bound for map access, increase map size!")
        self.var_id_to_expr_map[idx] = target

    def get_z3_val(self, val:int) -> z3.ExprRef:
        assert isinstance(val, int), "val is not a valid integer, the type of val is {}".format(type(val))
        return z3.IntVal(val, self.ctx)

    def is_virtual_mem_address(self, val: int) -> bool:
        assert isinstance(val, int), "val is not a valid integer, the type of val is {}".format(type(val))
        return val > 0 and (val & self.addressMask) == self.addressMask

    def get_virtual_mem_address(self, idx: int) -> int:
        assert isinstance(idx, int), "idx is not a valid integer, the type of idx is {}".format(type(idx))
        return self.addressMask + idx

    def get_memobj_address(self, addr: int) -> z3.ExprRef:
        assert isinstance(addr, int), "addr is not a valid integer, the type of addr is {}".format(type(addr))
        obj_idx = self.get_internal_id(addr)
        assert(self.pag.get_gnode(obj_idx).is_obj_var()), "Invalid memory object index"
        return self.create_expr_for_obj_var(self.pag.get_gnode(obj_idx).as_obj_var())

    def z3_expr_to_num_value(self, expr: z3.ExprRef) -> int:
        assert isinstance(expr, z3.ExprRef), "expr is not a valid z3 expression, the type of expr is {}".format(type(expr))
        val = z3.simplify(self.get_eval_expr(expr))
        if isinstance(val, z3.IntNumRef):
            return val.as_long()
        elif val.is_numeral():
            return val.as_long()
        else:
            assert False, "this expression is not numeral"
            sys.exit(1)

    def get_gepobj_address(self, base_expr: z3.ExprRef, offset: int) -> z3.ExprRef:
        assert isinstance(base_expr, z3.ExprRef), "base_expr is not a valid z3 expression, the type of base_expr is {}".format(type(base_expr))
        assert isinstance(offset, int), "offset is not a valid integer, the type of offset is {}".format(type(offset))

        obj = self.get_internal_id(self.z3_expr_to_num_value(base_expr))
        assert(self.pag.get_gnode(obj).is_obj_var()), "Fail to get the base object address!"
        gep_obj = self.pag.get_gep_obj_var(obj, offset)
        if obj == gep_obj:
            return self.create_expr_for_obj_var(self.pag.get_gnode(obj).as_obj_var())
        else:
            return self.create_expr_for_obj_var(self.pag.get_gnode(gep_obj).as_gep_obj_var())


    def get_gep_offset(self, gep: pysvf.GepStmt, callingCtx: list) -> int:
        assert isinstance(gep, pysvf.GepStmt), "gep is not a valid GepStmt object, the type of gep is {}".format(type(gep))
        assert isinstance(callingCtx, list), "callingCtx is not a valid list, the type of callingCtx is {}".format(type(callingCtx))
        if len(gep.get_offset_var_and_gep_type_pair_vec()) == 0:
            return gep.get_constant_struct_fld_idx()
        total_offset = 0
        for i in range(len(gep.get_offset_var_and_gep_type_pair_vec()) - 1, -1, -1):
            var = gep.get_offset_var_and_gep_type_pair_vec()[i][0]
            type = gep.get_offset_var_and_gep_type_pair_vec()[i][1]
            offset = 0
            if var.is_const_int_val_var():
                offset = var.as_const_int_val_var().get_sext_value()
            else:
                offset = self.get_eval_expr(self.get_z3_expr(var.get_id(), callingCtx)).as_long()
            if type is None:
                total_offset += offset
                continue
            if type.is_pointer_type():
                total_offset += offset * self.pag.get_num_of_flatten_elements(gep.get_src_pointee_type())
            else:
                total_offset += self.pag.get_flattened_elem_idx(type, offset)
        return total_offset

    def print_expr_values(self, callingCtx: list):
        assert isinstance(callingCtx, list), "callingCtx is not a valid list, the type of callingCtx is {}".format(type(callingCtx))
        print_val_map = {}
        obj_key_map = {}
        val_key_map = {}
        for nIter in self.pag:
            idx = nIter[0]
            node = nIter[1]
            e = self.get_eval_expr(self.get_z3_expr(idx, callingCtx))
            if z3.is_int_value(e) == False:
                continue
            if isinstance(node, pysvf.ValVar):
                if self.is_virtual_mem_address(e.as_long()):
                    valstr = "\t Value: " + hex(e.as_long()) + "\n"
                else:
                    valstr = "\t Value: " + str(e.as_long()) + "\n"
                print_val_map["ValVar" + str(idx) + "("+ node.get_name() +")" ] = valstr
                val_key_map[idx] = "ValVar" + str(idx) + "("+ node.get_name() +")"
            else:
                if self.is_virtual_mem_address(e.as_long()):
                    stored_value = self.get_eval_expr(self.load_value(e))
                    if z3.is_int_value(stored_value) == False:
                        continue
                    if isinstance(stored_value, z3.ExprRef):
                        if self.is_virtual_mem_address(stored_value.as_long()):
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

    def add_to_solver(self, expr: z3.ExprRef) -> None:
        assert isinstance(expr, z3.ExprRef), "expr is not a valid z3 expression, the type of expr is {}".format(type(expr))
        self.solver.add(expr)

    def reset_solver(self) -> None:
        self.solver.reset()
        self.var_id_to_expr_map = {}
        self.current_expr_idx = 0
        self.loc_to_val_map = z3.Const('loc2ValMap', self.loc_to_val_map.sort())

class SSE:
    def __init__(self, svfir: pysvf.SVFIR) -> None:
        assert isinstance(svfir, pysvf.SVFIR), "svfir is not a valid SVFIR object, the type of svfir is {}".format(type(svfir))
        self.svfir = svfir
        self.icfg = self.svfir.get_icfg()
        self.z3mgr = Z3Mgr(svfir)
        self.calling_ctx = []
        self.paths = []
        self.path = []
        self.visited = set()

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
        global assert_checked
        assert isinstance(inode, pysvf.ICFGNode), "inode is not a valid ICFGNode object, the type of inode is {}".format(type(inode))
        assert_checked = True
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
        global assert_checked
        for src in self.identify_source():
            assert isinstance(src, pysvf.GlobalICFGNode) and "reachability should start with GlobalICFGNode!"
            for sink in self.identify_sink():
                self.reachability(pysvf.IntraCFGEdge(None, src), sink)
                self.reset_solver()
        assert assert_checked, "assertion checking is not done!"

