extern void UNSAFE_BUFACCESS(void *, unsigned long);

int main(void) {
    char buf[1] = {0};
    volatile char *unsafe = &buf[1];
    volatile char observed = *unsafe;
    (void)observed;
    UNSAFE_BUFACCESS((void *)unsafe, 1);
    return 0;
}
