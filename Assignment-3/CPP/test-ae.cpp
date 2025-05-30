#include "Assignment_3.h"
#include "SVF-LLVM/SVFIRBuilder.h"
#include "Util/CommandLine.h"
#include "Util/Options.h"
#include "WPA/Andersen.h"
#include "WPA/WPAPass.h"

using namespace SVF;
using namespace SVFUtil;

int main(int argc, char** argv) {
	int arg_num = 0;
	int extraArgc = 5;
	char** arg_value = new char*[argc + extraArgc];
	for (; arg_num < argc; ++arg_num) {
		arg_value[arg_num] = argv[arg_num];
	}
	// add extra options
	arg_value[arg_num++] = (char*)"-model-consts=true";
	arg_value[arg_num++] = (char*)"-model-arrays=true";
	arg_value[arg_num++] = (char*)"-pre-field-sensitive=false";
	arg_value[arg_num++] = (char*)"-field-limit=10000";
	arg_value[arg_num++] = (char*)"-stat=false";
	assert(arg_num == (argc + extraArgc) && "more extra arguments? Change the value of extraArgc");

	std::vector<std::string> moduleNameVec;
	moduleNameVec =
	    OptionBase::parseOptions(arg_num, arg_value, "Static Symbolic Execution", "[options] <input-bitcode...>");
	delete[] arg_value;

	LLVMModuleSet::getLLVMModuleSet()->buildSVFModule(moduleNameVec);
	SVFIRBuilder builder;
	SVFIR* pag = builder.build();
	AndersenWaveDiff* ander = AndersenWaveDiff::createAndersenWaveDiff(pag);
	CallGraph* callgraph = ander->getCallGraph();
	builder.updateCallGraph(callgraph);
	pag->getICFG()->updateCallGraph(callgraph);
	AbstractExecution* ae = new AbstractExecution();
	ae->runOnModule(pag->getICFG());
	ae->ensureAllAssertsValidated();

	LLVMModuleSet::releaseLLVMModuleSet();
}
