#include <stdio.h>
#include <stdint.h> // For int64_t
#include <stdlib.h> // For exit and EXIT_FAILURE

// Helper function to print int64_t values
void printLongLongLine(int64_t value) {
    printf("%lld\n", value);
}

void CWE121_Stack_Based_Buffer_Overflow__CWE805_int64_t_declare_loop_01_bad() {
    int64_t * data;
    int64_t dataBadBuffer[50];
    int64_t dataGoodBuffer[100]; // This line remains unused in this snippet

    // FLAW: Set a pointer to a "small" buffer
    data = dataBadBuffer;

    int64_t source[100] = {0}; // Fill with 0's
    size_t i;

    // POTENTIAL FLAW: Possible buffer overflow if data < 100
    for (i = 0; i < 100; i++) {
        data[i] = source[i]; // Unsafe memory access
    }

    // Simulating buffer access at edge for demonstration
    printLongLongLine(data[0]); // Safe access within bounds of dataBadBuffer
    printLongLongLine(data[49]); // Safe access within bounds of dataBadBuffer
    printLongLongLine(data[50]); // Unsafe access, just out of bounds of dataBadBuffer
}

int main() {
    CWE121_Stack_Based_Buffer_Overflow__CWE805_int64_t_declare_loop_01_bad();
    return 0;
}