#include "stdbool.h"
extern void svf_assert(bool);

int main() {
    int x;
    x=1;
    while(x<10000) {
        x++;
    }
    svf_assert(x == 10000);
    return 0;
}