#include <stdbool.h>
#include <stddef.h>

extern void svf_assert(bool);
extern void *memcpy(void *, const void *, size_t);

int main(void) {
    unsigned char source[2] = {7, 9};
    unsigned char destination[2] = {0, 0};

    memcpy(destination, source, 2);
    svf_assert(destination[0] == 7);
    svf_assert(destination[1] == 9);
    return 0;
}
