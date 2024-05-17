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
	if (test_name == "test1") {
		SVFUtil::outs() << "Test 1: \n";
		mgr.test1();
		mgr.reset();
	}
	else if (test_name == "test2") {
		SVFUtil::outs() << "Test 2: \n";
		mgr.test2();
		mgr.reset();
	}
	else if (test_name == "test3") {
		SVFUtil::outs() << "Test 3: \n";
		mgr.test3();
		mgr.reset();
	}
	else if (test_name == "test4") {
		SVFUtil::outs() << "Test 4: \n";
		mgr.test4();
		mgr.reset();
	}
	else if (test_name == "test5") {
		SVFUtil::outs() << "Test 5: \n";
		mgr.test5();
		mgr.reset();
	}
	else if (test_name == "test6") {
		SVFUtil::outs() << "Test 6: \n";
		mgr.test6();
		mgr.reset();
	}
	else if (test_name == "test7") {
		SVFUtil::outs() << "Test 7: \n";
		mgr.test7();
		mgr.reset();
	}
	else if (test_name == "test8") {
		SVFUtil::outs() << "Test 8: \n";
		mgr.test8();
		mgr.reset();
	}
	else {
		std::cerr << "Invalid test name" << std::endl;
		return 1;
	}
	return 0;
}