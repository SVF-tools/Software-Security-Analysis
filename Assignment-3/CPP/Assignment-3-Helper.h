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
#include "Util/SVFBugReport.h"
namespace SVF {
	class AbstractExecutionHelper {
	 public:
		/// Add a detected bug to the bug reporter and print the report
		///@{
		void addBugToReporter(const AEException& e, const ICFGNode* node) {

			GenericBug::EventStack eventStack;
			SVFBugEvent sourceInstEvent(SVFBugEvent::EventType::SourceInst, node);
			eventStack.push_back(sourceInstEvent); // Add the source instruction event to the event stack

			if (eventStack.size() == 0) {
				return; // If the event stack is empty, return early
			}

			std::string loc = eventStack.back().getEventLoc(); // Get the location of the last event in the stack

			// Check if the bug at this location has already been reported
			if (_bugLoc.find(loc) != _bugLoc.end()) {
				return; // If the bug location is already reported, return early
			}
			else {
				_bugLoc.insert(loc); // Otherwise, mark this location as reported
			}

			// Add the bug to the recorder with details from the event stack
			_recoder.addAbsExecBug(GenericBug::FULLBUFOVERFLOW, eventStack, 0, 0, 0, 0);
			_nodeToBugInfo[node] = e.what(); // Record the exception information for the node
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

		void addToGepObjOffsetFromBase(const GepObjVar* obj, const IntervalValue& offset) {
			gepObjOffsetFromBase[obj] = offset;
		}

		bool hasGepObjOffsetFromBase(const GepObjVar* obj) const {
			return gepObjOffsetFromBase.find(obj) != gepObjOffsetFromBase.end();
		}

		IntervalValue getGepObjOffsetFromBase(const GepObjVar* obj) const {
			if (hasGepObjOffsetFromBase(obj))
				return gepObjOffsetFromBase.at(obj);
			else
				assert(false && "GepObjVar not found in gepObjOffsetFromBase");
		}
		SVFBugReport& getBugReporter() {
			return _recoder;
		}

	 private:
		/// Map a GEP objVar to its offset from the base address
		/// e.g. alloca [i32*10] x; lhs = gep x, 3
		/// gepObjOffsetFromBase[lhs] = [12, 12]
		Map<const GepObjVar*, IntervalValue> gepObjOffsetFromBase;
		/// Bug reporter
		Set<std::string> _bugLoc;
		SVFBugReport _recoder;
		Map<const ICFGNode*, std::string> _nodeToBugInfo;
	};
}
