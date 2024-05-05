    //
// Created by Jiawei Wang on 1/17/22.
//

#include "stdbool.h"
extern void svf_assert(bool);
#define LEN 10

int main() {
    int a[LEN] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    int *p  = a + 9;
    *p = 10;
    svf_assert(a[LEN-1] == 10);
    return 0;
}
