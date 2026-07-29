//===- Assignment-3.h -- Abstract Interpretation --//
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
#include "AEHelper.h"
#include <ostream>

namespace SVF {

	/// Assignment 3 feature implementation.
	class AbstractExecution : public AEHelper {
	 public:
		explicit AbstractExecution(const AssignmentCaseConfig& config = AssignmentCaseConfig())
		: AEHelper(config) {}
		~AbstractExecution() override = default;

		void handleCallSite(const CallICFGNode* callnode);

		void handleGlobalNode() override;
		void handleFunction(const ICFGNode* funEntry) override;
		bool handleICFGNode(const ICFGNode* node);
		void handleICFGCycle(const ICFGCycleWTO* cycle);

		void bufOverflowDetection(const ICFGNode* node);
		void nullptrDerefDetection(const ICFGNode* node);
	};

} // namespace SVF
