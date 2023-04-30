#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

typedef void* (*lyra_malloc_fn)(size_t);
typedef void (*lyra_free_fn)(void*);

typedef struct lyra_slab_ctx {
    lyra_malloc_fn alloc;
    lyra_free_fn dealloc;
    size_t slab_size;
    size_t slab_count;
    size_t last_free_idx;
    void* mem;
    uint8_t* slabs;
} lyra_slab_ctx;

#include "lyra.h"

lyra_slab_ctx* lyra_slab_init(const lyra_slab_init_args* args) {
    // it is required that the args not be null, and that malloc and free exist
    // in some capacity
    if (!args || !args->original_malloc || !args->original_free) {
        return NULL;
    }
    // allocate the context for returning later
    lyra_slab_ctx* ctx = (lyra_slab_ctx*)args->original_malloc(sizeof(struct lyra_slab_ctx));

    // populate the context
    ctx->alloc = args->original_malloc;
    ctx->dealloc = args->original_free;
    ctx->slab_size = args->slab_size;
    ctx->slab_count = args->slab_count;
    ctx->last_free_idx = 0;

    // allocate memory for future allocations
    ctx->mem = ctx->alloc(ctx->slab_size * ctx->slab_count);
    if (!ctx->mem) {
        return NULL;
    }

    // allocate memory for bookkeeping
    ctx->slabs = (uint8_t*)ctx->alloc(ctx->slab_count);
    if (!ctx->slabs) {
        return NULL;
    }

    // set up bookkeeping - `true` means `free`, so it marks all
    // slabs as free.
    memset(ctx->slabs, true, ctx->slab_count);

    return ctx;
}

void* lyra_slab_alloc(struct lyra_slab_ctx* ctx, size_t n) {
    if (n <= ctx->slab_size) {
        // try a quick search from the last free idx
        for (size_t i = ctx->last_free_idx; i < ctx->slab_count; ++i) {
            if (ctx->slabs[i]) {
                ctx->last_free_idx = (ctx->last_free_idx + 1) % ctx->slab_count;
                ctx->slabs[i] = false;
                return (uint8_t*)ctx->mem + (i * ctx->slab_size);
            }
        }
        // try an exhaustive search of all the places not searched before
        for (size_t i = 0; i < ctx->last_free_idx; ++i) {
            if (ctx->slabs[i]) {
                ctx->last_free_idx = i;
                ctx->slabs[i] = false;
                return (uint8_t*)ctx->mem + (i * ctx->slab_size);
            }
        }
    }
    // nothing found
    return NULL;
}

void lyra_slab_free(struct lyra_slab_ctx* ctx, void* p) {
    if (!p) {
        return;
    }
    size_t index = ((uintptr_t)p - (uintptr_t)ctx->mem) / ctx->slab_size;
    ctx->slabs[index] = true;
    ctx->last_free_idx = index;
}

void lyra_slab_deinit(struct lyra_slab_ctx* ctx) {
    ctx->dealloc(ctx->mem);
    ctx->mem = NULL;
    ctx->dealloc(ctx->slabs);
    ctx->slabs = NULL;
    ctx->dealloc(ctx);
}
