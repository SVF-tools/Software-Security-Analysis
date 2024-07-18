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
#include "Util/CommandLine.h"
#include "Util/Options.h"
#include "WPA/WPAPass.h"

#include "AEMgr.h"

using namespace SVF;
using namespace SVFUtil;

int main(int argc, char** argv) {
	if (argc != 2) {
		std::cerr << "Usage: ./lab3 test1" << std::endl;
		return 1;
	}

	AbstractExecutionMgr mgr;

	std::string test_name = argv[1];
	if (test_name == "test0") {
		SVFUtil::outs() << "Test 0: \n";
		AEState as = mgr.test0();
		// assert(x==5);
		// svf_assert(as[x].getInterval() == IntervalValue(5,5));
		mgr.svf_assert(as[mgr.getNodeID("x")].getInterval() == IntervalValue(5, 5));
		mgr.reset();
	}
	else if (test_name == "test1") {
		SVFUtil::outs() << "Test 1: \n";
		AEState as = mgr.test1();
		// assert(b>0);
		// svf_assert(as[b].getInterval() > IntervalValue(0,0));
		mgr.svf_assert(as[mgr.getNodeID("b")].getInterval() > IntervalValue(0, 0));
		mgr.reset();
	}
	else if (test_name == "test2") {
		SVFUtil::outs() << "Test 2: \n";
		AEState as = mgr.test2();
		// assert(b>3);
		// svf_assert(as[b].getInterval() > IntervalValue(3,3));
		mgr.svf_assert(as[mgr.getNodeID("b")].getInterval() > IntervalValue(3, 3));
		mgr.reset();
	}
	else if (test_name == "test3") {
		SVFUtil::outs() << "Test 3: \n";
		AEState as = mgr.test3();
		// assert(x==10);
		// svf_assert(as[x].getInterval() == IntervalValue(10, 10));
		mgr.svf_assert(as[mgr.getNodeID("x")].getInterval() == IntervalValue(10, 10));
		mgr.reset();
	}
	else if (test_name == "test4") {
		SVFUtil::outs() << "Test 4: \n";
		AEState as = mgr.test4();
		// assert((a + b)>20);
		// svf_assert(as[a].getInterval() + as[b].getInterval() > IntervalValue(20, 20));
		mgr.svf_assert(as[mgr.getNodeID("a")].getInterval() + as[mgr.getNodeID("b")].getInterval()
		               > IntervalValue(20, 20));
		mgr.reset();
	}
	else if (test_name == "test5") {
		SVFUtil::outs() << "Test 5: \n";
		AEState as = mgr.test5();
		// assert(z==15);
		// svf_assert(as[z].getInterval() == IntervalValue(15, 15));
		mgr.svf_assert(as[mgr.getNodeID("z")].getInterval() == IntervalValue(15, 15));
		mgr.reset();
	}
	else if (test_name == "test6") {
		SVFUtil::outs() << "Test 6: \n";
		AEState as = mgr.test6();
		// assert(b>=5);
		// svf_assert(as[b].getInterval() >= IntervalValue(5, 5));
		mgr.svf_assert(as[mgr.getNodeID("b")].getInterval() >= IntervalValue(5, 5));
		mgr.reset();
	}
	else if (test_name == "test7") {
		SVFUtil::outs() << "Test 7: \n";
		AEState as = mgr.test7();
		// assert(x== 3 && y==2);
		// AbstractValue cmp1 = as[x].getInterval() == IntervalValue(3, 3);
		// AbstractValue cmp2 = as[y].getInterval() == IntervalValue(2, 2);
		mgr.svf_assert(as[mgr.getNodeID("x")].getInterval() == IntervalValue(3, 3));
		mgr.svf_assert(as[mgr.getNodeID("y")].getInterval() == IntervalValue(2, 2));
		mgr.reset();
	}
	else if (test_name == "test8") {
		SVFUtil::outs() << "Test 8: \n";
		AEState as = mgr.test8();
		// assert(x == 0);
		// svf_assert(as[x].getInterval() == IntervalValue(0, 0));
		mgr.svf_assert(as[mgr.getNodeID("x")].getInterval() == IntervalValue(0, 0));
		mgr.reset();
	}
	else {
		std::cerr << "Invalid test name" << std::endl;
		return 1;
	}
	return 0;
}