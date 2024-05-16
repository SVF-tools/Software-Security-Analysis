#include <stdio.h>
#include <stdlib.h> // For rand() and srand()
#include <time.h>   // For time()
extern void OVERFLOW(void* data, int size);


void CWE121_Stack_Based_Buffer_Overflow__CWE129_rand_01_bad() {
    int data;
    /* Initialize data */
    data = -1;
    /* POTENTIAL FLAW: Set data to a random value */
    srand((unsigned)time(NULL)); // Seed the random number generator
    data = rand() % 100; // Simulating RAND32() to generate a value between 0 and 99

    int i;
    int buffer[10] = { 0 };
    /* POTENTIAL FLAW: Attempt to write to an index of the array that is above the upper bound
    * This code does check to see if the array index is negative */
    if (data >= 0)
    {
        OVERFLOW(buffer, 99 * sizeof(int));
        buffer[data] = 1;
    }
}

int main() {
    CWE121_Stack_Based_Buffer_Overflow__CWE129_rand_01_bad();
    return 0;
}
