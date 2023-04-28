#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef void* (*lyra_malloc_fn)(size_t);
typedef void (*lyra_free_fn)(void*);

#ifndef lyra_IMPL
typedef void lyra_slab_ctx;
#endif

/// Initialization arguments for a slab allocator.
///
/// There are always three levels of slab size.
struct lyra_slab_init_args {
    /// size of the largest possible slab
    size_t slab_size;
    /// how many large slabs to allocate to begin with
    /// this means that large_slab_size * large_slab_count
    /// is the total amount of memory available through
    /// this allocator.
    size_t slab_count;
    /// malloc function to use for the initial allocation.
    lyra_malloc_fn original_malloc;
    /// malloc function to use for the initial allocation.
    lyra_free_fn original_free;
};

lyra_slab_ctx* lyra_slab_init(const struct lyra_slab_init_args*);

void lyra_slab_deinit(lyra_slab_ctx*);

void* lyra_slab_alloc(lyra_slab_ctx*, size_t);

void lyra_slab_free(lyra_slab_ctx*, void* p);
