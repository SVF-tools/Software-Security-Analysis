//===- SVF IR and CodeGraph -- -------------------------------------//
//
//                     SVF: Static Value-Flow Analysis
//
// Copyright (C) <2013->  <Yulei Sui>
//

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//===-----------------------------------------------------------------------===//

/*
 // CodeGraphs including PAG, ICFG and ConstraintGraph
 //
 */

#include "Graphs/SVFG.h"
#include "SABER/LeakChecker.h"
#include "SVF-LLVM/LLVMUtil.h"
#include "SVF-LLVM/SVFIRBuilder.h"
#include "WPA/Andersen.h"

using namespace SVF;
using namespace llvm;
using namespace std;

int main(int argc, char** argv) {
	int arg_num = 0;
	int extraArgc = 4;
	char** arg_value = new char*[argc + extraArgc];
	for (; arg_num < argc; ++arg_num) {
		arg_value[arg_num] = argv[arg_num];
	}
	std::vector<std::string> moduleNameVec;

	int orgArgNum = arg_num;
	arg_value[arg_num++] = (char*)"-model-arrays=true";
	arg_value[arg_num++] = (char*)"-pre-field-sensitive=false";
	arg_value[arg_num++] = (char*)"-model-consts=true";
	arg_value[arg_num++] = (char*)"-stat=false";
	assert(arg_num == (orgArgNum + extraArgc) && "more extra arguments? Change the value of extraArgc");

	moduleNameVec = OptionBase::parseOptions(arg_num, arg_value, "SVF IR", "[options] <input-bitcode...>");

	SVFModule* svfModule = LLVMModuleSet::getLLVMModuleSet()->buildSVFModule(moduleNameVec);

	/// Build Program Assignment Graph (SVFIR or PAG)
	SVFIRBuilder builder(svfModule);
	SVFIR* pag = builder.build();
	// Dump pag
	pag->dump(svfModule->getModuleIdentifier() + ".pag");
	/// ICFG
	ICFG* icfg = pag->getICFG();
	/// Dump icfg
	icfg->dump(svfModule->getModuleIdentifier() + ".icfg");
	/// Create and dump ConstraintGraph
	ConstraintGraph* consCG = new ConstraintGraph(pag);
	consCG->dump(svfModule->getModuleIdentifier() + ".consG");

	std::cout << "\n\nPrinting code graphs...\n\n";

	/// iterate each ICFGNode on ICFG,
	/// where each node represents a SVFStatement (statement/instruction) and each edge represent a control-flow
	/// (execution order) between two statement
	std::cout << "\n\n####Printing ICFG (Interprocedural Control-Flow Graph)###\n\n";
	for (ICFG::iterator it = icfg->begin(); it != icfg->end(); it++) {
		std::cout << it->second->toString() << "\n\n";
	}
	std::cout << "######################################################\n";

	/// iterate each node on PAG,
	/// where each node represents an SVFVar (variable) and each edge represents a SVFStatement
	std::cout << "\n\n#######Printing PAG (Program Assignment Graph)#######\n\n";
	for (SVFIR::iterator it = pag->begin(); it != pag->end(); it++) {
		std::cout << it->second->toString() << "\n\n";
	}
	std::cout << "#################################################\n";

	/// iterate each node on Constraint Graph which is a subgraph of PAG.
	/// Constraint Graph only contains SVFVars are either pointers or objects (stack,global,heap and function objects)
	std::cout << "\n\n#############Printing Constraint Graph##########\n\n";
	for (ConstraintGraph::iterator it = consCG->begin(); it != consCG->end(); it++) {
		std::cout << it->second->toString() << "\n\n";
	}
	std::cout << "#################################################\n";

	return 0;
}
