#pragma once

#include <stddef.h>

typedef void* (*lyra_malloc_fn)(size_t);
typedef void (*lyra_free_fn)(void*);

/// You may define your own LYRA_ASSERT at compile-time
/// of the library (later has no effect).
/// This is used for safety checks, for example
/// checking if the circular buffer (lyra_cbuf) has elements
/// before allowing a pop()
#ifndef LYRA_ASSERT
#include <assert.h>
#define LYRA_ASSERT(x) assert(x)
#endif
