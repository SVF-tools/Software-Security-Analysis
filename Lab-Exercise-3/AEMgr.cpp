//===- AEMgr.cpp -- Abstract Execution Manual Translations---------------------------------//
//
//                     SVF: Static Value-Flow Analysis
//
// Copyright (C) <2013->  <Yulei Sui>
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

//
// Created on 2024/1/10.
//

#include "AEMgr.h"

namespace SVF {
	AEState AbstractExecutionMgr::test0() {
		/*
		// A simple example

		int main() {
			int* p;
			int q;
			int* r;
			int x;

			p = malloc;
			q = 5;
			*p = q;
			x = *p;
			assert(x==10);
		}
		*/

		AEState as;
		//  int** p;
		NodeID p = getNodeID("p");

		//  int q;
		NodeID q = getNodeID("q");

		//  int* r;
		NodeID r = getNodeID("r");

		//  int x;
		NodeID x = getNodeID("x");

		// p = malloc(..);
		NodeID malloc = getNodeID("malloc");
		as[p] = AddressValue(getMemObjAddress("malloc"));

		// q = 5;
		as[q] = IntervalValue(5, 5);

		// *p = q;
		as.storeValue(p, as[q]);

		// x = *p;
		as[x] = as.loadValue(p);
		as.printAbstractState();
		return as;
	}

	//    int main() {
	//        int a;
	//        int b;
	//        a = 0;
	//        b = a + 1;
	//        assert(b>0);
	//    }
	AEState AbstractExecutionMgr::test1() {
		AEState as;
		NodeID a = getNodeID("a");
		NodeID b = getNodeID("b");
		/// TODO: your code starts from here


		return as;
	}

	//    int main() {
	//        int* p;
	//        int q;
	//        int b;
	//        int a;
	//
	//        p = malloc;
	//        *p = 0;
	//        q = *p;
	//        *p = 3;
	//        b = *p + 1;
	//        assert(b>3);
	//    }
	AEState AbstractExecutionMgr::test2() {
		AEState as;
		NodeID p = getNodeID("p");
		NodeID q = getNodeID("q");
		NodeID b = getNodeID("b");
		/// TODO: your code starts from here


		return as;
	}

	//    int main() {
	//        int** p;
	//        int* q;
	//        int* r;
	//        int x;
	//
	//        p = malloc1(..);
	//        q = malloc2(..);
	//        *p = q; // storeAddr
	//        *q = 10; // storeValue
	//        r = *p;
	//        x = *r;
	//        assert(x==10);
	//    }
	AEState AbstractExecutionMgr::test3() {
		AEState as;
		NodeID p = getNodeID("p");
		NodeID q = getNodeID("q");
		NodeID r = getNodeID("r");
		NodeID x = getNodeID("x");
		/// TODO: your code starts from here


		return as;
	}

	//        int* p;
	//        int* x;
	//        int* y;
	//        int a;
	//        int b;
	//        p = malloc;
	//        x = &p[0];
	//        y = &p[1]
	//        *x = 10;
	//        *y = 11;
	//        a = *x;
	//        b = *y;
	//        assert((a + b)>20);
	AEState AbstractExecutionMgr::test4() {
		AEState as;
		NodeID p = getNodeID("p");
		NodeID x = getNodeID("x");
		NodeID y = getNodeID("y");
		NodeID a = getNodeID("a");
		NodeID b = getNodeID("b");
		/// TODO: your code starts from here


		return as;

	}

	//// Struct and pointers
	//
	//    struct A{ int f0; int* f1;};
	//    int main() {
	//       struct A* p;
	//       int* x;
	//       int* q;
	//       int** r;
	//       int* y;
	//       int z;
	//
	//       p = malloc1;
	//       x = malloc2;
	//       *x = 5;
	//       q = &(p->f0);
	//       *q = 10;
	//       r = &(p->f1);
	//       *r = x;
	//       y = *r;
	//       z = *q + *y;
	//       assert(z==15);
	AEState AbstractExecutionMgr::test5() {
		AEState as;
		NodeID p = getNodeID("p", 2);
		NodeID x = getNodeID("x");
		NodeID q = getNodeID("q");
		NodeID r = getNodeID("r");
		NodeID y = getNodeID("y");
		NodeID z = getNodeID("z");
		/// TODO: your code starts from here


		return as;
	}

	//    int main(int argv) {  // argv is an interval  [4, 10]
	//    int a;
	//    int b;
	//    a = argv + 1;
	//    b = 5;
	//    if(a > 10)
	//       b = a;
	//    assert(b>=5);
	//    }
	AEState AbstractExecutionMgr::test6() {
		AEState as;
		NodeID a = getNodeID("a");
		NodeID b = getNodeID("b");
		NodeID argv = getNodeID("argv");
		/// TODO: your code starts from here


		return as;
	}

	// int foo(int z) {
	//     k = z;
	//     return k;
	// }
	// int main() {
	//   int x;
	//   int y;
	//   y = foo(2);
	//   x = foo(3);
	//   assert(x== 3 && y==2);
	// }
	AEState AbstractExecutionMgr::test7() {
		AEState as;
		NodeID x = getNodeID("x");
		NodeID y = getNodeID("y");
		/// TODO: your code starts from here


		return as;
	}

	// int main() {
	//    int x;
	//    x=20;
	//    while(x>0) {
	//        x--;
	//    }
	//    assert(x == 0);
	//    return 0;
	// }
	AEState AbstractExecutionMgr::test8() {



		//+---------+
		//| entry   |
		//| int x;  |
		//| x = 20  |
		//+---+-----+
		//    |
		//    v
		//+-----------+
		//|   head    | <-------+
		//|   x>0?    |         |
		//+---+------++         |
		//    |      |          |
		//  (x<=0)  (x>0)       |
		//    v      v          |
		//+-------+  +------+   |
		//| exit  |  | body |---|
		//|       |  |  x-- |
		//+---+---+  +------+



		AEState entry_as;
		AEState cur_head_as;
		AEState body_as;
		AEState exit_as;
		u32_t widen_delay = 3;
		NodeID x = getNodeID("x");
		/// TODO: your code starts from here

		return exit_as;
	}

} // namespace SVF