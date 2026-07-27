#include <stdbool.h>
#include <string.h>
extern void svf_assert(bool);

int main(void) {
    char src[4] = {'a', 'b', 'c', 0};
    char dst[4] = {0};
    memcpy(dst, src, 4);
    svf_assert(dst[2] == 'c');
    return 0;
}
