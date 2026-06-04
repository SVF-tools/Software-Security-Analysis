//===- AEReporter.cpp -- Abstract Interpretation bug reporter --//
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
 * Bug-reporter implementations for the Assignment-3 harness.
 *
 * Holds the JSON-escape utility, target-report matching, and the AEReporter
 * coverage / summary methods.  All other harness state (AbstractExecution
 * driver, ICFG/WTO traversal, call-site dispatch, abstract-state helpers)
 * lives in AEHelper.cpp.
 */

#include "AEReporter.h"
#include "Graphs/ICFG.h"
#include <iomanip>
#include <sstream>

using namespace SVF;

std::string SVF::ass3JsonEscape(const std::string& input) {
	std::ostringstream os;
	for (char ch : input) {
		switch (ch) {
		case '"': os << "\\\""; break;
		case '\\': os << "\\\\"; break;
		case '\b': os << "\\b"; break;
		case '\f': os << "\\f"; break;
		case '\n': os << "\\n"; break;
		case '\r': os << "\\r"; break;
		case '\t': os << "\\t"; break;
		default:
			if (static_cast<unsigned char>(ch) < 0x20) {
				os << "\\u" << std::hex << std::setw(4) << std::setfill('0')
				   << static_cast<int>(static_cast<unsigned char>(ch))
				   << std::dec << std::setfill(' ');
			}
			else {
				os << ch;
			}
		}
	}
	return os.str();
}

static std::string ass3BaseName(const std::string& path) {
	size_t slash = path.find_last_of("/\\");
	if (slash == std::string::npos)
		return path;
	return path.substr(slash + 1);
}

static bool ass3ReportMatchesTarget(const AssignmentBugReport& report,
                                    const std::string& target) {
	if (target.empty())
		return false;
	if (report.location.find(target) != std::string::npos ||
	    report.message.find(target) != std::string::npos)
		return true;

	size_t colon = target.rfind(':');
	if (colon == std::string::npos)
		return false;
	std::string file = ass3BaseName(target.substr(0, colon));
	std::string line = target.substr(colon + 1);
	if (file.empty() || line.empty())
		return false;

	bool fileSeen = report.location.find(file) != std::string::npos ||
	                report.message.find(file) != std::string::npos;
	bool lineSeen = report.location.find("\"ln\": " + line) != std::string::npos ||
	                report.location.find("\"ln\":" + line) != std::string::npos ||
	                report.message.find("\"ln\": " + line) != std::string::npos ||
	                report.message.find("\"ln\":" + line) != std::string::npos ||
	                report.location.find(":" + line) != std::string::npos ||
	                report.message.find(":" + line) != std::string::npos;
	return fileSeen && lineSeen;
}

u32_t AEReporter::getTotalNodeCount(const ICFG* icfg) const {
	if (!icfg)
		return 0;
	u32_t total = 0;
	for (auto it = icfg->begin(); it != icfg->end(); ++it)
		total++;
	return total;
}

double AEReporter::getICFGCoverage(const ICFG* icfg) const {
	u32_t total = getTotalNodeCount(icfg);
	if (total == 0)
		return 0.0;
	return 100.0 * static_cast<double>(getAnalyzedNodeCount()) / static_cast<double>(total);
}

bool AEReporter::hasTargetReport() const {
	if (caseConfig.targetLoc.empty())
		return false;
	for (const AssignmentBugReport& report : _reports) {
		if (ass3ReportMatchesTarget(report, caseConfig.targetLoc))
			return true;
	}
	return false;
}

void AEReporter::writeJsonSummary(std::ostream& os, const ICFG* icfg,
                                  double wallSeconds, int exitCode,
                                  bool assertsValidated) const {
	const bool targetHit = hasTargetReport();
	const u32_t tp = caseConfig.targetLoc.empty() ? 0 : (targetHit ? 1 : 0);
	const u32_t fp = _reports.size() > tp ? static_cast<u32_t>(_reports.size() - tp) : 0;

	os << "{\n";
	os << "  \"case_id\": \"" << ass3JsonEscape(caseConfig.caseId) << "\",\n";
	os << "  \"target\": \"" << ass3JsonEscape(caseConfig.targetLoc) << "\",\n";
	os << "  \"tags\": \"" << ass3JsonEscape(caseConfig.tags) << "\",\n";
	os << "  \"exit_code\": " << exitCode << ",\n";
	os << "  \"asserts_validated\": " << (assertsValidated ? "true" : "false") << ",\n";
	os << "  \"tp\": " << tp << ",\n";
	os << "  \"fp\": " << fp << ",\n";
	os << "  \"reports\": " << _reports.size() << ",\n";
	os << "  \"wall_sec\": " << std::fixed << std::setprecision(3) << wallSeconds << ",\n";
	os << "  \"icfg_nodes\": " << getTotalNodeCount(icfg) << ",\n";
	os << "  \"analyzed_icfg_nodes\": " << getAnalyzedNodeCount() << ",\n";
	os << "  \"icfg_coverage\": " << std::fixed << std::setprecision(2) << getICFGCoverage(icfg) << ",\n";
	os << "  \"report_list\": [";
	for (size_t i = 0; i < _reports.size(); ++i) {
		const AssignmentBugReport& report = _reports[i];
		os << (i == 0 ? "\n" : ",\n");
		os << "    {\"kind\": \"" << ass3JsonEscape(report.kind)
		   << "\", \"node\": " << report.nodeId
		   << ", \"location\": \"" << ass3JsonEscape(report.location)
		   << "\", \"message\": \"" << ass3JsonEscape(report.message) << "\"}";
	}
	if (!_reports.empty())
		os << "\n  ";
	os << "]\n";
	os << "}\n";
}
