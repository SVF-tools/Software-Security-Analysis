//===- Z3Mgr.h -- Z3 manager for symbolic execution ------------------//
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
 * Z3 manager for symbolic execution
 *
 * Created on: Feb 19, 2024
 */

#ifndef SOFTWARE_SECURITY_ANALYSIS_Z3SSEMGR_H
#define SOFTWARE_SECURITY_ANALYSIS_Z3SSEMGR_H

#include "Z3Mgr.h"

namespace SVF {

class SVFIR;
class ValVar;
class ObjVar;
class GepStmt;

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
