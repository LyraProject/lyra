#include "lyra_cbuf.h"
#include "lyra_common.h"
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct test {
    int32_t a;
    int32_t b;
    int32_t c;
} test;

int main(void) {
    const lyra_cbuf_init_args args = {
        .element_size = sizeof(test),
        .count = 50,
        .alloc = malloc,
        .dealloc = free,
    };
    lyra_cbuf* cb = lyra_cbuf_init(&args);
    if (!cb) {
        return 1;
    }

    test t = { .a = INT32_MIN, .b = 5, .c = INT32_MAX };

    for (size_t i = 0; i < 1000; ++i) {
        lyra_cbuf_push(cb, &t);
    }

    test t2;
    while(!lyra_cbuf_is_empty(cb)) {
        lyra_cbuf_pop(cb, &t2);
        assert(t2.a == INT32_MIN);
        assert(t2.b == 5);
        assert(t2.c == INT32_MAX);
    }

    lyra_cbuf_deinit(cb);
}
