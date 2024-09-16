//===- Z3Examples.h -- Manual assertion-based verification (Z3 Example) ------------------//
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
 * Manual assertion-based verification (Z3 Example)
 *
 * Created on: Feb 19, 2024
 */

#ifndef SOFTWARE_SECURITY_ANALYSIS_Z3Examples_H
#define SOFTWARE_SECURITY_ANALYSIS_Z3Examples_H

#include "Z3Mgr.h"
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>
#include <string>

namespace SVF {
	class Z3Examples : public Z3Mgr {
	 public:
		Z3Examples(u32_t max)
		: Z3Mgr(max)
		, maxNumOfExpr(max)
		, currentExprIdx(0) {}

		// Return an z3 expr given an id
		inline z3::expr getZ3Expr(u32_t val) {
			return ctx.int_val(val);
		}

		// Return true if strToIDMap has this expression name
		inline bool hasZ3Expr(std::string exprName) {
			return strToIDMap.find(exprName) != strToIDMap.end();
		}

		// Return an z3 expr given a string name
		inline z3::expr getZ3Expr(std::string exprName) {
			auto it = strToIDMap.find(exprName);
			if (it != strToIDMap.end())
				return Z3Mgr::getZ3Expr(it->second);
			else {
				strToIDMap[exprName] = ++currentExprIdx;
				assert(maxNumOfExpr >= currentExprIdx && "creating more expression than upper limit");
				z3::expr e = ctx.int_const(exprName.c_str());
				updateZ3Expr(currentExprIdx, e);
				return e;
			}
		}

		// Return an z3 expr for an object given a string name
		inline z3::expr getMemObjAddress(std::string exprName) {
			z3::expr e = getZ3Expr(exprName);
			auto iter = strToIDMap.find(exprName);
			assert(iter != strToIDMap.end() && "address expr not found?");
			e = getZ3Expr(Z3Mgr::getVirtualMemAddress(iter->second));
			updateZ3Expr(iter->second, e);
			return e;
		}

		// Return a field object or array element object given a base pointer and an offset
		inline z3::expr getGepObjAddress(z3::expr pointer, u32_t offset) {
			std::string baseObjName = pointer.to_string();
			auto iter = strToIDMap.find(baseObjName);
			assert(iter != strToIDMap.end() && "Gep BaseObject expr not found?");
			u32_t baseObjID = iter->second;
			u32_t gepObj = baseObjID + offset;
			if (baseObjID == gepObj) {
				return pointer;
			}
			else {
				gepObj += maxNumOfExpr/2; // gep obj ID starts from maxNumOfExpr/2 in this lab exercise
				z3::expr e = getZ3Expr(Z3Mgr::getVirtualMemAddress(gepObj));
				updateZ3Expr(gepObj, e);
				return e;
			}
		}

		// Add an z3 expression into solver for later satisfiability solving
		void addToSolver(z3::expr e) {
			solver.add(e);
		}

		// Reset solver's stack and clear up the maps
		void resetSolver() {
			solver.reset();
			strToIDMap.clear();
			currentExprIdx = 0;
			clearVarID2ExprMap();
		}

		/// Print out all expressions' values after evaluation
		void printExprValues() {
			std::cout.flags(std::ios::left);
			std::cout << "-----------Var and Value-----------\n";
			for (auto nIter = strToIDMap.begin(); nIter != strToIDMap.end(); nIter++) {
				z3::expr e = Z3Mgr::getEvalExpr(Z3Mgr::getZ3Expr(nIter->second));
				if (e.is_numeral()) {
					s32_t value = e.get_numeral_int64();
					std::stringstream exprName;
					exprName << "Var" << nIter->second << " (" << nIter->first << ")";
					std::cout << std::setw(25) << exprName.str();
					if (Z3Mgr::isVirtualMemAddress(value))
						std::cout << "\t Value: " << std::hex << "0x" << value << "\n";
					else
						std::cout << "\t Value: " << std::dec << value << "\n";
				}
			}
			std::cout << "-----------------------------------------\n";
		}

		/// To implement
		///@{
		void test0();
		void test1();
		void test2();
		void test3();
		void test4();
		void test5();
		void test6();
		void test7();
		void test8();
		void test9();
		void test10();
		///@}

	 private:
		std::map<std::string, u32_t> strToIDMap;	/// map a string name to its corresponding id (only used in Lab-Exercise-2)
		u32_t maxNumOfExpr;
		u32_t currentExprIdx;
	};
} // namespace SVF

#endif // SOFTWARE_SECURITY_ANALYSIS_Z3Examples_H