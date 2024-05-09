#include "Z3Examples.h"
#include "Util/Options.h"

using namespace z3;
using namespace SVF;
using namespace SVFUtil;

/*
 // Software-Verification-Teaching Assignment 3 main function entry
 // Please set the "program": "${workspaceFolder}/bin/assign-3" in file '.vscode/launch.json'
 // To run your testcase from 1-7, please set the string number for "args" in file'.vscode/launch.json'
 // e.g. To run test0, set "args": ["0"] in file'.vscode/launch.json'
 */
int main(int argc, char **argv) {
    Z3Examples* z3Mgr = new Z3Examples(1000);
    int points = 0;
    int n = (argc == 1) ? 0 : atoi(argv[1]);
    bool result;
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
        z3Mgr->printExprValues();
        std::cout << SVFUtil::errMsg("The test-") << SVFUtil::errMsg(std::to_string(n)) << SVFUtil::errMsg(" assertion is unsatisfiable!!") << std::endl;
        assert(false);
    }
    else {
        z3Mgr->printExprValues();
        std::cout << SVFUtil::sucMsg("The test-") << SVFUtil::sucMsg(std::to_string(n)) << SVFUtil::sucMsg(" assertion is successfully verified!!") << std::endl;
    }
    z3Mgr->resetSolver();
    delete z3Mgr;
    return 0;
}