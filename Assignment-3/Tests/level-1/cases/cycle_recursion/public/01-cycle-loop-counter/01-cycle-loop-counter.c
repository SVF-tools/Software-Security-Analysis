#include <stdbool.h>
extern void svf_assert(bool);

int main(void) {
    int i = 0;
    while (i < 8) {
        i++;
    }
    svf_assert(i >= 8);
    return 0;
}
