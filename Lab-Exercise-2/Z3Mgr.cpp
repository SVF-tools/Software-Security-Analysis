//===- Z3Mgr.cpp -- Z3 manager for software verification ------------------//
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

#include "Z3Mgr.h"
#include "SVFIR/SVFIR.h"
#include "SVF-LLVM/LLVMUtil.h"
#include <set>
#include <iomanip>

using namespace SVF;
using namespace SVFUtil;
using namespace llvm;
using namespace z3;

Z3SSEMgr::Z3SSEMgr(SVFIR *ir) : Z3Mgr(ir->getPAGNodeNum()*10), svfir(ir)
{
    initMap();
}

/*
* (1) Create Z3 expressions for top-level variables and objects, and (2) create loc2val map
*/
void Z3SSEMgr::initMap(){
    for (SVFIR::iterator nIter = svfir->begin(); nIter != svfir->end(); ++nIter)
    {
        if(const ValVar* val = SVFUtil::dyn_cast<ValVar>(nIter->second))
            createExprForValVar(val);
        else if(const ObjVar* obj = SVFUtil::dyn_cast<ObjVar>(nIter->second))
            createExprForObjVar(obj);
        else
            assert(false && "SVFVar type not supported");
    }
    DBOP(printExprValues());
}

/*
* We initialize all ValVar to be an int expression.
* ValVar of PointerTy/isFunctionTy is assigned with an ID to represent the address of an object
* ValVar of IntegerTy is assigned with an integer value
* ValVar of FloatingPointTy is assigned with an integer value casted from a float
* ValVar of StructTy/ArrayTy is assigned with their elements of the above types in the form of integer values
*/
z3::expr Z3SSEMgr::createExprForValVar(const ValVar* valVar){
    std::string str;
    raw_string_ostream rawstr(str);
    rawstr << "ValVar" << valVar->getId();
    expr e(ctx);
    if(const SVFType* type = valVar->getType()){
        e = ctx.int_const(rawstr.str().c_str());
    }
    else{
        e = ctx.int_const(rawstr.str().c_str());
        assert(SVFUtil::isa<DummyValVar>(valVar) && "not a DummValVar if it has no type?");
    }

    updateZ3Expr(valVar->getId(), e);
    return e;
}

/*
* Object must be either a constaint data or a location value (address-taken variable)
* Constant data includes ConstantInt, ConstantFP, ConstantPointerNull and ConstantAggregate
* Locations includes pointers to globals, heaps, stacks
*/
z3::expr Z3SSEMgr::createExprForObjVar(const ObjVar* objVar){
    std::string str;
    raw_string_ostream rawstr(str);
    rawstr << "ObjVar" << objVar->getId();

    expr e = ctx.int_const(rawstr.str().c_str());
    if(objVar->hasValue()){
        const MemObj* obj = objVar->getMemObj();
        /// constant data
        if (obj->isConstDataOrAggData() || obj->isConstantArray() || obj->isConstantStruct())
        {
            if (const SVFConstantInt* consInt = SVFUtil::dyn_cast<SVFConstantInt>(obj->getValue()))
            {
                e = ctx.int_val((s32_t)consInt->getSExtValue());
            }
            else if (const SVFConstantFP *consFP = SVFUtil::dyn_cast<SVFConstantFP>(obj->getValue()))
                e = ctx.int_val(static_cast<u32_t>(consFP->getFPValue()));
            else if (SVFUtil::isa<SVFConstantNullPtr>(obj->getValue()))
                e = ctx.int_val(0);
            else if (SVFUtil::isa<SVFGlobalValue>(obj->getValue()))
                e = ctx.int_val(getVirtualMemAddress(objVar->getId()));
            else if (obj->isConstantArray() || obj->isConstantStruct())
                assert(false && "implement this part");
            else
            {
                std::cerr << obj->getValue()->toString() << "\n";
                assert(false && "what other types of values we have?");
            }
        }
            /// locations (address-taken variables)
        else
        {
            e = ctx.int_val(getVirtualMemAddress(objVar->getId()));
        }
    }
    else{
        assert(SVFUtil::isa<DummyObjVar>(objVar) && "it should either be a blackhole or constant dummy if this obj has no value?");
        e = ctx.int_val(getVirtualMemAddress(objVar->getId()));
    }

    updateZ3Expr(objVar->getId(), e);
    return e;
}

/// Store and Select for Loc2ValMap, i.e., store and load
z3::expr Z3Mgr::storeValue(const z3::expr loc, const z3::expr value){
    z3::expr deref = getEvalExpr(loc);
    assert(isVirtualMemAddress(deref) && "Pointer operand is not a physical address?");
    z3::expr loc2ValMap = varID2ExprMap[lastSlot];
    loc2ValMap = z3::store(loc2ValMap , deref, value);
    varID2ExprMap.set(lastSlot, loc2ValMap);
    return loc2ValMap;
}

z3::expr Z3Mgr::loadValue(const z3::expr loc){
    z3::expr deref = getEvalExpr(loc);
    assert(isVirtualMemAddress(deref) && "Pointer operand is not a physical address?");
    z3::expr loc2ValMap = varID2ExprMap[lastSlot];
    return z3::select(loc2ValMap , deref);
}


/// Return int value from an expression if it is a numeral, otherwise return an approximate value
s32_t Z3Mgr::z3Expr2NumValue(z3::expr e) {
    z3::expr val = getEvalExpr(e);
    if(val.is_numeral())
        return val.get_numeral_int64();
    else{
        assert(false && "this expression is not numeral");
        abort();
    }
}

