/*
  A simple test of the library
 */
#define J_IMPLEMENTATION
#include "j/j.h"

#include "utest/utest.h"

static inline void*
TEST_ALLOC(size_t size) {
    return malloc(size);
}

static inline void
TEST_FREE(void* ptr, size_t size) {
    j_unused(size);

    free(ptr);
}

static j_allocator_t GPA = {.alloc = TEST_ALLOC, .free = TEST_FREE, .free_all = NULL};

UTEST(j_vec, reserve) {
    int* v = j_vec_init(int, &GPA);

    j_vec_reserve_exact(v, &GPA, 100);
    ASSERT_EQ(j_vec_capacity(v), 100);

    j_vec_reserve(v, &GPA, 100);
    ASSERT_EQ(j_vec_capacity(v), 200);

    j_vec_deinit(v, &GPA);
}

UTEST(j_vec, insert_remove) {
    int* v = j_vec_init(int, &GPA);

    j_vec_insert_last(v, &GPA, 1);
    ASSERT_EQ(j_vec_capacity(v), 1);

    j_vec_insert(v, &GPA, 0, 2);
    ASSERT_EQ(j_vec_capacity(v), 2);
    ASSERT_EQ(v[0], 2);
    ASSERT_EQ(v[1], 1);

    ASSERT_EQ(j_vec_len(v), 2);

    j_vec_clear(v);

    for (int i = 0; i < 10; ++i) {
        j_vec_insert_last(v, &GPA, i);
    }

    ASSERT_EQ(j_vec_len(v), 10);

    j_vec_remove_last(v);
    j_vec_remove(v, 2);

    ASSERT_EQ(j_vec_len(v), 8);
    ASSERT_EQ(v[2], 3);

    j_vec_deinit(v, &GPA);
}

UTEST_MAIN();
