#include <iostream>
#include "Assignment-1.h"

fs::path commonRoot = CUR_DIR() / "testcases/icfg";

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
            gt->reachability(src, snk);
        }
    }
    std::string moduleName = moduleNameVec[0].substr(moduleNameVec[0].find_last_of('/') + 1);
    if (moduleName == "test1.ll") {
        std::set<std::string> expected = {"START->2->3->4->END"};
        assert(gt->getPaths() == expected && " \n wrong paths generated - test1 failed !");
    }
    else if (moduleName == "test2.ll") {
        std::set<std::string> expected = {"START->2->3->4->5->6->7->8->END", "START->2->3->4->5->6->END",
                                          "START->4->5->6->7->8->END", "START->4->5->6->END"};
        assert(gt->getPaths().size() == expected.size() && " \n wrong paths generated - test2 failed !");
        for (auto path : gt->getPaths()) {
            assert(expected.find(path) != expected.end() && " \n wrong paths generated - test2 failed !");
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