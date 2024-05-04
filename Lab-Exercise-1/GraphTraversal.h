//===- GraphTraversal.h -- depth-first search on a self-defined graph ------------------//
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
 * depth-first search on a self-defined graph
 *
 * Created on: Feb 18, 2024
 */

#ifndef SOFTWARE_SECURITY_ANALYSIS_GRAPHTRAVERSAL_H
#define SOFTWARE_SECURITY_ANALYSIS_GRAPHTRAVERSAL_H

#include <iostream>
#include <set>
#include <vector>

//declare
class Node;

class Edge;

class Node {
private:
    int nodeID;
    std::set<const Edge *> outEdges; // outgoing edges of this node

public:
    // constructor
    Node(int i) {
        nodeID = i;
    }

    // Get the private attribute nodeID
    int getNodeID() const {
        return nodeID;
    }

    // Get the private attribute outEdges
    std::set<const Edge *> getOutEdges() const {
        return outEdges;
    }

    // Add an edge into outEdges
    void addOutEdge(const Edge *edge) {
        outEdges.insert(edge);
    }
};

class Edge {
private:
    Node *src;  // source node of the edge
    Node *dst;  // target node of the edge

public:
    // Constructor
    Edge(Node *s, Node *d) : src(s), dst(d) {
    }

    // Get the source node
    Node *getSrc() const {
        return src;
    }

    // Get the target node
    Node *getDst() const {
        return dst;
    }
};

class Graph {
private:
    std::set<const Node *> nodes;   // a set of nodes on the graph

public:
    Graph() = default;

    // Get all the nodes of the graph
    std::set<const Node *> &getNodes() {
        return nodes;
    }

    // Add a node into the graph
    void addNode(const Node *node) {
        nodes.insert(node);
    }
};


class GraphTraversal {
public:
    // Constructor
    GraphTraversal() = default;

    // Destructor
    ~GraphTraversal() = default;

    /// To be implemented
    virtual void printICFGPath();

    /// To be implemented
    void DFS(const Node *src, const Node *dst);

    const std::set<std::string> &getPaths() {
        return paths;
    }

protected:
    std::set<std::string> paths;
    std::set<const Node *> visited;
    std::vector<unsigned> path;
};

#endif //SOFTWARE_SECURITY_ANALYSIS_GRAPHTRAVERSAL_H
