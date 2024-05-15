//===- test.cpp -- Abstract Execution -------------------------------------//
//
//                     SVF: Static Value-Flow Analysis
//
// Copyright (C) <2013-2017>  <Yulei Sui>
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
//===-----------------------------------------------------------------------===//

/*
 // Abstract Execution tests
 //
 */
#include "SVF-LLVM/SVFIRBuilder.h"
#include "WPA/WPAPass.h"
#include "Util/CommandLine.h"
#include "Util/Options.h"

#include "AEMgr.h"

using namespace SVF;
using namespace SVFUtil;

u32_t AbstractExecutionMgr::passed_num = 0;
u32_t AbstractExecutionMgr::total_num = 0;

int main(int argc, char** argv)
{
    AbstractExecutionMgr mgr;
    SVFUtil::outs() << "Test 1: \n";
    mgr.test1();
    mgr.reset();
    SVFUtil::outs() << "Test 2: \n";
    mgr.test2();
    mgr.reset();
    SVFUtil::outs() << "Test 3: \n";
    mgr.test3();
    mgr.reset();
    SVFUtil::outs() << "Test 4: \n";
    mgr.test4();
    mgr.reset();
    SVFUtil::outs() << "Test 5: \n";
    mgr.test5();
    mgr.reset();
    SVFUtil::outs() << "Test 6: \n";
    mgr.test6();
    mgr.reset();
    SVFUtil::outs() << "Test 7: \n";
    mgr.test7();
    mgr.reset();
    SVFUtil::outs() << "Test 8: \n";
    mgr.test8();
    mgr.reset();
    std::cout << "Passed " << AbstractExecutionMgr::passed_num << " out of " << AbstractExecutionMgr::total_num << " tests" << std::endl;
    return AbstractExecutionMgr::passed_num == AbstractExecutionMgr::total_num? 0 : 1;
}