/// Return int value from an expression if it is a numeral, otherwise return an approximate value
z3::expr Z3Mgr::getEvalExpr(z3::expr e) {
    z3::check_result res = solver.check();
    assert(res!=z3::unsat && "unsatisfied constraints! Check your contradictory constraints added to the solver");
    z3::model m = solver.get_model();
    return m.eval(e);
}

void Z3Mgr::printExprValues(){
    std::cout.flags(std::ios::left);
    std::cout << "-----------Var and Value-----------\n";
    for (u32_t i = 0; i < lastSlot; i++)
    {
        expr e = getEvalExpr(varID2ExprMap[i]);
        if(e.is_numeral()){
            s32_t value = e.get_numeral_int64();
            std::stringstream exprName;
            exprName << "Var" << i;
            std::cout << std::setw(25)  <<  exprName.str();
            if(isVirtualMemAddress(value))
                std::cout << "\t Value: "<< std::hex << "0x" << value << "\n";
            else
                std::cout << "\t Value: " << std::dec << value << "\n";
        }
    }
    std::cout << "-----------------------------------------\n";
}

/// Return the address expr of a ObjVar
z3::expr Z3SSEMgr::getMemObjAddress(u32_t idx) const{
    NodeID objIdx = getInternalID(idx);
    assert(SVFUtil::isa<ObjVar>(svfir->getGNode(objIdx)) && "Fail to get the MemObj!");
    return getZ3Expr(objIdx);
}


z3::expr Z3SSEMgr::getGepObjAddress(z3::expr pointer, u32_t offset){
    NodeID baseObj = getInternalID(z3Expr2NumValue(pointer));
    assert(SVFUtil::isa<ObjVar>(svfir->getGNode(baseObj)) && "Fail to get the base object address!");
    NodeID gepObj = svfir->getGepObjVar(baseObj, offset);
    /// TODO: check whether this node has been created before or not to save creation time
    if(baseObj==gepObj)
        return getZ3Expr(baseObj);
    else
        return createExprForObjVar(SVFUtil::cast<GepObjVar>(svfir->getGNode(gepObj)));
}

s32_t Z3SSEMgr::getGepOffset(const GepStmt* gep){
    if(gep->getOffsetVarAndGepTypePairVec().empty())
        return gep->getConstantStructFldIdx();

    s32_t totalOffset = 0;
    for(int i = gep->getOffsetVarAndGepTypePairVec().size() - 1; i >= 0; i--){
        const SVFValue* value = gep->getOffsetVarAndGepTypePairVec()[i].first->getValue();
        const SVFType* type = gep->getOffsetVarAndGepTypePairVec()[i].second;
        const SVFConstantInt *op = SVFUtil::dyn_cast<SVFConstantInt>(value);
        s32_t offset = 0;
        /// constant as the offset
        if(op)
            offset = op->getSExtValue();
            /// variable as the offset
        else
            offset = z3Expr2NumValue(getZ3Expr(svfir->getValueNode(value)));

        if(type==nullptr){
            totalOffset += offset;
            continue;
        }

        /// Caculate the offset
        if(const SVFPointerType* pty = SVFUtil::dyn_cast<SVFPointerType>(type))
            totalOffset += offset * gep->getAccessPath().getElementNum(gep->getAccessPath().gepSrcPointeeType());
        else
            totalOffset +=  SymbolTableInfo::SymbolInfo()->getFlattenedElemIdx(type, offset);
    }
    return totalOffset;
}

void Z3SSEMgr::printExprValues(){
    std::cout.flags(std::ios::left);
    std::cout << "-----------SVFVar and Value-----------\n";
    std::map<std::string, std::string> printValMap;
    std::map<NodeID, std::string> objKeyMap;
    std::map<NodeID, std::string> valKeyMap;
    for (SVFIR::iterator nIter = svfir->begin(); nIter != svfir->end(); ++nIter)
    {
        expr e = getEvalExpr(getZ3Expr(nIter->first));
        if(e.is_numeral()){
            NodeID varID = nIter->second->getId();
            s32_t value = e.get_numeral_int64();
            std::stringstream exprName;
            std::stringstream valstr;
            if(const ValVar* valVar = SVFUtil::dyn_cast<ValVar>(nIter->second)){
                exprName << "ValVar" << varID;
                if(isVirtualMemAddress(value))
                    valstr << "\t Value: "<< std::hex << "0x" << value << "\n";
                else
                    valstr << "\t Value: " << std::dec << value << "\n";
                printValMap[exprName.str()] = valstr.str();
                valKeyMap[varID] = exprName.str();
            }
            else{
                exprName << "ObjVar" << varID << std::hex << " (0x" << getVirtualMemAddress(varID) << ") ";
                if(isVirtualMemAddress(value)){
                    expr storedValue = getEvalExpr(loadValue(e));
                    if(storedValue.is_numeral()){
                        s32_t contentValue = z3Expr2NumValue(storedValue);
                        if(isVirtualMemAddress(contentValue))
                            valstr << "\t Value: " << std::hex << "0x" << contentValue << "\n";
                        else
                            valstr << "\t Value: " << std::dec << contentValue << "\n";
                    }
                    else
                        valstr << "\t Value: NULL" << "\n";
                }
                else
                    valstr << "\t Value: NULL" << "\n";
                printValMap[exprName.str()] = valstr.str();
                objKeyMap[varID] = exprName.str();
            }
        }
    }
    for (auto it = objKeyMap.begin(); it != objKeyMap.end(); ++it) {
        std::string printKey = it->second;
        std::cout << std::setw(25)  <<  printKey << printValMap[printKey];
    }
    for (auto it = valKeyMap.begin(); it != valKeyMap.end(); ++it) {
        std::string printKey = it->second;
        std::cout << std::setw(25)  <<  printKey << printValMap[printKey];
    }
    std::cout << "-----------------------------------------\n";
}