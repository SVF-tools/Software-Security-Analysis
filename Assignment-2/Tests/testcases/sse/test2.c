#include "stdbool.h"
//ADDR, LOAD, STORE, CAST(COPY)
extern void svf_assert(bool);

void foo(int* p) {
    *p = 1;
}

int main() {
    int a = 0;
    foo(&a);
    svf_assert(a == 1);
}