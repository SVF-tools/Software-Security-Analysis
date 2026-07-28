#include <stdbool.h>
#include <stddef.h>

extern void svf_assert(bool);
extern char *strcpy(char *, const char *);
extern size_t strlen(const char *);

int main(void) {
    char source[3] = {'a', 'b', '\0'};
    char destination[8] = {0, 0, 9, 9, 0, 0, 0, 0};

    strcpy(destination, source);
    svf_assert(destination[0] == 'a');
    svf_assert(destination[1] == 'b');
    svf_assert(destination[2] == '\0');
    svf_assert(strlen(source) == 2);
    return 0;
}
