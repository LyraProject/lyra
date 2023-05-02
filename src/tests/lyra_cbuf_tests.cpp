#include <gtest/gtest.h>

extern "C" {
#include <lyra_cbuf.h>
}

TEST(lyra_cbuf, init_normal) {
    const lyra_cbuf_init_args args = {
        .count = 10,
        .element_size = sizeof(int),
        .alloc = malloc,
        .dealloc = free,
    };
    lyra_cbuf* cb = lyra_cbuf_init(&args);
    EXPECT_NE(cb, nullptr);
    lyra_cbuf_deinit(cb);
}

TEST(lyra_cbuf, init_bad_count) {
    const lyra_cbuf_init_args args = {
        .count = 0,
        .element_size = sizeof(int),
        .alloc = malloc,
        .dealloc = free,
    };
    lyra_cbuf* cb = lyra_cbuf_init(&args);
    EXPECT_EQ(cb, nullptr);
}

TEST(lyra_cbuf, init_bad_size) {
    const lyra_cbuf_init_args args = {
        .count = 10,
        .element_size = 0,
        .alloc = malloc,
        .dealloc = free,
    };
    lyra_cbuf* cb = lyra_cbuf_init(&args);
    EXPECT_EQ(cb, nullptr);
}

TEST(lyra_cbuf, init_no_alloc) {
    const lyra_cbuf_init_args args = {
        .count = 10,
        .element_size = 0,
        .alloc = nullptr,
        .dealloc = free,
    };
    lyra_cbuf* cb = lyra_cbuf_init(&args);
    EXPECT_EQ(cb, nullptr);
}

TEST(lyra_cbuf, init_no_dealloc) {
    const lyra_cbuf_init_args args = {
        .count = 10,
        .element_size = 0,
        .alloc = malloc,
        .dealloc = nullptr,
    };
    lyra_cbuf* cb = lyra_cbuf_init(&args);
    EXPECT_EQ(cb, nullptr);
}

TEST(lyra_cbuf, init_no_args) {
    lyra_cbuf* cb = lyra_cbuf_init(nullptr);
    EXPECT_EQ(cb, nullptr);
}
