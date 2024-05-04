#include "Assignment-1.h"

#include "SVF-LLVM/SVFIRBuilder.h"
#include "Util/Options.h"
#include "Util/CommandLine.h"

fs::path commonRoot = CUR_DIR() / "testcases/pta";

void Test1() {

    SVF::SVFModule *svfModule = SVF::LLVMModuleSet::getLLVMModuleSet()->buildSVFModule(
            {commonRoot / "no_alias.ll"});
    /// Build Program Assignment Graph (SVFIR)
    SVF::SVFIRBuilder builder(svfModule);
    SVF::SVFIR *pag = builder.build();
    pag->dump(commonRoot / "no_alias_init");
    AndersenPTA *andersenPTA = new AndersenPTA(pag);
    andersenPTA->analyze();
    andersenPTA->dump_consCG(commonRoot / "no_alias_final");
    delete andersenPTA;
    SVF::LLVMModuleSet::releaseLLVMModuleSet();
    SVF::SVFIR::releaseSVFIR();
}

void Test2() {

    SVF::SVFModule *svfModule = SVF::LLVMModuleSet::getLLVMModuleSet()->buildSVFModule(
            {commonRoot / "CI-global.ll"});
    /// Build Program Assignment Graph (SVFIR)
    SVF::SVFIRBuilder builder(svfModule);
    SVF::SVFIR *pag = builder.build();
    pag->dump(commonRoot / "CI-global_init");
    AndersenPTA *andersenPTA = new AndersenPTA(pag);
    andersenPTA->analyze();
    andersenPTA->dump_consCG(commonRoot / "CI-global_final");
    delete andersenPTA;
    SVF::SVFIR::releaseSVFIR();
    SVF::LLVMModuleSet::releaseLLVMModuleSet();
}

void Test3() {
    SVF::SVFModule *svfModule = SVF::LLVMModuleSet::getLLVMModuleSet()->buildSVFModule(
            {commonRoot / "CI-local.ll"});
    /// Build Program Assignment Graph (SVFIR)
    SVF::SVFIRBuilder builder(svfModule);
    SVF::SVFIR *pag = builder.build();
    pag->dump(commonRoot / "CI-local_init");
    AndersenPTA *andersenPTA = new AndersenPTA(pag);
    andersenPTA->analyze();
    andersenPTA->dump_consCG(commonRoot / "CI-local_final");
    SVF::LLVMModuleSet::releaseLLVMModuleSet();
    SVF::SVFIR::releaseSVFIR();
    delete andersenPTA;
}

void Test() {
    Test1();
    Test2();
    Test3();
}


int main(int argc, char **argv) {
    int arg_num = 0;
    int extraArgc = 1;
    char **arg_value = new char *[argc + extraArgc];
    for (; arg_num < argc; ++arg_num) {
        arg_value[arg_num] = argv[arg_num];
    }

    // You may comment it to see the details of the analysis
    arg_value[arg_num++] = (char*) "-stat=false";
    (void) OptionBase::parseOptions(
            arg_num, arg_value, "Andersen's pointer analysis", "[options]"
    );
    Test();
    delete[] arg_value;
    return 0;
}