#include <stdbool.h>
void mem_insert(void *buffer, const void *data, int data_size, int position);
extern void OVERFLOW(void* data, int size);
extern void svf_assert(bool condition);
int main() {
    char buffer[10] = {0};
    mem_insert(buffer, "abcdef", 3, 5);
    svf_assert(buffer[5] == 'a');
    svf_assert(buffer[6] == 'b');
    svf_assert(buffer[7] == 'c');
    svf_assert(buffer[8] != 'd');
    return 0;
}