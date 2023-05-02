#include "lyra_tsque.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <threads.h>

/// This example demonstrates the use of `lyra_tsque`.
/// Whatever you enter is echoed from different threads back to the
/// user.
/// Watch the CPU usage while using this example - even though it spawns
/// 10 threads, all of them are waiting (for input or waiting on the queue).

int thread_main(void* data) {
    lyra_tsque* q = (lyra_tsque*)data;

    while (true) {
        char buf[10];
        lyra_tsque_wait_and_pop(q, buf);
        printf("thread %lu got: %s\n", thrd_current(), buf);
    }

    return 0;
}

int main(void) {
    const lyra_tsque_init_args args = {
        .count = 10,
        .element_size = sizeof(const char[10]),
        .alloc = malloc,
        .dealloc = free,
    };
    lyra_tsque* q = lyra_tsque_init(&args);
    if (!q) {
        return 1;
    }

    thrd_t th[10];
    for (size_t i = 0; i < 10; ++i) {
        thrd_create(&th[i], thread_main, q);
    }

    char buf[10];

    printf("enter some text, the thread will print it word by word.\n"
           "watch the cpu usage - the thread is not spinning.\n");

    while (true) {
        memset(buf, 0, sizeof(buf));
        scanf("%9s", buf);
        while (!lyra_tsque_try_push(q, buf)) {
            printf("failed to push '%s': no room in queue! waiting and trying again...\n", buf);
            struct timespec ts;
            ts.tv_sec = 1;
            ts.tv_nsec = 0;
            thrd_sleep(&ts, NULL);
        }
    }

    for (size_t i = 0; i < 10; ++i) {
        thrd_join(th[i], NULL);
    }
    lyra_tsque_deinit(q);
}
