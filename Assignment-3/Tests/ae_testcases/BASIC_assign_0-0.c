//
// Created by Jiawei Wang on 1/20/22.
//
#include "stdbool.h"

extern void svf_assert(bool);
int main() {
    int a = -1;
    a = a*a;
    svf_assert(a>0);
    return 0;
}
