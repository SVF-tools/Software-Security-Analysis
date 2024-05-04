//===- Assignment-1.h -- Taint analysis ------------------//
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
 * Taint analysis
 *
 * Created on: Feb 18, 2024
 */

#ifndef SOFTWARE_SECURITY_ANALYSIS_ASSIGNMENT_1_H
#define SOFTWARE_SECURITY_ANALYSIS_ASSIGNMENT_1_H

#include "GraphTraversal.h"
#include "SVF-LLVM/LLVMUtil.h"
#include "SVF-LLVM/SVFIRBuilder.h"
#include "WPA/Andersen.h"
#include <filesystem>
namespace fs = std::filesystem;

#define CUR_DIR() (fs::path(__FILE__).parent_path())

using namespace SVF;

class ICFGTraversal : public GraphTraversal {
public:

    typedef std::vector<const SVFInstruction *> CallStack;
    typedef std::pair<const ICFGNode *, CallStack> ICFGNodeCallStackPair;

public:

    ICFGTraversal(SVFIR *p) : pag(p) {}

    /// TODO: to be implemented context sensitive DFS
    void DFS(const ICFGNode *src, const ICFGNode *dst);

    // Identify source nodes on ICFG (i.e., call instruction with its callee function named 'src')
    virtual std::set<const CallICFGNode *> &identifySources() {
        for (const CallICFGNode *cs: pag->getCallSiteSet()) {
            const SVFFunction *fun = SVFUtil::getCallee(cs->getCallSite());
            if (fun->getName() == "source") {
                sources.insert(cs);
            }
        }
        return sources;
    }

    // Identify sink nodes on ICFG (i.e., call instruction with its callee function named 'sink')
    virtual std::set<const CallICFGNode *> &identifySinks() {
        for (const CallICFGNode *cs: pag->getCallSiteSet()) {
            const SVFFunction *fun = SVFUtil::getCallee(cs->getCallSite());
            if (fun->getName() == "sink") {
                sinks.insert(cs);
            }
        }
        return sinks;
    }

protected:
    std::set<const CallICFGNode *> sources;
    std::set<const CallICFGNode *> sinks;
    Set<std::pair<const ICFGNode *, CallStack>> visited;
    CallStack callstack;

    SVFIR *pag;
};

class AndersenPTA : public SVF::AndersenBase {
public:
    // Constructor
    AndersenPTA(SVF::SVFIR *_pag) : AndersenBase(_pag) {};

    //dump constraint graph
    void dump_consCG(std::string name) {
        consCG->dump(name);
    };

private:

    // Process all the address constraint edges
    void processAllAddr();

    // To be implemented
    void solveWorklist() override;

    /// Add copy edge on constraint graph
    virtual bool addCopyEdge(SVF::NodeID src, SVF::NodeID dst) {
        if (consCG->addCopyCGEdge(src, dst))
            return true;
        else
            return false;
    }

};

class TaintGraphTraversal : public ICFGTraversal {

public:
    // mapping a type to its corresponding APIs, e.g., source -> {getenv, ...}
    TaintGraphTraversal(SVFIR *pag) : ICFGTraversal(pag) {}

    // Return true if two pointers are aliases
    bool aliasCheck(const CallICFGNode *src, const CallICFGNode *snk);

    // Identify source nodes on ICFG (i.e., call instruction with its callee function named 'src')
    std::set<const CallICFGNode *> &identifySources() override;

    // Identify sink nodes on ICFG (i.e., call instruction with its callee function named 'sink')
    std::set<const CallICFGNode *> &identifySinks() override;

    // TODO: implement the path printing
    void printICFGPath() override;

    // TODO: Source and sink function names read from SrcSnk.txt
    void readSrcSnkFromFile(const std::string &filename);

    // The driver method for taint checking
    void taintChecking();

private:
    AndersenPTA *ander;

    // default source and sink function name API if SrcSnk.txt is not added
    std::set<std::string> checker_source_api;
    std::set<std::string> checker_sink_api;

};

#endif

