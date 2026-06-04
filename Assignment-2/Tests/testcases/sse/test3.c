#include "stdbool.h"
// CHECK: ^sat$

extern int nd(void);

extern void svf_assert(bool);

int test(int a, int b){
    int x,y;
    x=1; y=1;

    if (a > b) {
        x++;
        y++;
        svf_assert (x == y);
    } else {
        x++;
        svf_assert (x == 2);
    }
    return 0;
}

int main(){
    int a = 1;
    int b = 2;
    test(a,b);
    return 0;
}