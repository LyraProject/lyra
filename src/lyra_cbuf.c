#include "lyra_cbuf.h"
#include "lyra_common.h"
#include <stdint.h>
#include <string.h>

typedef struct lyra_cbuf {
    uint8_t* data;
    size_t count;
    size_t element_size;
    lyra_free_fn dealloc;
    size_t begin_idx;
    size_t end_idx;
    size_t occupied_count;
} lyra_cbuf;

lyra_cbuf* lyra_cbuf_init(const lyra_cbuf_init_args* args) {
    // all these are basic conditions for the following code
    // to be able to make assumptions (has allocator, etc.)
    if (!args 
        || !args->dealloc 
        || !args->alloc
        || args->count == 0
        || args->element_size == 0) {
        return NULL;
    }
    // allocate struct itself
    lyra_cbuf* cb = (lyra_cbuf*)args->alloc(sizeof(lyra_cbuf));
    if (!cb) {
        return NULL;
    }
    // initialize more struct members before performing more allocations
    // since they're (sort of) needed for them
    cb->count = args->count;
    cb->element_size = args->element_size;
    cb->dealloc = args->dealloc;

    // allocate the data - this is the memory used for the data
    // of the circular buffer.
    cb->data = (uint8_t*)args->alloc(cb->count * cb->element_size);
    if (!cb->data) {
        args->dealloc(cb);
        return NULL;
    }

    // a circular buffer begins with the read- and write-pointers
    // (here called begin and end) at zero
    cb->begin_idx = 0;
    cb->end_idx = 0;
    // empty
    cb->occupied_count = 0;

    return cb;
}

void lyra_cbuf_push(lyra_cbuf* cb, const void* value) {
    // first calculate the "real index" (byte index)
    //     real index = idx * bytes_per_element
    // then copy from `value` to that real index,
    // exactly the amount of bytes that one element is large.
    // we use memmove() because it guarantees that it works even if we
    // copy from overlapping memory.
    memmove(&cb->data[cb->element_size * cb->end_idx], value, cb->element_size);

    // move the end pointer forward.
    // the modulo `%` of the count wraps the end_idx around
    // to 0 once it reaches count.
    // this needs to be done in two separate expressions because
    // of sequencing (see -Wsequence-point).
    ++cb->end_idx;
    cb->end_idx = cb->end_idx % cb->count;
    ++cb->occupied_count;
}

void lyra_cbuf_pop(lyra_cbuf* cb, void* out_value) {
    LYRA_ASSERT(!lyra_cbuf_is_empty(cb));
    // copy from the begin element exactly elemnent_size bytes.
    // see lyra_cbuf_push for more details.
    memmove(out_value, &cb->data[cb->element_size * cb->begin_idx], cb->element_size);

    // increment and wrap around if begin_idx>=count.
    // this needs to be done in two separate expressions because
    // of sequencing (see -Wsequence-point).
    ++cb->begin_idx;
    cb->begin_idx = cb->begin_idx % cb->count;
    --cb->occupied_count;
}

bool lyra_cbuf_is_empty(lyra_cbuf* cb) {
    return cb->occupied_count == 0;
}

bool lyra_cbuf_is_full(lyra_cbuf* cb) {
    return cb->occupied_count == cb->count;
}

void lyra_cbuf_deinit(lyra_cbuf* buf) {
    // allow NULL to be passed, as per C convention
    if (buf) {
        // if, for some reason, the buffer is NULL, also allow that
        if (buf->data) {
            buf->dealloc(buf->data);
        }
        buf->dealloc(buf);
    }
}
