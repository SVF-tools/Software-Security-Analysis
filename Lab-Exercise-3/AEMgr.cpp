#include "AEMgr.h"

namespace SVF {

u32_t AbstractExecutionMgr::currentExprIdx = 0;

//    int main() {
//        int a;  ID 0
//        int b;  ID 1
//        a = 0;
//        b = a + 1;
//        assert(b>0);
//    }
void AbstractExecutionMgr::test1() {

    /// TODO: your code starts from here
}


//        int main() {
//        int* p;
//        int q;
//        int b;
//        int a;
//        p = &a;
//        *p = 0;
//        q = *p;
//        *p = 3;
//        b = *p + 1;
//        assert(b>3);
//    }
void AbstractExecutionMgr::test2() {

    /// TODO: your code starts from here
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
void AbstractExecutionMgr::test3() {

    /// TODO: your code starts from here
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
void AbstractExecutionMgr::test4() {    //    int main() {

    /// TODO: your code starts from here
}


//// Struct and pointers
//
//    struct A{ int f0; int* f1;};
//    int main() {
//       struct A* p;
//       int a;
//       int* x;
//       int* q;
//       int** r;
//       int* y;
//       int z;
//
//       p = malloc;
//       x = &a;
//       *x = 5;
//       q = &(p->f0);
//       *q = 10;
//       r = &(p->f1);
//       *r = x;
//       y = *r;
//       z = *q + *y;
//       assert(z==15);
void AbstractExecutionMgr::test5() {


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
void AbstractExecutionMgr::test6() {

    /// TODO: your code starts from here

}


//int foo(int z) {
//    k = z;
//    return k;
//}
//int main() {
//  int x;
//  int y;
//  y = foo(2);
//  x = foo(3);
//  assert(x== 3 && y==2);
//}
void AbstractExecutionMgr::test7() {

    /// TODO: your code starts from here

}


// int main() {
//    int x;
//    x=0;
//    while(x<20) {
//        x+=2;
//    }
//    assert(x == 20);
//    return 0;
// }
void AbstractExecutionMgr::test8() {

    /// TODO: your code starts from here

}

}