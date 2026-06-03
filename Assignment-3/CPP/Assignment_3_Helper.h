//===- Assignment-3-Helper.h -- Abstract Interpretation Helper funcs --//
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
 * Abstract Interpretation Helper Functions
 *
 * Created on: Feb 19, 2024
 */

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
	class AbstractInterpretation;

	/// Narrow facade over the SVF abstract-interpretation state manager.
	///
	/// Students interact with the abstract state *only* through this object
	/// (the `svfStateMgr` member of AbstractExecution).  It forwards exactly the
	/// state read/write and GEP primitives the assignment is allowed to use, and
	/// deliberately exposes no path to the SVF external-API modeller
	/// (AbsExtAPI / handleExtAPI / getRangeLimitFromType / getUtils), so the
	/// memory/string library summaries and the cast-range logic must be written
	/// by hand.  Method bodies live in Assignment_3_Helper.cpp.
	class Ass3StateManager {
	 public:
		explicit Ass3StateManager(AbstractInterpretation* ai = nullptr) : ai(ai) {}

		const AbstractValue& getAbsValue(const ValVar* var, const ICFGNode* node);
		const AbstractValue& getAbsValue(const ObjVar* var, const ICFGNode* node);
		const AbstractValue& getAbsValue(const SVFVar* var, const ICFGNode* node);

		void updateAbsValue(const ValVar* var, const AbstractValue& val, const ICFGNode* node);
		void updateAbsValue(const ObjVar* var, const AbstractValue& val, const ICFGNode* node);
		void updateAbsValue(const SVFVar* var, const AbstractValue& val, const ICFGNode* node);

		AbstractValue loadValue(const ValVar* pointer, const ICFGNode* node);
		void storeValue(const ValVar* pointer, const AbstractValue& val, const ICFGNode* node);

		AddressValue getGepObjAddrs(const ValVar* pointer, IntervalValue offset);
		IntervalValue getGepElementIndex(const GepStmt* gep);
		IntervalValue getGepByteOffset(const GepStmt* gep);
		u32_t getAllocaInstByteSize(const AddrStmt* addr);

	 private:
		// harness-only: AbstractExecution reaches the underlying manager for the
		// post-trace; never exposed to student code.
		friend class AbstractExecution;
		AbstractInterpretation* ai;
	};

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

	class AbstractExecutionHelper {
	 public:

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
	};
}
