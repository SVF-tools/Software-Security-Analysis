#include <stdbool.h>

extern void svf_assert(bool);

int main(void) {
    int value = 0;
    while (value < 10) {
        value++;
    }
    svf_assert(value == 10);
    return 0;
}
