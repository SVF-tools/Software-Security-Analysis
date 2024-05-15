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

#include "Z3Examples.h"
#include "Util/Options.h"

using namespace z3;
using namespace SVF;
using namespace SVFUtil;

/*
 // Please set the "program": "${workspaceFolder}/bin/ass3" in file '.vscode/launch.json'
 // To run your testcase from 1-7, please set the string number for "args" in file'.vscode/launch.json'
 // e.g. To run test0, set "args": ["0"] in file'.vscode/launch.json'
 */
int main(int argc, char **argv) {
    Z3Examples* z3Mgr = new Z3Examples(1000);
    int points = 0;
    bool result;
    for (int n = 1; n <= 7; n++) {
        switch (n) {
            case 0:
                z3Mgr->test0(); // simple integers
                result = true;
                break;
            case 1:
                z3Mgr->test1(); // simple integers
                result = z3Mgr->getEvalExpr(z3Mgr->getZ3Expr("b") == z3Mgr->getZ3Expr(1)).is_true();
                break;
            case 2:
                z3Mgr->test2(); // one-level pointers
                result = z3Mgr->getEvalExpr(z3Mgr->getZ3Expr("b") == z3Mgr->getZ3Expr(4)).is_true();
                break;
            case 3:
                z3Mgr->test3(); // mutiple-level pointers
                result = z3Mgr->getEvalExpr(z3Mgr->loadValue(z3Mgr->getZ3Expr("q")) == z3Mgr->getZ3Expr(10)).is_true();
                break;
            case 4:
                z3Mgr->test4(); // array and pointers
                result = z3Mgr->getEvalExpr(z3Mgr->getZ3Expr("a") == z3Mgr->getZ3Expr(10)).is_true();
                break;

            case 5:
                z3Mgr->test5(); // struct and pointers
                result = z3Mgr->getEvalExpr(z3Mgr->loadValue(z3Mgr->getZ3Expr("q")) == z3Mgr->getZ3Expr(10)).is_true();
                break;

            case 6:
                z3Mgr->test6(); // branches
                result = z3Mgr->getEvalExpr(z3Mgr->getZ3Expr("b") == z3Mgr->getZ3Expr(5)).is_true();
                break;

            case 7:
                z3Mgr->test7(); // call
                result = (z3Mgr->getEvalExpr(z3Mgr->getZ3Expr("x") == 3)).is_true();
                break;
            default:
                assert(false && "wrong test number! input from 0 to 7 only");
                break;
        }
        if (result == false) {
            std::cout << SVFUtil::errMsg("The test-") << SVFUtil::errMsg(std::to_string(n)) << SVFUtil::errMsg(" assertion is unsatisfiable!!") << std::endl;
            assert(false);
        }
        else {
            std::cout << SVFUtil::sucMsg("The test-") << SVFUtil::sucMsg(std::to_string(n)) << SVFUtil::sucMsg(" assertion is successfully verified!!") << std::endl;
        }
        z3Mgr->resetSolver();
    }
    delete z3Mgr;
    return 0;
}