#include "stdbool.h"
extern void svf_assert(bool);

int main(){
    int x;
    x=1;
    while(x<5) {
        x++;
        if(x==3) break;
    }
    svf_assert(x == 3);
    return 0;
}

