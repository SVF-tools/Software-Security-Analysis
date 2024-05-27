#include "Assignment-1.h"

#include "Graphs/PTACallGraph.h"
#include "SVF-LLVM/LLVMUtil.h"
#include "SVF-LLVM/SVFIRBuilder.h"
#include "Util/CommandLine.h"
#include "Util/Options.h"

using namespace std;

void TestICFG(std::vector<std::string>& moduleNameVec) {
	SVFModule* svfModule = LLVMModuleSet::getLLVMModuleSet()->buildSVFModule(moduleNameVec);

	/// Build Program Assignment Graph (SVFIR)
	SVFIRBuilder builder(svfModule);
	SVFIR* pag = builder.build();
	ICFG* icfg = pag->getICFG();
	// If you want to test your own case, please change the dump name
	ICFGTraversal* gt = new ICFGTraversal(pag);
	const fs::path& config = CUR_DIR() / "../SrcSnk.txt";
	gt->readSrcSnkFromFile(config);
	for (const CallICFGNode* src : gt->identifySources()) {
		for (const CallICFGNode* snk : gt->identifySinks()) {
			gt->reachability(src, snk);
		}
	}
	std::string moduleName = moduleNameVec[0].substr(moduleNameVec[0].find_last_of('/') + 1);
	if (moduleName == "test1.ll") {
		std::set<std::string> expected = {"START->5->6->7->8->11->1->2->3->12->15->END",
		                                  "START->5->6->7->8->9->1->2->3->10->13->END"};
		assert(gt->getPaths() == expected && " \n wrong paths generated - test1 failed !");
	}
	else if (moduleName == "test2.ll") {
		std::set<std::string> expected = {"START->16->1->2->END"};
		assert(gt->getPaths().size() == expected.size() && " \n wrong paths generated - test2 failed !");
		for (auto path : gt->getPaths()) {
			assert(expected.find(path) != expected.end() && " \n wrong paths generated - test2 failed !");
		}
	}

	for (auto path : gt->getPaths())
		std::cerr << path << "\n";
	SVFIR::releaseSVFIR();
	LLVMModuleSet::releaseLLVMModuleSet();
	delete gt;
}

void TestPTA(std::vector<std::string>& moduleNameVec) {
	SVFModule* svfModule = LLVMModuleSet::getLLVMModuleSet()->buildSVFModule(moduleNameVec);
	SVF::SVFIRBuilder builder(svfModule);
	SVF::SVFIR* pag = builder.build();
	AndersenPTA* andersenPTA = new AndersenPTA(pag);
	andersenPTA->analyze();
	delete andersenPTA;
	SVF::LLVMModuleSet::releaseLLVMModuleSet();
	SVF::SVFIR::releaseSVFIR();
}

void TestTaint(std::vector<std::string>& moduleNameVec) {
	SVFModule* svfModule = LLVMModuleSet::getLLVMModuleSet()->buildSVFModule(moduleNameVec);
	/// Build Program Assignment Graph (SVFIR)
	SVF::SVFIRBuilder builder(svfModule);
	SVF::SVFIR* pag = builder.build();

	ICFGTraversal* taint = new ICFGTraversal(pag);

	taint->taintChecking();
	std::string moduleName = moduleNameVec[0].substr(moduleNameVec[0].find_last_of('/') + 1);
	if (moduleName == "test1.ll") {
		set<string> expected = {"START->5->1->2->3->6->7->8->9->END"};
		assert(taint->getPaths() == expected && " \n wrong paths generated - test1 failed !");
		cout << "\n test1 passed !" << endl;
	}
	else if (moduleName == "test2.ll") {
		set<string> expected = {"START->5->1->2->3->6->7->8->9->10->12->14->END"};
		assert(taint->getPaths() == expected && " \n wrong paths generated - test4 failed !");
		cout << "\n test2 passed !" << endl;
	}
	SVF::SVFIR::releaseSVFIR();
	SVF::LLVMModuleSet::releaseLLVMModuleSet();
}

int main(int argc, char** argv) {
	int arg_num = 0;
	int extraArgc = 1;
	char** arg_value = new char*[argc + extraArgc];
	bool ptaEnabled = false;
	bool taintEnabled = false;
	bool icfgEnabled = false;
	int cur_arg = 0;
	for (; arg_num < argc; ++arg_num) {
		if (strcmp(argv[arg_num], "-pta") == 0) {
			ptaEnabled = true;
		}
		else if (strcmp(argv[arg_num], "-taint") == 0) {
			taintEnabled = true;
		}
		else if (strcmp(argv[arg_num], "-icfg") == 0) {
			icfgEnabled = true;
		}
		else {
			arg_value[cur_arg++] = argv[arg_num];
		}
	}
	// only one can be true
	if (ptaEnabled + taintEnabled + icfgEnabled == 0) {
		// default to taint
		taintEnabled = true;
		cout << "If no analysis is specified, the default is set to taint analysis (-taint)" << endl;
	}
	assert((ptaEnabled + taintEnabled + icfgEnabled) == 1 && "only one analysis can be enabled");

	// You may comment it to see the details of the analysis
	arg_value[cur_arg++] = (char*)"-stat=false";

	std::vector<std::string> moduleNameVec;
	moduleNameVec = OptionBase::parseOptions(cur_arg,
	                                         arg_value,
	                                         "Teaching-Software-Analysis Assignment 1",
	                                         "[options] <input-bitcode...>");
	if (ptaEnabled) {
		TestPTA(moduleNameVec);
	}
	else if (taintEnabled) {
		TestTaint(moduleNameVec);
	}
	else if (icfgEnabled) {
		TestICFG(moduleNameVec);
	}
	delete[] arg_value;
	return 0;
}