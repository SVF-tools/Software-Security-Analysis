#include <stdio.h>
#include <stdint.h> // For int64_t
#include <stdlib.h> // For exit and EXIT_FAILURE
extern void OVERFLOW(void* data, int size);

void CWE121_Stack_Based_Buffer_Overflow__CWE805_int64_t_declare_loop_01_bad() {
    int64_t dataBadBuffer[50];

    int64_t source[100] = {0}; // Fill with 0's
    size_t i;

    // POTENTIAL FLAW: Possible buffer overflow if data < 100
    OVERFLOW(dataBadBuffer, 100 * sizeof(int64_t));
    for (i = 0; i < 100; i++) {
        dataBadBuffer[i] = source[i]; // Unsafe memory access
    }
}

int main() {
    CWE121_Stack_Based_Buffer_Overflow__CWE805_int64_t_declare_loop_01_bad();
    return 0;
}