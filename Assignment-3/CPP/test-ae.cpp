#include "Assignment_3.h"
#include "SVF-LLVM/SVFIRBuilder.h"
#include "Util/CommandLine.h"
#include "Util/Options.h"
#include "WPA/Andersen.h"
#include "WPA/WPAPass.h"
#include <chrono>
#include <iostream>
#include <string>
#include <vector>

using namespace SVF;
using namespace SVFUtil;

static bool consumeAss3Option(const char* arg, AssignmentCaseConfig& config) {
	std::string opt(arg);
	auto valueOf = [&](const std::string& prefix) -> std::string {
		return opt.substr(prefix.size());
	};

	if (opt == "--emit-json") {
		config.emitJson = true;
		return true;
	}
	if (opt.rfind("--case-id=", 0) == 0) {
		config.caseId = valueOf("--case-id=");
		return true;
	}
	if (opt.rfind("--target=", 0) == 0) {
		config.targetLoc = valueOf("--target=");
		return true;
	}
	if (opt.rfind("--tags=", 0) == 0) {
		config.tags = valueOf("--tags=");
		return true;
	}
	return false;
}

int main(int argc, char** argv) {
	auto started = std::chrono::steady_clock::now();
	AssignmentCaseConfig config;

	std::vector<std::string> argStorage;
	argStorage.emplace_back(argv[0]);
	for (int i = 1; i < argc; ++i) {
		if (!consumeAss3Option(argv[i], config))
			argStorage.emplace_back(argv[i]);
	}

	// Assignment runner defaults. These are deliberately applied outside the
	// student implementation so grading cases are comparable.
	argStorage.emplace_back("-model-consts=true");
	argStorage.emplace_back("-model-arrays=true");
	argStorage.emplace_back("-pre-field-sensitive=false");
	argStorage.emplace_back("-field-limit=10000");
	argStorage.emplace_back("-stat=false");

	std::vector<char*> argValue;
	argValue.reserve(argStorage.size());
	for (std::string& arg : argStorage)
		argValue.push_back(const_cast<char*>(arg.c_str()));

	std::vector<std::string> moduleNameVec;
	moduleNameVec =
	    OptionBase::parseOptions(static_cast<int>(argValue.size()), argValue.data(),
	                             "Assignment 3 Abstract Execution",
	                             "[assignment-options] [svf-options] <input-bitcode...>");

	LLVMModuleSet::getLLVMModuleSet()->buildSVFModule(moduleNameVec);
	SVFIRBuilder builder;
	SVFIR* pag = builder.build();
	AndersenWaveDiff* ander = AndersenWaveDiff::createAndersenWaveDiff(pag);
	CallGraph* callgraph = ander->getCallGraph();
	builder.updateCallGraph(callgraph);
	pag->getICFG()->updateCallGraph(callgraph);
	AbstractExecution ae(config);
	ae.runOnModule(pag->getICFG());
	ae.ensureAllAssertsValidated();
	auto finished = std::chrono::steady_clock::now();
	double wallSeconds = std::chrono::duration<double>(finished - started).count();
	if (config.emitJson) {
		std::cout << "ASS3_JSON_BEGIN\n";
		ae.getReporter().writeJsonSummary(std::cout, pag->getICFG(), wallSeconds, 0, true);
		std::cout << "ASS3_JSON_END\n";
	}

	LLVMModuleSet::releaseLLVMModuleSet();
}
