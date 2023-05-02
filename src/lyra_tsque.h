#pragma once

#include "lyra_common.h"
#include <stdbool.h>

typedef struct lyra_tsque lyra_tsque;

/// Arguments for lyra_tsque_init
typedef struct lyra_tsque_init_args {
    /// How many elements the queue should be able to hold.
    size_t count;
    /// Size of the elements in the queue.
    size_t element_size;
    /// Allocation function, for example `malloc()`, used for the
    /// initial allocation.
    lyra_malloc_fn alloc;
    /// Deallocation/free function, for example `free()`.
    lyra_free_fn dealloc;
} lyra_tsque_init_args;

/// Initializes the threadsafe queue.
///
/// Returns NULL on error, for example if it fails to allocate,
/// failes to create a mutex, or similar.
lyra_tsque* lyra_tsque_init(const lyra_tsque_init_args* args);

/// Frees all memory and deinitializes the state of the threadsafe queue.
/// Ensure that no `wait_and_pop()` or other similar lock is acquired.
void lyra_tsque_deinit(lyra_tsque*);

/// Pushes an element into the queue and notifies consumers.
void lyra_tsque_push(lyra_tsque*, const void* data);

/// Pushes an element into the queue and notifies consumers.
/// If the queue is full, returns false and does not push anything.
/// If the queue has space, returns true and pushes the element.
bool lyra_tsque_try_push(lyra_tsque*, const void* data);

/// Pops an element from the queue as soon as one is available.
/// Blocks until one is made available (via _push())
void lyra_tsque_wait_and_pop(lyra_tsque*, void* data);

/// Pops an element if one is available and returns true, otherwise returns false.
/// Never blocks.
bool lyra_tsque_try_pop(lyra_tsque*, void* data);
