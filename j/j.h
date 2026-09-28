/*

  j.h - Base library for my projects

  MIT License

  Copyright (c) 2026 Yjiro

  Permission is hereby granted, free of charge, to any person obtaining a
  copy of this software and associated documentation files (the "Software"),
  to deal in the Software without restriction, including without limitation
  the rights to use, copy, modify, merge, publish, distribute, sublicense,
  and/or sell copies of the Software, and to permit persons to whom the
  Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
  DEALINGS IN THE SOFTWARE.
 */
#ifndef J_H_
#define J_H_

#ifdef _WIN32
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#endif

#include <assert.h>
#include <string.h>

#ifdef J_BUILD_DLL
#ifdef _MSC_VER
#define J_API declspec(dllexport)
#else
#define J_API __attribute__((visibility("default")))
#endif
#else
#ifdef _MSC_VER
#define J_API declspec(dllimport)
#else
#define J_API
#endif
#endif

#ifdef J_WARN_DEPRECATED
#if defined(__GNUC__) || defined(__clang__)
#define j_deprecated(message) __attribute__((deprecated(message)))
#elif defined(_MSC_VER)
#define j_deprecated(message) __declspec(deprecated(message))
#else
#define j_deprecated(...)
#endif
#else
#define j_deprecated(...)
#endif

#define j_unused(v)         (void)(v)

#define j_arr_len(arr)      (sizeof(arr) / sizeof(arr[0]))
#define j_arr_get(arr, pos) (assert((size_t)pos < j_arr_len(arr)), arr[(size_t)pos])

typedef struct j_allocator {
    void* (*alloc)(size_t size);
    void (*free)(void* ptr, size_t size);
    void (*free_all)();
} j_allocator_t;

/*
  Most of the macro functions here can be evaluated twice
  instead of:
       j_vec_to_base(get_vec())
  do:
       void* vec = get_vec();
       j_vec_to_base(vec);
 */

typedef struct j_vec_base {
    size_t stride;
    size_t capacity;
    size_t len;
} j_vec_base_t;

#define j_vec_to_base(vec)    ((j_vec_base_t*)(vec) - 1)
#define j_vec_from_base(base) (void*)((j_vec_base_t*)(base) + 1)

#define j_vec_stride(vec)     ((vec) ? j_vec_to_base((vec))->stride : (size_t)0)
#define j_vec_capacity(vec)   ((vec) ? j_vec_to_base((vec))->capacity : (size_t)0)
#define j_vec_len(vec)        ((vec) ? j_vec_to_base((vec))->len : (size_t)0)

#define j_vec_is_empty(vec)   (j_vec_len((vec)) == 0)

#define j_vec_set_capacity(vec, v)                                                                                     \
    do {                                                                                                               \
        if ((vec))                                                                                                     \
            j_vec_to_base((vec))->capacity = (v);                                                                      \
    } while (0)
#define j_vec_set_len(vec, v)                                                                                          \
    do {                                                                                                               \
        assert((v) <= j_vec_capacity((vec)));                                                                          \
                                                                                                                       \
        if ((vec))                                                                                                     \
            j_vec_to_base((vec))->len = (v);                                                                           \
    } while (0)

#define j_vec_clear(vec)    j_vec_set_len((vec), 0)

#define j_vec_get(vec, pos) ((vec) && (size_t)(pos) < j_vec_len((vec)) ? &(vec)[(pos)] : NULL)
#define j_vec_first(vec)    ((vec) && j_vec_len((vec)) != 0 ? j_vec_at((vec), 0) : NULL)
#define j_vec_last(vec)     ((vec) && j_vec_len((vec)) != 0 ? j_vec_at((vec), j_vec_len((vec)) - 1) : NULL)

#define j_vec_init_capacity(T, allocator, capacity) ((T*)j_vec__init_capacity(sizeof(T), (allocator), (capacity)))
#define j_vec_init(T, allocator)                    j_vec_init_capacity(T, (allocator), 0)

#define j_vec_ensure_capacity(vec, allocator, v)                                                                       \
    do {                                                                                                               \
        void* tmp__ = j_vec__ensure_capacity((vec), (allocator), (v));                                                 \
        if (tmp__)                                                                                                     \
            (vec) = tmp__;                                                                                             \
    } while (0)

#define j_vec_shrink_to_fit(vec, allocator)    j_vec_ensure_capacity((vec), (allocator), j_vec_len((vec)))

