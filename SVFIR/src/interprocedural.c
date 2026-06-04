#include "stdbool.h"
extern void svf_assert(bool);

int foo(int p) {
    return p;
}
int main(int argc) {
  int x;
  int y;
  x = foo(3); // ctx_7
  y = foo(argc); // ctx_8
  svf_assert(y == argc);    
}
