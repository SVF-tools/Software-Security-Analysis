//===- test.cpp -- Manual assertion-based verification (Z3 Example) ------------------//
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
 * Manual symbolic execution for assertion-based verification test cases
 *
 * Created on: Feb 19, 2024
 */

#include "Util/Options.h"
#include "Z3Examples.h"

using namespace z3;
using namespace SVF;
using namespace SVFUtil;

// For assert (Q), add ¬Q to the solver to prove the absence of counterexamples; 
// otherwise return a counterexample
bool checkNegateAssert(Z3Examples* z3Mgr, z3::expr q) {
	// negative check
	z3Mgr->getSolver().push();
	z3Mgr->addToSolver(!q);
	z3Mgr->getSolver().check();
	bool res = z3Mgr->getSolver().check() == z3::unsat;
	z3Mgr->getSolver().pop();
	return res;
}
/*
 // Please set the "program": "${workspaceFolder}/bin/ass3" in file '.vscode/launch.json'
 // To run your testcase from 1-7, please set the string number for "args" in file'.vscode/launch.json'
 // e.g. To run test0, set "args": ["0"] in file'.vscode/launch.json'
 */
int main(int argc, char** argv) {
	if (argc != 2) {
		std::cerr << "Usage: ./lab2 test1" << std::endl;
		return 1;
	}
	Z3Examples* z3Mgr = new Z3Examples(1000);
	bool result;
	std::string test_name = argv[1];
	if (test_name == "test0") {
		z3Mgr->test0();
		//  assert(x==5);
		z3::expr assert_cond = (z3Mgr->getZ3Expr("x") == z3Mgr->getZ3Expr(5));
		result = checkNegateAssert(z3Mgr, assert_cond) && z3Mgr->hasZ3Expr("x") && z3Mgr->z3Expr2NumValue(z3Mgr->getZ3Expr("x")) == 5;
	}
	else if (test_name == "test1") {
		z3Mgr->test1();
		// assert(b > 0);
		// You are suggested to write your own results checking here
		result = false;
	}
	else if (test_name == "test2") {
		z3Mgr->test2();
		// assert(b > 3);
		// You are suggested to write your own results checking here
		result = false;
	}
	else if (test_name == "test3") {
		z3Mgr->test3();
		// assert(x==10);
		// You are suggested to write your own results checking here
		result = false;
	}
	else if (test_name == "test4") {
		z3Mgr->test4();
		// assert((a + b)>20);
		// You are suggested to write your own results checking here
		result = false;
	}
	else if (test_name == "test5") {
		z3Mgr->test5();
		// assert(b1 >= 5);
		// You are suggested to write your own results checking here
		result = false;
	}
	else if (test_name == "test6") {
		z3Mgr->test6();
		// assert(*p == 5);
		// You are suggested to write your own results checking here
		result = false;
	}
	else if (test_name == "test7") {
		z3Mgr->test7();
		// assert(d == 5);
		// You are suggested to write your own results checking here
		result = false;
	}
	else if (test_name == "test8") {
		z3Mgr->test8();
		// assert(*p == 0);
		// You are suggested to write your own results checking here
		result = false;
	}
	else if (test_name == "test9") {
		z3Mgr->test9();
		//assert(z == 15);
		// You are suggested to write your own results checking here
		result = false;
	}
	else if (test_name == "test10") {
		z3Mgr->test10();
		// assert(x == 3 && y == 2);
		// You are suggested to write your own results checking here
		result = false;
	}
	else {
		std::cerr << "Invalid test name" << std::endl;
		return 1;
	}

	if (result) {
		std::cout << test_name << " passed!!" << std::endl;
	}
	else {
		std::cout << SVFUtil::errMsg(test_name)
		          << SVFUtil::errMsg(" assertion is unsatisfiable!!") << std::endl;
		assert(result);
	}
	z3Mgr->resetSolver();
	delete z3Mgr;
	return 0;
}