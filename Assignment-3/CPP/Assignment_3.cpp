//===- Assignment-3.cpp -- Abstract Interpretation --//
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
 * Abstract Interpretation and buffer overflow detection
 *
 * Created on: Feb 19, 2024
 */

#include "Assignment_3.h"

using namespace SVF;
using namespace SVFUtil;

// ===========================================================================
// Student TODOs
// ===========================================================================
// Implement abstract interpretation for the assignment cases. The harness
// (Assignment_3_Helper.cpp) drives the analysis and only calls into the six
// entry points below; design and add any internal helpers you need.
// ===========================================================================

void AbstractExecution::updateAbsState(const SVFStmt* stmt) {
	// TODO: dispatch on the statement subtype and update the abstract state.
}

bool AbstractExecution::mergeStatesFromPredecessors(const ICFGNode* block, AbstractState& as) {
	// TODO: join predecessor post-states (with branch refinement) into `as`.
	return false;
}

void AbstractExecution::handleICFGCycle(const ICFGCycleWTO* cycle) {
	// TODO: iterate the cycle body to a fixpoint (widening optional).
}

void AbstractExecution::bufOverflowDetection(const ICFGNode* node) {
	// TODO: detect out-of-bounds memory accesses at `node`.
}

void AbstractExecution::nullptrDerefDetection(const ICFGNode* node) {
	// TODO: detect nullptr dereferences at `node`.
}

void AbstractExecution::updateStateOnExtCall(const SVF::CallICFGNode* call) {
	// TODO: model memory/string library calls and assignment-specific stubs.
}
