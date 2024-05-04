#include "stdbool.h"
//ADDR, LOAD, STORE, CAST(COPY)
extern void svf_assert(bool);

int main() {
    int *p;
    int a = 1;
    p = &a;
    *p = 3;
    svf_assert(a == 3);
}