#pragma once

/// @file
/// Describes a circular buffer (FIFO).

#include "lyra_common.h"

#include <stdbool.h>

typedef struct lyra_cbuf lyra_cbuf;

typedef struct lyra_cbuf_init_args {
    /// Maximum size for the circular buffer.
    /// Does not grow, so this needs to be large enough to hold
    /// all elements. Once full, old elements will be overridden.
    size_t count;
    /// Size of one element in the buffer.
    size_t element_size;
    /// Used for allocating any memory for the buffer.
    lyra_malloc_fn alloc;
    /// Used to free the memory on deinit.
    lyra_free_fn dealloc;
} lyra_cbuf_init_args;

/// Allocates and initializes a lyra circular buffer.
/// Returns NULL if the arguments are invalid or allocation failed.
lyra_cbuf* lyra_cbuf_init(const lyra_cbuf_init_args* args);

void lyra_cbuf_push(lyra_cbuf* cb, const void* value);

void lyra_cbuf_pop(lyra_cbuf* cb, void* out_value);

bool lyra_cbuf_is_empty(lyra_cbuf* cb);

bool lyra_cbuf_is_full(lyra_cbuf* cb);

/// Frees the buffer and all memory it holds.
/// Set the buffer to NULL after to avoid mistakes.
void lyra_cbuf_deinit(lyra_cbuf* buf);

