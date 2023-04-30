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

/// Initializes a lyra slab allocator context. This context is used
/// for all further operations on the slab allocator.
///
/// Arguments to this function are the lyra_slab_init_args struct,
/// which needs to be supplied (can't be NULL), and has to be filled
/// with valid values.
///
/// From here, all memory the slab allocator will ever need is allocated:
/// - (slab_size * slab_count) bytes of memory for allocations
/// - (slab_count) additional bytes for bookkeeping
/// - sizeof(lyra_slab_ctx) (in the .c file) for the context.
///
/// Keep in mind that this context needs to be freed with
/// lyra_slab_deinit().
///
/// Returns NULL on error. Will fail if the arguments are invalid,
/// or if allocation of the slab memory or bookkeeping memory failed.
lyra_slab_ctx* lyra_slab_init(const struct lyra_slab_init_args*);

/// Frees all memory and cleans up after the allocator.
/// Must be called if lyra_slab_init returned a valid context.
/// May be called with NULL.
void lyra_slab_deinit(lyra_slab_ctx*);

/// Allocates a slab of slab_size. The size argument exists
/// as a fail safe - the allocation *will* fail if the given size
/// is larger than the slab size.
///
/// All allocations are the same size regardless of the size argument.
void* lyra_slab_alloc(lyra_slab_ctx*, size_t size);

/// Frees a section of memory allocated with lyra_slab_alloc().
/// Makes that memory available for the next allocation.
void lyra_slab_free(lyra_slab_ctx*, void* p);
