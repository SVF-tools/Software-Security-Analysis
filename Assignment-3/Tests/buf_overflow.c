extern void UNSAFE_BUFACCESS(void *, unsigned int);

int main(void) {
    char buf[4] = {0};
    UNSAFE_BUFACCESS(buf + 4, 1);
    buf[4] = 'x';
    return 0;
}