#define j_vec_reserve_exact(vec, allocator, v) j_vec_ensure_capacity((vec), (allocator), j_vec_capacity((vec)) + (v))
#define j_vec_reserve(vec, allocator, v)                                                                               \
    do {                                                                                                               \
        size_t capacity__ = j_vec_capacity((vec));                                                                     \
        size_t req__ = capacity__ + (v);                                                                               \
        if (capacity__ == 0)                                                                                           \
            capacity__ = 1;                                                                                            \
        while (capacity__ < req__)                                                                                     \
            capacity__ <<= 1;                                                                                          \
        j_vec_ensure_capacity((vec), (allocator), capacity__);                                                         \
    } while (0)

#define j_vec_insert_last(vec, allocator, v)                                                                           \
    do {                                                                                                               \
        size_t capacity__ = j_vec_capacity((vec));                                                                     \
        size_t len__ = j_vec_len((vec));                                                                               \
        if (capacity__ <= len__)                                                                                       \
            j_vec_reserve((vec), (allocator), 1);                                                                      \
        (vec)[len__] = (v);                                                                                            \
        j_vec_set_len((vec), len__ + 1);                                                                               \
    } while (0);

#define j_vec_insert(vec, allocator, pos, v)                                                                           \
    do {                                                                                                               \
        size_t capacity__ = j_vec_capacity((vec));                                                                     \
        size_t len__ = j_vec_len((vec));                                                                               \
        if (capacity__ <= len__)                                                                                       \
            j_vec_reserve((vec), (allocator), 1);                                                                      \
        if ((pos) < len__) {                                                                                           \
            memmove((vec) + (pos) + 1, (vec) + (pos), sizeof(*(vec)) * (len__ - (pos)));                               \
        }                                                                                                              \
        (vec)[(pos)] = (v);                                                                                            \
        j_vec_set_len((vec), len__ + 1);                                                                               \
    } while (0)

#define j_vec_remove_last(vec)                                                                                         \
    do {                                                                                                               \
        size_t len__ = j_vec_len((vec));                                                                               \
        if (len__ > 0)                                                                                                 \
            j_vec_set_len((vec), len__ - 1);                                                                           \
    } while (0)

#define j_vec_remove(vec, pos)                                                                                         \
    do {                                                                                                               \
        if (vec) {                                                                                                     \
            size_t len__ = j_vec_len((vec));                                                                           \
            if (len__ > (pos)) {                                                                                       \
                j_vec_set_len((vec), len__ - 1);                                                                       \
                memmove((vec) + (pos), (vec) + (pos) + 1, sizeof(*(vec)) * (len__ - 1 - (pos)));                       \
            }                                                                                                          \
        }                                                                                                              \
    } while (0)

void* j_vec__init_capacity(size_t stride, const j_allocator_t* allocator, size_t capacity);

/*
  NOTE: This will not set the vector to NULL
          and it doesnt free the elements inside of the vector
 */
void j_vec_deinit(void* vec, const j_allocator_t* allocator);
void* j_vec__ensure_capacity(void* vec, const j_allocator_t* allocator, size_t v);

#ifdef J_IMPLEMENTATION

void*
j_vec__init_capacity(size_t stride, const j_allocator_t* allocator, size_t capacity) {
    assert(stride != 0);
    assert(allocator != NULL);

    j_vec_base_t* base = (j_vec_base_t*)allocator->alloc(sizeof(*base) + capacity * stride);
    if (!base) {
        return NULL;
    }

    base->stride = stride;
    base->capacity = capacity;
    base->len = 0;

    return j_vec_from_base(base);
}

void
j_vec_deinit(void* vec, const j_allocator_t* allocator) {
    assert(allocator != NULL);
    if (vec) {
        j_vec_base_t* base = j_vec_to_base(vec);
        allocator->free(base, sizeof(*base) + base->capacity * base->stride);
    }
}

void*
j_vec__ensure_capacity(void* vec, const j_allocator_t* allocator, size_t v) {
    assert(allocator != NULL);
    if (vec) {
        j_vec_base_t* base = j_vec_to_base(vec);
        void* tmp = j_vec__init_capacity(base->stride, allocator, v);
        if (!tmp) {
            return NULL;
        }

        j_vec_set_len(tmp, base->len);
        memcpy(tmp, vec, v * base->stride);
        j_vec_deinit(vec, allocator);
        return tmp;
    }
    return NULL;
}

#endif /* J_IMPLEMENTATION */

#endif /* J_H_ */
