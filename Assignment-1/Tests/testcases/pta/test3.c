extern void MAYALIAS(void* p, void* q);

void foo(int *m, int *n)
{
    MAYALIAS(m,n);
    int x, y;
    x = *n;
    y = *m;
    *m = x;
    *n = y;
}

int main()
{
    int *p, *q;
    int a, b, c;
    if (c) {
        p = &a;
        q = &b;
        foo(p,q);
    }
    else {
        p = &b;
        q = &c;
        foo(p,q);
    }
    return 0;
}
