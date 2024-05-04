//===- codegraph.cpp -- A driver program to dump SVFIR and ICFG------------------//
//
//                     SVF: Static Value-Flow Analysis
//
// Copyright (C) <2013-2022>  <Yulei Sui>
//

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Affero General Public License for more details.

// You should have received a copy of the GNU Affero General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//===----------------------------------------------------------------------===//
/*
 * A driver program to dump SVFIR and ICFG
 *
 * Created on: Feb 18, 2024
 */

#include "SVF-LLVM/SVFIRBuilder.h"
#include "Util/CommandLine.h"
#include "Util/Options.h"

using namespace SVF;
using namespace llvm;
using namespace std;

/*!
 * Function: traverseOnICFG
 * Description: Traverses the Interprocedural Control Flow Graph (ICFG) starting from a given ICFG node.
 *              It collects all successor nodes along the graph.
 * Parameters:
 *   - icfg: Pointer to the ICFG.
 *   - iNode: The starting ICFG node for traversal.
 */
void traverseOnICFG(ICFG *icfg, const ICFGNode *iNode) {
    // Initialize a FIFO worklist and a set to keep track of visited nodes
    FIFOWorkList<const ICFGNode *> worklist;
    Set<const ICFGNode *> visited;
    worklist.push(iNode);

    // Traverse along ICFG
    while (!worklist.empty()) {
        const ICFGNode *vNode = worklist.pop();
        // Iterate through the outgoing edges of the current node
        for (ICFGNode::const_iterator it = vNode->OutEdgeBegin(), eit = vNode->OutEdgeEnd(); it != eit; ++it) {
            ICFGEdge *edge = *it;
            ICFGNode *succNode = edge->getDstNode();
            // If the successor node has not been visited, add it to the visited set and worklist
            if (visited.find(succNode) == visited.end()) {
                visited.insert(succNode);
                worklist.push(succNode);
            }
        }
    }
}

// Entry point of the application
int main(int argc, char **argv) {

    std::vector<std::string> moduleNameVec;
    // Parse options for generating SVFIR and ICFG from input bitcode
    moduleNameVec = OptionBase::parseOptions(
            argc, argv, "Generate SVFIR and ICFG", "[options] <input-bitcode...>"
    );

    // Build the SVF module from the provided module names
    SVFModule *svfModule = LLVMModuleSet::getLLVMModuleSet()->buildSVFModule(moduleNameVec);

    // Use the SVFIRBuilder to build the SVFIR
    SVFIRBuilder builder(svfModule);
    SVFIR *pag = builder.build();

    // Dump the Program Assignment Graph (PAG) and the ICFG
    pag->dump(svfModule->getModuleIdentifier() + ".pag");
    ICFG *icfg = pag->getICFG();
    icfg->dump(svfModule->getModuleIdentifier() + ".icfg");

    // traverseOnICFG(icfg, icfg->getGlobalICFGNode());

    // Release resources associated with SVFIR and LLVM modules
    SVFIR::releaseSVFIR();
    SVF::LLVMModuleSet::releaseLLVMModuleSet();

    return 0;
}