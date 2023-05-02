// compile example:
// cc example1.c -llyra_s -Lpath/to/lyra.a -I/path/to/lyra.h -o example1
// OR use cmake with -DLYRA_BUILD_EXAMPLES=ON
//
// this example uselessly allocates integers.
// once no more memory is left in the slab allocator, it errors.
//
// you can use valgrind to check that, despite us not deallocating any of those
// values, they get deallocated by lyra_slab_deinit.

#include <lyra.h>

#include <stdio.h>
#include <stdlib.h>

int main(void) {
    struct lyra_slab_init_args args = {
        .slab_size = sizeof(int), // we want to store integers, so this is enough
        .slab_count = 100, // enough space for 100 integers
        .original_malloc = malloc,
        .original_free = free,
    };
    lyra_slab_ctx* ctx = lyra_slab_init(&args);
    if (!ctx) {
        return 1;
    }
    
    for(;;) {
        printf("enter an integer to allocate memory for it (or non-number to exit): ");
        int value;
        if (scanf("%d", &value) != 1) {
            printf("Exit!\n");
            return 0;
        }
        int* x = (int*)lyra_slab_alloc(ctx, sizeof(int));
        if (!x) {
            printf("no more memory left!\n");
            break;
        }
        *x = value;
        printf("%d stored at %p!\n", value, (void*)x);
    }

    lyra_slab_deinit(ctx);
}

