extern void UNSAFE_PTRDEREF(void *);

int main(void) {
    volatile int *ptr = (volatile int *)0;
    volatile int observed = *ptr;
    (void)observed;
    UNSAFE_PTRDEREF((void *)ptr);
    return 0;
}
