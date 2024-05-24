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

#include "AE/Core/ICFGWTO.h"
#include "AE/Core/AbstractState.h"
#include "Util/SVFBugReport.h"
#include "WPA/Andersen.h"

namespace SVF {
	class AbstractExe;

    /// Exception handling for bug detections
    class AEException : public std::exception {
    public:
        AEException(const std::string& message)
                : msg_(message) {}

        virtual const char* what() const throw() {
            return msg_.c_str();
        }

    private:
        std::string msg_;
    };

	/// Abstract Execution class
    class AbstractExe {
    public:
        /// Constructor
        AbstractExe() {}

        virtual void runOnModule(ICFG* icfg);

        /// Handle a Weak Topological Order (WTO) node in the control flow graph
        void handleSingletonWTO(const ICFGNode* node);
        /// Handle a WTO which involves control-flow cycle(s)
        void handleCycleWTO(const ICFGWTOCycle* cycle);
        /// Handle a function in the control flow graph
        void handleFunc(const SVFFunction *func);

        /// Handle global variables and initializations
        void handleGlobalNode();

        /// Driver of the program
        virtual void analyse();

        /// Handle state updates for each type of SVF statement
        virtual void updateAbsState(const SVFStmt* stmt);

        /// Fuction used to implement buffer overflow detection
        virtual void bufOverflowDetection(const SVFStmt* stmt);

        // handle SVF Statements
        ///@{
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
        void updateStateOnSelect(const SelectStmt* select);
        ///@}

        /// Add a detected bug to the bug reporter and print the report
        ///@{
        void addBugToReporter(const AEException& e, const ICFGNode* node);
        void printReport();
        inline const Map<const ICFGNode*, std::string>& getBugInfo() const {
            return _nodeToBugInfo;
        }
        ///@}

        /// Initialize an object variable in the abstract state
        void initObjVar(AbstractState& as, ObjVar* objVar);

        /// Get the address value for a GEP (GetElementPtr) object
        AddressValue getGepObjAddress(AbstractState& es, u32_t pointer, APOffset offset);
        /// Get the byte offset interval for a given abstract state and GEP statement
        IntervalValue getByteOffset(const AbstractState& es, const GepStmt* gep);
        /// Get the interval value of an element index for a given abstract state and GEP statement
        IntervalValue getElementIndex(const AbstractState& es, const GepStmt* gep);

        /// Handle stub functions for verifying abstract interpretation results
        void handleStubFunctions(const CallICFGNode* call);

        /// Mark recursive functions in the call graph
        void initWTO();

        /// Path feasiblity handling
        ///@{
        bool propagateStateIfFeasible(const ICFGNode* curNode);
        bool isCmpBranchFeasible(const CmpStmt* cmpStmt, s64_t succ,
                                 AbstractState& as);
        bool isSwitchBranchFeasible(const SVFVar* var, s64_t succ,
                                    AbstractState& as);
        bool isBranchFeasible(const IntraCFGEdge* intraEdge,
                              AbstractState& as);
        ///@}

        /// Handle a call site in the control flow graph
        void handleCallSite(const CallICFGNode* callnode);

        /// Get the byte size of an allocation instruction
        u32_t getAllocaInstByteSize(AbstractState& as, const AddrStmt *addr);

        /// Return its abstract state given an ICFGNode
        AbstractState& getAbsState(const ICFGNode* node)
        {
            const ICFGNode* repNode = _icfg->getRepNode(node);
            if (_postAbsTrace.count(repNode) == 0)
                assert(0 && "No preAbsTrace for this node");
            else
                return _postAbsTrace[repNode];
        }

        /// Destructor
        virtual ~AbstractExe() {}

    protected:
        // Map an object to its byte size
        Map<NodeID, IntervalValue> obj2size;

        /// SVFIR and ICFG
        SVFIR* _svfir;
        ICFG* _icfg;
        AndersenWaveDiff* _ander;

        /// Bug reporter 
        Set<std::string> _bugLoc;
        SVFBugReport _recoder;
        Map<const ICFGNode*, std::string> _nodeToBugInfo;

        /// callstack
        std::vector<const CallICFGNode*> _callSiteStack;
        /// Map a function to its corresponding WTO
        Map<const SVFFunction*, ICFGWTO*> _funcToWTO;
        /// A set of functions which are involved in recursions
        Set<const SVFFunction*> _recursiveFuns;
        /// Abstract trace immediately before an ICFGNode.
        Map<const ICFGNode*, AbstractState> _preAbsTrace;
        /// Abstract trace immediately after an ICFGNode.
        Map<const ICFGNode*, AbstractState> _postAbsTrace;
    };
} // namespace SVF

#endif