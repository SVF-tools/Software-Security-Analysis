#include "AE/Core/AbstractState.h"
#include "AE/Core/ICFGWTO.h"
#include "Util/SVFBugReport.h"
namespace SVF {

	/// Exception handling for bug detections
	class AEException : public std::exception {
	 public:
		AEException(const std::string& message)
		: msg_(message) {}

		virtual const char* what() const throw() {
			return msg_.c_str();
		}

	 private:
		std::string msg_;
	};

	class AbstractExecutionHelper {
	 public:
		/// Add a detected bug to the bug reporter and print the report
		///@{
		void addBugToReporter(const AEException& e, const ICFGNode* node) {
			const SVFInstruction* inst = nullptr;

			// Determine the instruction associated with the ICFG node
			if (const CallICFGNode* call = SVFUtil::dyn_cast<CallICFGNode>(node)) {
				inst = call->getCallSite(); // If the node is a call node, get the call site instruction
			}
			else {
				inst = node->getSVFStmts().back()->getInst(); // Otherwise, get the last instruction of the node's
				                                              // statements
			}

			GenericBug::EventStack eventStack;
			SVFBugEvent sourceInstEvent(SVFBugEvent::EventType::SourceInst, inst);
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
