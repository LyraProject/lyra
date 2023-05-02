#include <gtest/gtest.h>

extern "C" {
#include <lyra_slab.h>
}

TEST(lyra_slab, init_normal) {
    const lyra_slab_init_args args = {
        .slab_size = sizeof(int),
        .slab_count = 30,
        .original_malloc = malloc,
        .original_free = free,
    };
    lyra_slab_ctx* ctx = lyra_slab_init(&args);
    EXPECT_NE(ctx, nullptr);
    lyra_slab_deinit(ctx);
}

TEST(lyra_slab, init_bad_size) {
    const lyra_slab_init_args args = {
        .slab_size = 0,
        .slab_count = 30,
        .original_malloc = malloc,
        .original_free = free,
    };
    lyra_slab_ctx* ctx = lyra_slab_init(&args);
    EXPECT_EQ(ctx, nullptr);
}

TEST(lyra_slab, init_bad_count) {
    const lyra_slab_init_args args = {
        .slab_size = sizeof(int),
        .slab_count = 0,
        .original_malloc = malloc,
        .original_free = free,
    };
    lyra_slab_ctx* ctx = lyra_slab_init(&args);
    EXPECT_EQ(ctx, nullptr);
}

TEST(lyra_slab, init_null_args) {
    lyra_slab_ctx* ctx = lyra_slab_init(nullptr);
    EXPECT_EQ(ctx, nullptr);
}

TEST(lyra_slab, init_bad_alloc) {
    const lyra_slab_init_args args = {
        .slab_size = sizeof(int),
        .slab_count = 30,
        .original_malloc = nullptr,
        .original_free = free,
    };
    lyra_slab_ctx* ctx = lyra_slab_init(&args);
    EXPECT_EQ(ctx, nullptr);
}

TEST(lyra_slab, init_bad_dealloc) {
    const lyra_slab_init_args args = {
        .slab_size = sizeof(int),
        .slab_count = 30,
        .original_malloc = malloc,
        .original_free = nullptr,
    };
    lyra_slab_ctx* ctx = lyra_slab_init(&args);
    EXPECT_EQ(ctx, nullptr);
}

TEST(lyra_slab, alloc_dealloc_single) {
    const lyra_slab_init_args args = {
        .slab_size = sizeof(int),
        .slab_count = 30,
        .original_malloc = malloc,
        .original_free = free,
    };
    lyra_slab_ctx* ctx = lyra_slab_init(&args);
    EXPECT_NE(ctx, nullptr);

    void* ptr = lyra_slab_alloc(ctx, sizeof(int));
    EXPECT_NE(ptr, nullptr);
    lyra_slab_free(ctx, ptr);

    lyra_slab_deinit(ctx);
}

TEST(lyra_slab, alloc_full) {
    const lyra_slab_init_args args = {
        .slab_size = sizeof(int),
        .slab_count = 30,
        .original_malloc = malloc,
        .original_free = free,
    };
    lyra_slab_ctx* ctx = lyra_slab_init(&args);
    EXPECT_NE(ctx, nullptr);

    for (size_t i = 0; i < 30; ++i) {
        void* ptr = lyra_slab_alloc(ctx, sizeof(int));
        EXPECT_NE(ptr, nullptr);
    }
    void* ptr = lyra_slab_alloc(ctx, sizeof(int));
    EXPECT_EQ(ptr, nullptr);

    lyra_slab_deinit(ctx);
}

TEST(lyra_slab, alloc_free_1000) {
    const lyra_slab_init_args args = {
        .slab_size = sizeof(int),
        .slab_count = 30,
        .original_malloc = malloc,
        .original_free = free,
    };
    lyra_slab_ctx* ctx = lyra_slab_init(&args);
    EXPECT_NE(ctx, nullptr);

    for (size_t i = 0; i < 1000; ++i) {
        void* ptr = lyra_slab_alloc(ctx, sizeof(int));
        EXPECT_NE(ptr, nullptr);
        lyra_slab_free(ctx, ptr);
    }

    lyra_slab_deinit(ctx);
}
