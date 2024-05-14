//===- GraphTraversal.cpp -- Graph algorithms ------------------//
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

#include "GraphAlgorithm.h"

using namespace std;


/// TODO: Implement your depth-first search here to traverse each program path (once for any loop) from src to dst.
/// Add each path as a string into std::set<std::string> paths.
/// Each path should have a format like this: "START->1->2->4->5->END", where -> indicates an edge connecting two node IDs.
void Graph::reachability(Node *src, Node *dst) {

 /// TODO: your code starts from here
}


/// TODO: Implement constraint solving by (1) propagating inclusion sets among nodes on CGraph, and (2) adding new copy edges until a fixed point is reached (i.e., no new copy edges are added).
/// The solving rules are as follows:
/// q <--COPY-- p   =>  IS(q) = IS(p)
/// q <--LOAD-- p   =>  for each o ∈ IS(p) : q <--COPY-- o
/// q <--STORE-- p  =>  for each o ∈ IS(q) : o <--COPY-- p
/// IS(q) denotes the inclusion set of node q
void CGraph::constraintSolving(CGraph* graph) {

 /// TODO: your code starts from here
}