#include <stdbool.h>

extern void svf_assert(bool);
extern void SAFE_BUFACCESS(void *, unsigned long);
extern void SAFE_PTRDEREF(void *);

int main(void) {
    char buffer[2] = {4, 5};
    volatile char byte = buffer[1];
    svf_assert(byte == 5);
    SAFE_BUFACCESS(&buffer[1], 1);

    int value = 8;
    volatile int observed = *(volatile int *)&value;
    svf_assert(observed == 8);
    SAFE_PTRDEREF(&value);
    return 0;
}
