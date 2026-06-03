extern void UNSAFE_LOAD(void *);

int main(void) {
    int *p = (int *)0;
    UNSAFE_LOAD(p);
    return 0;
}
