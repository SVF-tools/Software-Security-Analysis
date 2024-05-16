#include "Assignment-1.h"

#include "SVF-LLVM/SVFIRBuilder.h"
#include "Util/Options.h"
#include "Util/CommandLine.h"


using namespace SVF;
using namespace SVFUtil;

int main(int argc, char** argv)
{
    int arg_num = 0;
    int extraArgc = 4;
    char **arg_value = new char *[argc + extraArgc];
    for (; arg_num < argc; ++arg_num)
    {
        arg_value[arg_num] = argv[arg_num];
    }
    // add extra options
    arg_value[arg_num++] = (char*) "-model-consts=true";
    arg_value[arg_num++] = (char*) "-model-arrays=true";
    arg_value[arg_num++] = (char*) "-pre-field-sensitive=false";
    arg_value[arg_num++] = (char*) "-stat=false";
    assert(arg_num == (argc + extraArgc) && "more extra arguments? Change the value of extraArgc");

    std::vector<std::string> moduleNameVec;
    moduleNameVec = OptionBase::parseOptions(
            arg_num, arg_value, "Static Symbolic Execution", "[options] <input-bitcode...>"
    );
    delete[] arg_value;

    SVFModule *svfModule = LLVMModuleSet::getLLVMModuleSet()->buildSVFModule(moduleNameVec);
    SVF::SVFIRBuilder builder(svfModule);
    SVF::SVFIR *pag = builder.build();
    AndersenPTA *andersenPTA = new AndersenPTA(pag);
    andersenPTA->analyze();
    delete andersenPTA;
    SVF::LLVMModuleSet::releaseLLVMModuleSet();
    SVF::SVFIR::releaseSVFIR();

    return 0;
}