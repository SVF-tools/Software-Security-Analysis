//===- Assignment-1.cpp -- Taint analysis ------------------//
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
 * Graph reachability, Andersen's pointer analysis and taint analysis
 *
 * Created on: Feb 18, 2024
 */

#include "Assignment-1.h"
#include <fstream>
#include <sstream>
#include <string>
#include <filesystem>
#include <sys/stat.h>
#include "WPA/Andersen.h"

using namespace SVF;
using namespace llvm;
using namespace std;

/// TODO: Implement your context-sensitive ICFG traversal here to traverse each program path (once for any loop) from src to dst
/// Printout the path from src to dst by calling collectICFGPath
void ICFGTraversal::reachability(const ICFGNode *src, const ICFGNode *dst) {

    /// TODO: your code starts from here
}

/// TODO: collect each path once this method is called, and
/// (1) add each path (a sequence of node IDs) as a string into std::set<std::string> paths
/// in the format "START->1->2->4->5->END", where -> indicate an ICFGEdge connects two ICFGNode IDs
void ICFGTraversal::collectICFGPath() {

    /// TODO: your code starts from here
}

// TODO: Implement your Andersen's Algorithm here
void AndersenPTA::solveWorklist() {

    /// TODO: your code starts from here
}

// TODO: Implement your code to parse the two lines from `SrcSnk.txt` in the form of
// line 1 for sources  "{ api1 api2 api3 }"
// line 2 for sinks    "{ api1 api2 api3 }"
void ICFGTraversal::readSrcSnkFromFile(const string &filename) {

    /// TODO: your code starts from here
}

/// TODO: Checking aliases of the two variables at source and sink. For example:
/// src instruction:  actualRet = source();
/// snk instruction:  sink(actualParm,...);
/// return true if actualRet is aliased with any parameter at the snk node (e.g., via ander->alias(..,..))
bool ICFGTraversal::aliasCheck(const CallICFGNode *src, const CallICFGNode *snk) {

    /// TODO: your code starts from here
    return false;
}

// Process all address constraints to initialize pointers' points-to sets
void AndersenPTA::processAllAddr() {
    for (ConstraintGraph::const_iterator nodeIt = consCG->begin(), nodeEit = consCG->end();
         nodeIt != nodeEit; nodeIt++) {
        ConstraintNode *cgNode = nodeIt->second;
        for (ConstraintEdge *edge: cgNode->getAddrInEdges()) {
            const AddrCGEdge *addr = SVFUtil::cast<AddrCGEdge>(edge);
            NodeID dst = addr->getDstID();
            NodeID src = addr->getSrcID();
            if (addPts(dst, src))
                pushIntoWorklist(dst);
        }
    }
}

// Start taint checking.
// There is a tainted flow from p@source to q@sink
// if (1) alias(p,q)==true and (2) source reaches sink on ICFG.
void ICFGTraversal::taintChecking() {
    const fs::path &config = CUR_DIR() / "SrcSnk.txt";
    // configure sources and sinks for taint analysis
    readSrcSnkFromFile(config);

    // Set file permissions to read-only for user, group and others
    if (chmod(config.string().c_str(), S_IRUSR | S_IRGRP | S_IROTH) == -1) {
        std::cerr << "Error setting file permissions for " << config
                  << ": " << std::strerror(errno) << std::endl;
        abort();
    }
    ander = new AndersenPTA(pag);
    ander->analyze();
    for (const CallICFGNode *src: identifySources()) {
        for (const CallICFGNode *snk: identifySinks()) {
            if (aliasCheck(src, snk))
                reachability(src, snk);
        }
    }
}
