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
 * Abstract Interpretation and bug detection
 *
 * Created on: Feb 19, 2024
 */

#include "Assignment_3.h"
#include "WPA/Andersen.h"

using namespace SVF;
using namespace SVFUtil;

void AbstractExecution::handleGlobalNode() {
	// TODO
}

void AbstractExecution::handleFunction(const ICFGNode* funEntry) {
	// TODO
}

bool AbstractExecution::handleICFGNode(const ICFGNode* node) {
	// TODO
	return false;
}

void AbstractExecution::handleICFGCycle(const ICFGCycleWTO* cycle) {
	// TODO
}

void AbstractExecution::handleCallSite(const CallICFGNode* callNode) {
	// TODO
}

void AbstractExecution::bufOverflowDetection(const ICFGNode* node) {
	// TODO
}

void AbstractExecution::nullptrDerefDetection(const ICFGNode* node) {
	// TODO
}
