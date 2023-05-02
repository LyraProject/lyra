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
        .dealloc = free,
    };
    lyra_cbuf* cb = lyra_cbuf_init(&args);
    EXPECT_EQ(cb, nullptr);
}

TEST(lyra_cbuf, init_no_args) {
    lyra_cbuf* cb = lyra_cbuf_init(nullptr);
    EXPECT_EQ(cb, nullptr);
}

TEST(lyra_cbuf, push_pop_single) {
    const lyra_cbuf_init_args args = {
        .count = 10,
        .element_size = sizeof(int),
        .alloc = malloc,
        .dealloc = free,
    };
    lyra_cbuf* cb = lyra_cbuf_init(&args);
    ASSERT_NE(cb, nullptr);
    int value = rand();
    lyra_cbuf_push(cb, &value);
    int out_value = 0;
    lyra_cbuf_pop(cb, &out_value);
    EXPECT_EQ(value, out_value);
    lyra_cbuf_deinit(cb);
}

TEST(lyra_cbuf, push_pop_multi) {
    const lyra_cbuf_init_args args = {
        .count = 10,
        .element_size = sizeof(int),
        .alloc = malloc,
        .dealloc = free,
    };
    lyra_cbuf* cb = lyra_cbuf_init(&args);
    ASSERT_NE(cb, nullptr);
    for (int i = 0; i < 10; ++i) {
        lyra_cbuf_push(cb, &i);
    }
    int out_value = 0;
    for (int i = 0; i < 10; ++i) {
        lyra_cbuf_pop(cb, &out_value);
        EXPECT_EQ(i, out_value);
    }
    lyra_cbuf_deinit(cb);
}

TEST(lyra_cbuf, is_empty) {
    const lyra_cbuf_init_args args = {
        .count = 10,
        .element_size = sizeof(int),
        .alloc = malloc,
        .dealloc = free,
    };
    lyra_cbuf* cb = lyra_cbuf_init(&args);
    ASSERT_NE(cb, nullptr);
    EXPECT_TRUE(lyra_cbuf_is_empty(cb));
    int value = 1;
    lyra_cbuf_push(cb, &value);
    EXPECT_FALSE(lyra_cbuf_is_empty(cb));
    lyra_cbuf_deinit(cb);
}

TEST(lyra_cbuf, is_full) {
    const lyra_cbuf_init_args args = {
        .count = 10,
        .element_size = sizeof(int),
        .alloc = malloc,
        .dealloc = free,
    };
    lyra_cbuf* cb = lyra_cbuf_init(&args);
    ASSERT_NE(cb, nullptr);
    EXPECT_FALSE(lyra_cbuf_is_full(cb));
    int value = 1;
    for (size_t i = 0; i < 10; ++i) {
        EXPECT_FALSE(lyra_cbuf_is_full(cb));
        lyra_cbuf_push(cb, &value);
    }
    EXPECT_TRUE(lyra_cbuf_is_full(cb));
    lyra_cbuf_deinit(cb);
}

