#include <string.h>
#include <stdio.h>
#include "stdbool.h"

extern void broadcast(char* num);

char *tgetstr(){
    // e.g. sql injection init
    static char initstr[25] = "select* From City ..";
    return initstr;
}

extern void svf_assert(bool);


int main(){

    // Assignment-1
    char *injection = tgetstr();
    char* s = injection;
    char* b = s;
    broadcast(b);

    // Assignment-2
    int *p;
    int a = 1;
    p = &a;
    *p = 3;
    svf_assert(a == 3);

    // Assignment-3
    int arr[5];
    int i = 0;
    while(i < 5) {
    	arr[++i]=10;
    }

    return 0;
}
