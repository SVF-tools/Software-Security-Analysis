#include <stdbool.h>
extern void sink();
void source(int *s){
    sink();
};

int main(){
    int a = 1;
    while ( a <=1){
        source(&a);
        a++;
    }
    return 0;
};