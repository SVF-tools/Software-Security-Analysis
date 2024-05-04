//===- Z3Mgr.h -- Z3 manager for software verification ------------------//
//
//                     SVF: Static Value-Flow Analysis
//
// Copyright (C) <2013-2022>  <Yulei Sui>
//

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Affero General Public License for more details.

// You should have received a copy of the GNU Affero General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//===----------------------------------------------------------------------===//
/*
 * Z3 manager for software verification
 *
 * Created on: Feb 19, 2024
 */

#ifndef SOFTWARE_SECURITY_ANALYSIS_Z3MGR_H
#define SOFTWARE_SECURITY_ANALYSIS_Z3MGR_H

#include "z3++.h"


namespace SVF {

#ifndef DEBUGINFO
#define DBOP(X) X;
#else
#define DBOP(X)
#endif

#define AddressMask 0x7f000000
#define FlippedAddressMask (AddressMask^0xffffffff)

class SVFIR;

class ValVar;

class ObjVar;

class GepStmt;

typedef unsigned u32_t;
typedef signed s32_t;

/// Z3 manager interface
class Z3Mgr {

public:
    /// Constructor
    Z3Mgr(u32_t numOfMapElems) : solver(ctx), varID2ExprMap(ctx), lastSlot(numOfMapElems) {
        resetZ3ExprMap();
    }

    inline void resetZ3ExprMap() {
        varID2ExprMap.resize(lastSlot + 1);
        z3::expr loc2ValMap = ctx.constant("loc2ValMap", ctx.array_sort(ctx.int_sort(), ctx.int_sort()));
        updateZ3Expr(lastSlot, loc2ValMap);
    }

    /// Store and Select for Loc2ValMap, i.e., store and load
    z3::expr storeValue(const z3::expr loc, const z3::expr value);

    z3::expr loadValue(const z3::expr loc);

    /// The physical address starts with 0x7f...... + idx
    inline u32_t getVirtualMemAddress(u32_t idx) const {
        return AddressMask + idx;
    }

    inline bool isVirtualMemAddress(u32_t val) {
        return (val > 0 && (val & AddressMask) == AddressMask);
        //return ((val & AddressMask) > 0);
    }

    inline bool isVirtualMemAddress(z3::expr e) {
        return isVirtualMemAddress(z3Expr2NumValue(e));
    }

    /// Return the internal index if idx is an address otherwise return the value of idx
    inline u32_t getInternalID(u32_t idx) const {
        return (idx & FlippedAddressMask);
    }

    /// Return Z3 expression based on SVFVar ID
    inline z3::expr getZ3Expr(u32_t idx) const {
        assert(getInternalID(idx) == idx && "SVFVar idx overflow > 0x7f000000?");
        assert(varID2ExprMap.size() >= idx + 1 && "idx out of bound for map access, increase map size!");
        return varID2ExprMap[getInternalID(idx)];
    }

    /// Update expression when assignments
    inline void updateZ3Expr(u32_t idx, z3::expr target) {
        assert(varID2ExprMap.size() >= idx + 1 && "idx out of bound for map access, increase map size!");
        varID2ExprMap.set(getInternalID(idx), target);
    }

    /// Return int value from an expression if it is a numeral, otherwise return an approximate value
    s32_t z3Expr2NumValue(z3::expr e);

    /// Return int value from an expression if it is a numeral, otherwise return an approximate value
    z3::expr getEvalExpr(z3::expr e);

    /// Print values of all expressions
    void printExprValues();

    inline z3::solver &getSolver() {
        return solver;
    }

    inline z3::context &getCtx() {
        return ctx;
    }

    inline void clearVarID2ExprMap() {
        while (!varID2ExprMap.empty())
            varID2ExprMap.pop_back();

        resetZ3ExprMap();
    }

protected:
    z3::context ctx;
    z3::solver solver;

private:
    z3::expr_vector varID2ExprMap;
    u32_t lastSlot;
};


class Z3SSEMgr : public Z3Mgr {

public:
    /// Constructor
    Z3SSEMgr(SVFIR *ir);

    /// Initialize map (varID2ExprMap: ID->expr)from VARID to z3 expr                                            --- Using elements from 0 to lastSlot
    /// Initialize map (loc2ValMap: ID->ID) from Location (pointer address) to Value    --- Using the last slot
    /// V = L U C    (V is SVFVar, L is Pointers + Nonconst Objects, C is Constants )
    /// loc2ValMap : IDX(L) -> IDX(V)
    /// idx \in IDX(V) (IDX is a set of Indices of all SVFVars)
    void initMap();

    /// Declare the expr type for each top-level pointers
    z3::expr createExprForValVar(const ValVar *val);

    /// Initialize the expr value for each objects (address-taken variables and constants)
    z3::expr createExprForObjVar(const ObjVar *obj);

    /// Return the address expr of a ObjVar
    z3::expr getMemObjAddress(u32_t idx) const;

    /// Return the field address given a pointer points to a struct object and an offset
    z3::expr getGepObjAddress(z3::expr pointer, u32_t offset);

    /// Return the offset expression of a GepStmt
    s32_t getGepOffset(const GepStmt *gep);

    /// Dump values of all exprs
    virtual void printExprValues();

private:
    SVFIR *svfir;
};


}


#endif //SOFTWARE_SECURITY_ANALYSIS_Z3MGR_H
