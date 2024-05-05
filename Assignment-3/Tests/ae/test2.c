mv #include "stdbool.h"
extern void svf_assert(bool);
int main(int i) {
    int a, c;
    a = i + 1;
    c = a;
    svf_assert( c == i + 1 );
    return 0;
}