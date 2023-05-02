#include "lyra_tsque.h"
#include "lyra_cbuf.h"
#include "lyra_common.h"

#include <threads.h>

typedef struct lyra_tsque {
    mtx_t mtx;
    cnd_t cnd;
    lyra_cbuf* cbuf;
} lyra_tsque;

lyra_tsque* lyra_tsque_init(const lyra_tsque_init_args* args) {
    if (!args || !args->alloc || !args->dealloc) {
        return NULL;
    }

    lyra_tsque* q = (lyra_tsque*)args->alloc(sizeof(lyra_tsque));
    if (!q) {
        return NULL;
    }

    int ret = mtx_init(&q->mtx, mtx_plain);
    if (ret != thrd_success) {
        args->dealloc(q);
        return NULL;
    }

    ret = cnd_init(&q->cnd);
    if (ret != thrd_success) {
        mtx_destroy(&q->mtx);
        args->dealloc(q);
        return NULL;
    }

    const lyra_cbuf_init_args cbuf_args = {
        .element_size = args->element_size,
        .count = args->count,
        .dealloc = args->dealloc,
        .alloc = args->alloc,
    };

    q->cbuf = lyra_cbuf_init(&cbuf_args);
    if (!q->cbuf) {
        cnd_destroy(&q->cnd);
        mtx_destroy(&q->mtx);
        args->dealloc(q);
        return NULL;
    }

    return q;
}

void lyra_tsque_deinit(lyra_tsque* q) {
    if (q) {
        mtx_destroy(&q->mtx);
        cnd_destroy(&q->cnd);
        lyra_cbuf_deinit(q->cbuf);
    }
}

void lyra_tsque_push(lyra_tsque* q, const void* data) {
    int ret = mtx_lock(&q->mtx);
    LYRA_ASSERT(ret == thrd_success);
    lyra_cbuf_push(q->cbuf, data);
    ret = mtx_unlock(&q->mtx);
    LYRA_ASSERT(ret == thrd_success);
    ret = cnd_broadcast(&q->cnd);
    LYRA_ASSERT(ret == thrd_success);
}

void lyra_tsque_wait_and_pop(lyra_tsque* q, void* out_value) {
    int ret = mtx_lock(&q->mtx);
    LYRA_ASSERT(ret == thrd_success);

    while (lyra_cbuf_is_empty(q->cbuf)) {
        ret = cnd_wait(&q->cnd, &q->mtx);
        LYRA_ASSERT(ret == thrd_success);
    }

    lyra_cbuf_pop(q->cbuf, out_value);

    ret = mtx_unlock(&q->mtx);
    LYRA_ASSERT(ret == thrd_success);
}

bool lyra_tsque_try_pop(lyra_tsque* q, void* out_value) {
    int ret = mtx_trylock(&q->mtx);
    LYRA_ASSERT(ret != thrd_error);
    if (ret == thrd_success) {
        bool ok = false;
        if (!lyra_cbuf_is_empty(q->cbuf)) {
            lyra_cbuf_pop(q->cbuf, out_value);
            ok = true;
        }
        mtx_unlock(&q->mtx);
        return ok;
    } else {
        return false;
    }
}
