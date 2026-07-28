#define main arvo_min_case_main
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static volatile uint64_t sink;

static void touch_prefix(unsigned char *buf, size_t n) {
    for (size_t i = 0; i < n; ++i) {
        buf[i] = (unsigned char)(i * 17u + 3u);
        sink += buf[i] & 1u;
    }
}

__attribute__((noinline))
static void trigger_42519094(unsigned char *buf, size_t index) {
#line 389 "/src/curl/lib/curl_fnmatch.c"
    sink += buf[index];
}

int main(void) {
    const size_t n = 4;
    unsigned char *buf = (unsigned char *)malloc(n);
    if (!buf) {
        return 2;
    }
    touch_prefix(buf, n);
    trigger_42519094(buf, n);
    printf("%llu\n", (unsigned long long)sink);
    free(buf);
    return 0;
}

#undef main
