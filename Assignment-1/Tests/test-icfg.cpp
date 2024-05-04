#include <iostream>
#include "Assignment-1.h"

fs::path commonRoot = CUR_DIR() / "testcases/trav";

void Test(std::vector<std::string>& moduleNameVec) {

    SVFModule *svfModule = LLVMModuleSet::getLLVMModuleSet()->buildSVFModule(moduleNameVec);

    /// Build Program Assignment Graph (SVFIR)
    SVFIRBuilder builder(svfModule);
    SVFIR *pag = builder.build();
    ICFG *icfg = pag->getICFG();
    // If you want to test your own case, plase change the dump name
    //icfg->dump(commonRoot / "test1.ll.icfg");
    ICFGTraversal *gt = new ICFGTraversal(pag);
    for (const CallICFGNode *src: gt->identifySources()) {
        for (const CallICFGNode *snk: gt->identifySinks()) {
            gt->DFS(src, snk);
        }
    }
    for(auto path : gt->getPaths())
        std::cerr << path << "\n";
    SVFIR::releaseSVFIR();
    LLVMModuleSet::releaseLLVMModuleSet();
    delete gt;
}

int main(int argc, char *argv[]) {
    int arg_num = 0;
    int extraArgc = 1;
    char **arg_value = new char *[argc + extraArgc];
    for (; arg_num < argc; ++arg_num) {
        arg_value[arg_num] = argv[arg_num];
    }

    // You may comment it to see the details of the analysis
    arg_value[arg_num++] = (char*) "-stat=false";

    std::vector<std::string> moduleNameVec;
    moduleNameVec = OptionBase::parseOptions(
            arg_num, arg_value, "Teaching-Software-Analysis Assignment 3", "[options] <input-bitcode...>"
    );


    Test(moduleNameVec);
    return 0;
}