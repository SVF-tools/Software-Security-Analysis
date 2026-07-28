#include <stdbool.h>

extern void svf_assert(bool);

static void count_down(int value) {
    if (value <= 0) {
        svf_assert(value <= 0);
        return;
    }
    count_down(value - 1);
}

int main(void) {
    count_down(3);
    return 0;
}
