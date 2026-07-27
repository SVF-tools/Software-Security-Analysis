extern void UNSAFE_BUFACCESS(void *, unsigned long);

int main(void) {
    char buf[4] = {0};
    char *q = &buf[4];
    UNSAFE_BUFACCESS(q, sizeof(*q));
    return 0;
}
