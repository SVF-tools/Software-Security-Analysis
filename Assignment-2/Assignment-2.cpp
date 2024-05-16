//===- Assignment-2.cpp -- Automated assertion-based verification (Static symbolic execution) --//
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
 * Automated assertion-based verification (Static symbolic execution)
 *
 * Created on: Feb 19, 2024
 */

#include "Assignment-2.h"
#include "Util/Options.h"

using namespace SVF;
using namespace SVFUtil;
using namespace llvm;
using namespace z3;

/// TODO: Implement your context-sensitive ICFG traversal here to traverse each program path (once for any loop) from src to dst
void SSE::reachability(const ICFGEdge *curEdge, const ICFGNode *sink) {

    /// TODO: your code starts from here
}

/// TODO: collect each path once this method is called during reachability analysis, and
/// add each path as a string into std::set<std::string> paths
/// Print the path in the format "START->1->2->4->5->END", where -> indicate an ICFGEdge connects two ICFGNode IDs
void SSE::collectICFGPath()
{

    /// TODO: your code starts from here
}

/// TODO: Implement handling of function calls
/// Return true means a feasible path, false otherwise
bool SSE::handleCall(const CallCFGEdge* calledge){

    /// TODO: your code starts from here
    return true;
}

/// TODO: Implement handling of function returns
/// Return true means a feasible path, false otherwise
bool SSE::handleRet(const RetCFGEdge* retEdge){

    /// TODO: your code starts from here
    return true;
}

/// TODO: Implement handling of branch statement inside a function
/// Return true means a feasible path, false otherwise
bool SSE::handleBranch(const IntraCFGEdge* edge){

    /// TODO: your code starts from here
    return true;
}

/// TODO: Implement handling of non-branch statement inside a function
/// including handling of (1) AddrStmt, (2) CopyStmt, (3) LoadStmt, (4) StoreStmt, (5) GepStmt and (6) any other types of intra SVFStmts
/// Return true means a feasible path, false otherwise
bool SSE::handleNonBranch(const IntraCFGEdge* edge) {

    /// TODO: your code starts from here
    return true;
}

/// Traverse each program path
bool SSE::translatePath(std::vector<const ICFGEdge *> &path){

    for(const ICFGEdge* edge : path){
        if(const IntraCFGEdge* intraEdge = SVFUtil::dyn_cast<IntraCFGEdge>(edge)){
            if(handleIntra(intraEdge) == false)
                return false;
        }
        else if (const CallCFGEdge *call = SVFUtil::dyn_cast<CallCFGEdge>(edge)){
            handleCall(call);
        }
        else if(const RetCFGEdge *ret = SVFUtil::dyn_cast<RetCFGEdge>(edge)){
            handleRet(ret);
        }
        else
            assert(false && "what other edges we have?");
    }

    return true;
}


/// Program entry
void SSE::analyse()
{
    for (const ICFGNode *src : identifySources()) {
        assert(SVFUtil::isa<GlobalICFGNode>(src) && "reachability should start with GlobalICFGNode!");
        for (const ICFGNode *sink: identifySinks()) {
            const IntraCFGEdge startEdge(nullptr,const_cast<ICFGNode*>(src));
            handleIntra(&startEdge);
            reachability(&startEdge, sink);
            resetSolver();
        }
    }
}
