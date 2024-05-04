#include "Assignment-1.h"

#include "SVF-LLVM/LLVMUtil.h"
#include "SVF-LLVM/SVFIRBuilder.h"
#include "Graphs/PTACallGraph.h"
#include "Util/Options.h"
#include "Util/CommandLine.h"


using namespace std;

fs::path commonRoot = CUR_DIR() / "testcases/taint";

void Test1() {
    cout << "\n running test1: " << endl;
    SVFModule *svfModule = LLVMModuleSet::getLLVMModuleSet()->buildSVFModule({commonRoot / "test1.ll"});
    /// Build Program Assignment Graph (SVFIR)
    SVF::SVFIRBuilder builder(svfModule);
    SVF::SVFIR *pag = builder.build();
    TaintGraphTraversal *taint = new TaintGraphTraversal(pag);
    taint->taintChecking();
    set<string> expected = {"START: 5->1->2->3->6->7->8->9->END"};
    assert(taint->getPaths() == expected && " \n wrong paths generated - test1 failed !");
    cout << "\n test1 passed !" << endl;
    SVF::LLVMModuleSet::releaseLLVMModuleSet();
    SVF::SVFIR::releaseSVFIR();
}

void Test2() {
    cout << "\n running test2 :" << endl;
    SVFModule *svfModule = LLVMModuleSet::getLLVMModuleSet()->buildSVFModule({commonRoot / "test2.ll"});
    /// Build Program Assignment Graph (SVFIR)
    SVF::SVFIRBuilder builder(svfModule);
    SVF::SVFIR *pag = builder.build();

    TaintGraphTraversal *taint = new TaintGraphTraversal(pag);

    taint->taintChecking();
    assert(taint->getPaths().size() == 0 && " \n should not exist tainted path - test2 failed !");
    cout << "\n test2 passed !" << endl;
    SVF::LLVMModuleSet::releaseLLVMModuleSet();
    SVF::SVFIR::releaseSVFIR();
}

void Test3() {
    cout << "\n running test3 :" << endl;
    SVFModule *svfModule = LLVMModuleSet::getLLVMModuleSet()->buildSVFModule({commonRoot / "test3.ll"});
    /// Build Program Assignment Graph (SVFIR)
    SVF::SVFIRBuilder builder(svfModule);
    SVF::SVFIR *pag = builder.build();

    TaintGraphTraversal *taint = new TaintGraphTraversal(pag);

    taint->taintChecking();
    assert(taint->getPaths().size() == 0 && " \n should not exist tainted path - test3 failed !");
    cout << "\n test3 passed !" << endl;
    SVF::SVFIR::releaseSVFIR();
    SVF::LLVMModuleSet::releaseLLVMModuleSet();

}

void Test4() {
    cout << "\n running test4 :" << endl;
    SVFModule *svfModule = LLVMModuleSet::getLLVMModuleSet()->buildSVFModule({commonRoot / "test4.ll"});
    /// Build Program Assignment Graph (SVFIR)
    SVF::SVFIRBuilder builder(svfModule);
    SVF::SVFIR *pag = builder.build();

    TaintGraphTraversal *taint = new TaintGraphTraversal(pag);

    taint->taintChecking();
    set<string> expected = {"START: 5->1->2->3->6->7->8->9->10->12->14->END"};
    assert(taint->getPaths() == expected && " \n wrong paths generated - test4 failed !");
    cout << "\n test4 passed !" << endl;
    SVF::LLVMModuleSet::releaseLLVMModuleSet();
    SVF::SVFIR::releaseSVFIR();
}

int main(int argc, char **argv) {
    int arg_num = 0;
    int extraArgc = 1;
    char **arg_value = new char *[argc + extraArgc];
    for (; arg_num < argc; ++arg_num) {
        arg_value[arg_num] = argv[arg_num];
    }

    // You may comment it to see the details of the analysis
    arg_value[arg_num++] = (char *) "-stat=false";
    (void) OptionBase::parseOptions(
            arg_num, arg_value, "Taint analysis", "[options]"
    );
    Test1();
    Test2();
    Test3();
    Test4();
    delete[] arg_value;
    return 0;
}