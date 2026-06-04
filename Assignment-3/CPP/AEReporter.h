//===- AEReporter.h -- Abstract Interpretation bug reporter --//
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

#ifndef ASSIGNMENT_3_AE_REPORTER_H
#define ASSIGNMENT_3_AE_REPORTER_H

#include "AE/Core/AbstractState.h"
#include "AE/Svfexe/AEDetector.h"
#include "AE/Core/ICFGWTO.h"
#include "SVFIR/SVFStatements.h"
#include "Util/Options.h"
#include "Util/SVFBugReport.h"
#include <ostream>
#include <string>
#include <vector>

namespace SVF {

	class ICFG;
	class CallICFGNode;

	struct AssignmentCaseConfig {
		std::string caseId;
		std::string targetLoc;
		std::string tags;
		bool emitJson = false;
	};

	struct AssignmentBugReport {
		std::string kind;
		std::string location;
		std::string message;
		NodeID nodeId = 0;
	};

	std::string ass3JsonEscape(const std::string& input);

	/// Bug reporter and harness bookkeeper for the Assignment-3
	/// abstract-interpretation pipeline.  Owns the bug list plus the
	/// harness-only state (analysed ICFG nodes, validated assertion
	/// call sites, case config) and renders the human / JSON summary
	/// consumed by the grader.
	class AEReporter {
	 public:
		AEReporter() = default;
		explicit AEReporter(const AssignmentCaseConfig& config) : caseConfig(config) {}

		const AssignmentCaseConfig& getCaseConfig() const { return caseConfig; }
		void setCaseConfig(const AssignmentCaseConfig& config) { caseConfig = config; }

		/// Coverage tracking: harness records every ICFG node it processed.
		void noteAnalyzed(const ICFGNode* node) { analyzedNodes.insert(node); }
		u32_t getAnalyzedNodeCount() const {
			return static_cast<u32_t>(analyzedNodes.size());
		}
		u32_t getTotalNodeCount(const ICFG* icfg) const;
		double getICFGCoverage(const ICFG* icfg) const;

		/// Assertion-point tracking: harness records every stub call site it
		/// actually reached so `ensureAllAssertsValidated` can detect missed
		/// ones.
		void noteAssertionPoint(const CallICFGNode* call) { assert_points.insert(call); }
		bool isAssertionPoint(const CallICFGNode* call) const {
			return assert_points.find(call) != assert_points.end();
		}

		bool hasTargetReport() const;
		void writeJsonSummary(std::ostream& os, const ICFG* icfg,
		                      double wallSeconds, int exitCode,
		                      bool assertsValidated) const;

		/// Add a detected bug to the bug reporter and print the report
		///@{
		void addBugToReporter(const AEException& e, const ICFGNode* node) {
			addBugToReporter("buffer-overflow", e, node);
		}

		void addBugToReporter(const std::string& kind, const AEException& e, const ICFGNode* node) {

			GenericBug::EventStack eventStack;
			SVFBugEvent sourceInstEvent(SVFBugEvent::EventType::SourceInst, node);
			eventStack.push_back(sourceInstEvent); // Add the source instruction event to the event stack

			if (eventStack.size() == 0) {
				return; // If the event stack is empty, return early
			}

			std::string loc = eventStack.back().getEventLoc(); // Get the location of the last event in the stack

			// Check if the bug at this location has already been reported
			const std::string dedupKey = kind + ":" + loc;
			if (_bugLoc.find(dedupKey) != _bugLoc.end()) {
				return; // If the bug location is already reported, return early
			}
			else {
				_bugLoc.insert(dedupKey); // Otherwise, mark this location as reported
			}

			// Add the bug to the recorder with details from the event stack
			_recoder.addAbsExecBug(GenericBug::FULLBUFOVERFLOW, eventStack, 0, 0, 0, 0);
			_nodeToBugInfo[node] = e.what(); // Record the exception information for the node
			_reports.push_back({kind, loc, e.what(), node ? node->getId() : 0});
		}

		void printReport() {
			if (_nodeToBugInfo.size() > 0) {
				std::cerr << "######################Buffer Overflow (" + std::to_string(_nodeToBugInfo.size())
				                 + " found)######################\n";
				std::cerr << "---------------------------------------------\n";
				for (auto& it : _nodeToBugInfo) {
					std::cerr << it.second << "\n---------------------------------------------\n";
				}
			}
		}
		///@}

		const std::vector<AssignmentBugReport>& getReports() const {
			return _reports;
		}

		u32_t getReportCount() const {
			return static_cast<u32_t>(_reports.size());
		}

		SVFBugReport& getBugReporter() {
			return _recoder;
		}

	 private:
		/// Bug reporter
		Set<std::string> _bugLoc;
		SVFBugReport _recoder;
		Map<const ICFGNode*, std::string> _nodeToBugInfo;
		std::vector<AssignmentBugReport> _reports;

		/// Harness bookkeeping (moved here from AbstractExecution so
		/// Assignment_3.h stays focused on student-facing surface).
		AssignmentCaseConfig caseConfig;
		Set<const ICFGNode*> analyzedNodes;
		Set<const CallICFGNode*> assert_points;
	};

} // namespace SVF

#endif // ASSIGNMENT_3_AE_REPORTER_H
