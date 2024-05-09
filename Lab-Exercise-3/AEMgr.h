//===- AE.cpp -- Abstract Execution---------------------------------//
//
//                     SVF: Static Value-Flow Analysis
//
// Copyright (C) <2013->  <Yulei Sui>
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
// The implementation is based on
// Xiao Cheng, Jiawei Wang and Yulei Sui. Precise Sparse Abstract Execution via Cross-Domain Interaction.
// 46th International Conference on Software Engineering. (ICSE24)
//===----------------------------------------------------------------------===//


//
// Created by Jiawei Wang on 2024/1/10.
//

#include "Util/SVFBugReport.h"
#include "AE/Core/ICFGWTO.h"
#include "WPA/Andersen.h"
#include "AE/Svfexe/SVFIR2AbsState.h"


namespace SVF
{
    class IntervalExeState;
    class IntervalValue;
    class ExeState;
    class SVFIR2ItvExeState;

    class AbstractExecutionMgr {
    public:
        AbstractExecutionMgr() = default;
        ~AbstractExecutionMgr() = default;
        void test1();
        void test2();
        void test3();
        void test4();
        void test5();
        void test6();
        void test7();
        void test8();

        void reset() {
            _strToID.clear();
            as.clear();
        };

        NodeID getInternalID(NodeID addr)
        {
            return AddressValue::getInternalID(addr);
        }

        NodeID getNodeID(std::string name) {
            auto it = _strToID.find(name);
            if(it!=_strToID.end())
                return it->second;
            else{
                _strToID[name] = ++currentExprIdx;
                return currentExprIdx;
            }
        }

        void removeNodeID(std::string name) {
            auto it = _strToID.find(name);
            if (it != _strToID.end())
                _strToID.erase(it);
            else
                assert(false && "removeNodeID: name not found");
        }

        NodeID getNodeID(std::string name, u32_t size) {
            auto it = _strToID.find(name);
            if(it!=_strToID.end())
                return it->second;
            else{
                _strToID[name] = ++currentExprIdx;
                currentExprIdx += (size - 1);
                return _strToID[name];
            }
        }

        u32_t getVirtualAddr(NodeID id) const {
            return AddressMask + id;
        }

        u32_t getMemObjAddress(std::string name) {
            auto it = _strToID.find(name);
            if (it == _strToID.end())
            {
                assert(false && "");
                abort();
            }
            else {
                u32_t res = _strToID[name];
                return AddressMask + res;
            }
        }

        NodeID getGepObjAddress(std::string arr_name, u32_t offset) {
            auto iter = _strToID.find(arr_name);
            assert(iter != _strToID.end() && "Gep BaseObject expr not found?");
            u32_t baseObjID = iter->second;
            u32_t gepObj = baseObjID + offset;
            if (baseObjID == gepObj){
                return  AddressMask + baseObjID;
            }
            else {
                return  AddressMask + gepObj;
            }
        }

        void getExitState(AbstractState &es, NodeID x);

        void svf_assert(AbstractValue absv) {
            IntervalValue iv = absv.getInterval();
            if (iv.is_numeral()) {
                if (iv.getNumeral() == 0) {
                    SVFUtil::outs() << SVFUtil::errMsg("\t FAILURE :")  << "assertion failed\n";
                    assert(false && "assertion failed");
                    abort();
                }
                else {
                    SVFUtil::outs() << SVFUtil::sucMsg("\t SUCCESS :")  << "assertion passed\n";
                    return;
                }
            } else {
                SVFUtil::outs() << SVFUtil::errMsg("\t FAILURE :")  << "assertion failed\n";
                assert(false && "assertion failed");
                abort();
            }
        }
        static u32_t currentExprIdx;

    private:
        AbstractState as;
        Map<std::string, NodeID> _strToID;
    };
}
