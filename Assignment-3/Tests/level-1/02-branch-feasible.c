#include <stdbool.h>
extern void svf_assert(bool);
extern int nd(void);

int main(void) {
    int x = nd();
    if (x < 10) {
        svf_assert(x <= 9);
    }
    return 0;
}
