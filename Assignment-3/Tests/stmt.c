#include <stdbool.h>
extern void svf_assert(bool);

int main(void) {
    unsigned char c = 255;
    unsigned int wide = c;
    unsigned int result = wide + 1;
    svf_assert(result == 256);
    return 0;
}
