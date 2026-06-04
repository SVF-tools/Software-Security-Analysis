extern void UNSAFE_PTRDEREF(void *);

int main(void) {
    int *p = (int *)0;
    UNSAFE_PTRDEREF(p);
    return 0;
}
