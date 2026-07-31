extern void UNSAFE_PTRDEREF(void *);

int main(void) {
    UNSAFE_PTRDEREF((void *)0);
    return 0;
}
