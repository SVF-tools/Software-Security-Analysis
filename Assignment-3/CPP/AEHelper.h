//===- AEHelper.h -- Abstract Interpretation harness --//
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

#ifndef ASSIGNMENT_3_AE_HELPER_H
#define ASSIGNMENT_3_AE_HELPER_H

#include "AEReporter.h"
#include "AEState.h"
#include "SVFIR/SVFIR.h"

namespace SVF {

	class AndersenWaveDiff;

	/// Harness services shared by the Assignment 3 implementation.
	class AEHelper {
	 public:
		explicit AEHelper(const AssignmentCaseConfig& config = AssignmentCaseConfig())
		: bugReporter(config) {}
		virtual ~AEHelper() = default;

		AEReporter& getReporter() {
			return bugReporter;
		}
		const AEReporter& getReporter() const {
			return bugReporter;
		}

		void runOnModule(ICFG* icfg);
		void analyse();
		void initWTO();
		void ensureAllAssertsValidated();

		void handleStubFunctions(const CallICFGNode* call);
		void handleCheckpointStubs(const CallICFGNode* call);
		bool isExternalCallForAssignment(const FunObjVar* func);

		void reportBufOverflow(const ICFGNode* node);
		void reportNullDeref(const ICFGNode* node);

		AEState& getAEState(const ICFGNode* node);
		void updateStateOnCall(const CallPE* call);

	 protected:
		virtual void handleGlobalNode() = 0;
		virtual void handleFunction(const ICFGNode* funEntry) = 0;

		SVFIR* svfir = nullptr;
		ICFG* icfg = nullptr;
		AndersenWaveDiff* ander = nullptr;
		Map<const FunObjVar*, ICFGWTO*> funcToWTO;
		Map<const ICFGNode*, AEState> preAbsTrace;
		Map<const ICFGNode*, AEState> postAbsTrace;

	 private:
		AEReporter bugReporter;
	};

} // namespace SVF

#endif // ASSIGNMENT_3_AE_HELPER_H
