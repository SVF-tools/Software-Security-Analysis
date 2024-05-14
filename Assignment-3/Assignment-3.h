//===- Assignment-3.h -- Abstract Interpretation --//
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
 * Abstract Interpretation and buffer overflow detection
 *
 * Created on: Feb 19, 2024
 */

#ifndef SOFTWARE_SECURITY_ANALYSIS_ASSIGNMENT_3_H
#define SOFTWARE_SECURITY_ANALYSIS_ASSIGNMENT_3_H

#include "Util/SVFBugReport.h"
#include "AE/Core/ICFGWTO.h"
#include "WPA/Andersen.h"
#include "AE/Svfexe/AbstractInterpretation.h"

namespace SVF
{
class AbsExe;

class Assign3Exception : public std::exception {
public:
    Assign3Exception(const std::string& message): msg_(message) {}

    virtual const char* what() const throw() {
        return msg_.c_str();
    }

private:
    std::string msg_;
};



/// AbstractInterpretation is same as Abstract Execution
class AbsExe: virtual public AbstractInterpretation
{
public:
    /// Constructor
    AbsExe() : AbstractInterpretation() {
    }

    virtual void runOnModule(ICFG* icfg) {
        AbstractInterpretation::runOnModule(icfg);
    }

    void handleWTONode(const ICFGNode* node);
    virtual void handleCycle(const ICFGWTOCycle* cycle);

    /// Program entry
    virtual void analyse();

    virtual void updateAbsState(const SVFStmt* stmt);
    virtual void bufOverflowDetection(const SVFStmt* stmt);


    // handle SVF Statements
    void updateStateOnAddr(const AddrStmt* addr);
    void updateStateOnGep(const GepStmt* gep);
    void updateStateOnStore(const StoreStmt* store);
    void updateStateOnLoad(const LoadStmt* load);
    void updateStateOnCmp(const CmpStmt* cmp);
    void updateStateOnCall(const CallPE* call);
    void updateStateOnRet(const RetPE* retPE);
    void updateStateOnCopy(const CopyStmt* copy);
    void updateStateOnPhi(const PhiStmt* phi);
    void updateStateOnBinary(const BinaryOPStmt* binary);
    void updateStateOnSelect(const SelectStmt *select);

    void addBugToReporter(const Assign3Exception& e, const ICFGNode* node);

    // helper functions related to gep
    void initSVFVar(AbstractState& es, u32_t varId);
    void initObjVar(AbstractState& es, const ObjVar *objVar, u32_t varId);
    AddressValue getGepObjAddress(AbstractState& es, u32_t pointer, APOffset offset);
    IntervalValue getByteOffset(const AbstractState& es, const GepStmt *gep);
    IntervalValue getElementIndex(const AbstractState& es, const GepStmt *gep);

    /// Destructor
    virtual ~AbsExe() {
    }
protected:
    //Record the byte size of an object
    Map<NodeID, AbstractValue> obj2size;
};
}

#endif