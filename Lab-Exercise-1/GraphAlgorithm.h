//===- GraphTraversal.h ---- Graph algorithms ------------------//
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
 * Graph reachability and constraint solving on a self-defined graph
 *
 * Created on: Feb 18, 2024
 */

#ifndef SOFTWARE_SECURITY_ANALYSIS_GRAPHTRAVERSAL_H
#define SOFTWARE_SECURITY_ANALYSIS_GRAPHTRAVERSAL_H

#include <cassert>
#include <iostream>
#include <map>
#include <queue>
#include <set>
#include <vector>

class Edge;

/// Node class
class Node {
 private:
	unsigned nodeID;
	std::set<const Edge*> outEdges; // outgoing edges of this node

 public:
	// constructor
	Node(unsigned i) {
		nodeID = i;
	}

	// Get the private attribute nodeID
	unsigned getNodeID() const {
		return nodeID;
	}

	// Get the private attribute outEdges
	std::set<const Edge*> getOutEdges() const {
		return outEdges;
	}

	// Add an edge into outEdges
	void addOutEdge(const Edge* edge) {
		outEdges.insert(edge);
	}
};

/// Edge class
class Edge {
 protected:
	Node* src; // source node of the edge
	Node* dst; // target node of the edge

 public:
	// Constructor
	Edge(Node* s, Node* d)
	: src(s)
	, dst(d) {}

	// Get the source node
	Node* getSrc() const {
		return src;
	}

	// Get the target node
	Node* getDst() const {
		return dst;
	}
};

/// Graph class
class Graph {
 private:
	std::set<const Node*> nodes; // a set of nodes on the graph

 public:
	Graph(){};

	// Get all the nodes of the graph
	std::set<const Node*>& getNodes() {
		return nodes;
	}

	// Add a node into the graph
	void addNode(const Node* node) {
		nodes.insert(node);
	}

	/// TODO: To be implemented
	void reachability(Node* src, Node* dst);

	/// Return all the possible paths from a src to a sink on the graph
	std::set<std::string> getPaths() {
		return paths;
	}

 private:
	std::set<std::string> paths;
	std::set<Node*> visited;
	std::vector<unsigned> path;
};

class CGEdge;
/// Constraint Graph Node class
class CGNode {
 private:
	std::set<unsigned> pointsToSet; // a node's points-to set (e.g., a set of nodes that this node points to)
	std::set<CGEdge*> inEdges; /// a set of incoming edges to this node
	std::set<CGEdge*> outEdges; /// a set of outgoing edges from this node
	unsigned nodeID;

 public:
	/// Constructor
	CGNode(unsigned i)
	: nodeID(i) {}

	/// The points-to set of this node
	std::set<unsigned>& getPts() {
		return pointsToSet;
	}

	/// Return the outgoing edges
	std::set<CGEdge*> getOutEdges() {
		return outEdges;
	}

	/// Return the incoming edges
	std::set<CGEdge*> getInEdges() {
		return inEdges;
	}

	/// Add the outgoing edges of this node
	void addOutEdge(CGEdge* edge) {
		outEdges.insert(edge);
	}

	/// Add the incoming edges of this node
	void addInEdge(CGEdge* edge) {
		inEdges.insert(edge);
	}

	/// Return the NodeID
	unsigned getID() {
		return nodeID;
	}
};

/// Constraint Graph Edge class
class CGEdge {
 public:
	enum EdgeType {
		ADDR, // p=&a     p<--ADDR--a    where p and a are two CGNodes on the graph
		COPY, // p=q      p<--COPY--q
		STORE, // *p=q     p<--STORE--q
		LOAD // p=*q     p<--LOAD--q
	};

	/// Constructor
	CGEdge(CGNode* s, CGNode* d, EdgeType t)
	: src(s)
	, dst(d)
	, ty(t) {}

	/// Return type of this edge
	EdgeType getType() const {
		return ty;
	}

	// Get the source node
	CGNode* getSrc() {
		return src;
	}

	// Get the target node
	CGNode* getDst() {
		return dst;
	}

 private:
	EdgeType ty;
	CGNode* src;
	CGNode* dst;
};

class CGraph {
 private:
	std::set<CGEdge*> edges; // a set of edges on the graph
	std::map<unsigned, CGNode*> IDToNodeMap; // a map from NodeID to CGNode*

 public:
	/// Constructor
	CGraph() {}

	/// Add an edge to the graph
	bool addEdge(CGNode* s, CGNode* d, CGEdge::EdgeType t) {
		for (auto e : edges) {
			if (e->getSrc() == s && e->getDst() == d && e->getType() == t) {
				return false;
			}
		}
		CGEdge* newEdge = new CGEdge(s, d, t);
		s->addOutEdge(newEdge);
		d->addInEdge(newEdge);
		edges.insert(newEdge);
		return true;
	}

	/// Add a Node to the graph
	void addNode(CGNode* s) {
		IDToNodeMap[s->getID()] = s;
	}

	// Rerturn CGNode* given an id
	CGNode* getNode(unsigned id) const {
		std::map<unsigned, CGNode*>::const_iterator it = IDToNodeMap.find(id);
		if (it != IDToNodeMap.end())
			return it->second;
		else {
			assert(false && "CGNode not found!");
			abort();
		}
	}

	/// Return the points-to set of a node
	std::set<unsigned>& getPts(unsigned id) const {
		CGNode* node = getNode(id);
		return node->getPts();
	}

	/// Add o to the points-to set of p
	bool addPts(CGNode* p, CGNode* o) {
		return p->getPts().insert(o->getID()).second;
	}

	/// Union the points-to set of q to that of p
	/// pts(p) = pts(p) ∪ pts(q)
	bool unionPts(CGNode* p, CGNode* q) {
		bool changed = false;
		for (auto e : q->getPts()) {
			if (p->getPts().find(e) == p->getPts().end()) {
				p->getPts().insert(e);
				changed = true;
			}
		}
		return changed;
	}

	/// TODO: to be implemented
	void solveWorklist();

	/// Push a node into the worklist for further processing
	void pushIntoWorklist(unsigned id) {
		worklist.push(id);
	}

	/// Pop a node from the worklist and return the node id
	unsigned popFromWorklist() {
		unsigned nodeId = worklist.front();
		worklist.pop();
		return nodeId;
	}

 protected:
	std::set<std::string> paths;
	std::set<Node*> visited;
	std::vector<unsigned> path;
	std::queue<unsigned> worklist;
};

#endif // SOFTWARE_SECURITY_ANALYSIS_GRAPHTRAVERSAL_H
