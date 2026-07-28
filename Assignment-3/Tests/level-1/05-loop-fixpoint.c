#include <stdbool.h>

extern void svf_assert(bool);
extern int nd(void);

int main(void) {
    int value = 0;
    while (nd()) {
        value++;
    }
    svf_assert(value >= 0);
    return 0;
}
