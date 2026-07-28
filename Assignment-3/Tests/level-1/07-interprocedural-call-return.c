#include <stdbool.h>

extern void svf_assert(bool);

static int add_one(int value) {
    return value + 1;
}

int main(void) {
    int result = add_one(41);
    svf_assert(result == 42);
    return 0;
}
