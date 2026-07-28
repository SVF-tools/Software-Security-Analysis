extern void UNSAFE_BUFACCESS(void *, unsigned long);

int main(void) {
    char buf[1] = {0};
    UNSAFE_BUFACCESS(&buf[1], 1);
    return 0;
}
