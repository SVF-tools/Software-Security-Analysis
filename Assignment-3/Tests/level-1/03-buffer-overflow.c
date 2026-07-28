extern void UNSAFE_BUFACCESS(void *, unsigned long);

int main(void) {
    char buf[1] = {0};
    volatile int observed = *(volatile int *)(void *)buf;
    (void)observed;
    UNSAFE_BUFACCESS((void *)buf, sizeof(int));
    return 0;
}
