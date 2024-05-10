// LICENSE
// See end of the file for license information.

#ifndef RIT_STR_INCLUDED
#define RIT_STR_INCLUDED

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#define DEFAULT_STRING_CAP 16

typedef struct rit_str_allocator rit_str_allocator;
typedef struct rit_str rit_str;
typedef struct rit_str_view rit_str_view;

/// @brief Custom allocator interface.
/// Functions allocating memory takes a custom allocator based off this
/// interface as a parameter.
struct rit_str_allocator {
  void *(*alloc)(void *, size_t);
  void (*free)(void *, void *);
  void *(*realloc)(void *, void *, size_t, size_t);
  void *m_ctx;  // The arena, stack or etc where the memory would be allocated,
                // NULL if none
};

// Disable warning C4200: nonstandard extension used: zero-sized array in
// struct/union in MSVC
#pragma warning(push)
#pragma warning(disable : 4200)

/// @brief A string structure holding a string and it's length
struct rit_str {
  size_t m_size;
  size_t m_capacity;
  char m_str[];
};

// Enable the warning again
#pragma warning(pop)

/// @brief A string view structure
/// A string view is a non owning reference to a string.
/// Which means this string view holds a reference to a
/// string only, it does not copy the string and allocate
/// memory.
struct rit_str_view {
  size_t m_size;
  size_t m_index;
  char const *m_str;
};

/// @brief Allocates a new string
/// @param t_str The string to be allocated
/// @param t_allocator Custom allocator for the function to use
/// @return rit_str*
#define rit_str_create(t_str, t_allocator) \
  rit_str_create_with_location(__FILE__, __LINE__, t_str, t_allocator)

/// @internal
rit_str *rit_str_create_with_location(const char *t_file, int t_line,
                                      const char *t_str,
                                      rit_str_allocator *t_allocator);

/// @brief Allocate some space for a new string
/// @param t_capacity The capacity of the rit_str
/// @param t_allocator Custom allocator for the function to use
/// @return rit_str*
#define rit_str_alloc(t_capacity, t_allocator) \
  rit_str_alloc_with_location(__FILE__, __LINE__, t_capacity, t_allocator)

/// @internal
rit_str *rit_str_alloc_with_location(const char *t_file, int t_line,
                                     size_t t_capacity,
                                     rit_str_allocator *t_allocator);

/// @brief Reallocate some space for an existing string
/// @param t_rit_str Pointer to the rit_str
/// @param t_capacity The new set capacity
/// @param t_allocator Custom allocator for the function to use
/// @return void
#define rit_str_realloc(t_rit_str, t_capacity, t_allocator)                \
  rit_str_realloc_with_location(__FILE__, __LINE__, t_rit_str, t_capacity, \
                                t_allocator)

/// @internal
void rit_str_realloc_with_location(const char *t_file, int t_line, rit_str **t_rit_str, size_t t_capacity,
                     rit_str_allocator *t_allocator);

/// @brief Copy a c string to rit_str
/// @param t_rit_str
/// @param t_str The c string
/// @param t_allocator Custom allocator for the function to use
/// @return void
#define rit_str_set(t_rit_str, t_str, t_allocator)                 \
  do {                                                             \
    if (t_rit_str->m_capacity < strlen(t_str)) {                   \
      rit_str_realloc(&t_rit_str, strlen(t_str) * 2, t_allocator); \
      t_rit_str->m_size = strlen(t_str);                           \
    }                                                              \
    size_t i;                                                      \
                                                                   \
    for (i = 0; i < strlen(t_str); ++i) {                          \
      t_rit_str->m_str[i] = t_str[i];                              \
    }                                                              \
    t_rit_str->m_str[i] = '\0';                                    \
  } while (0)

/// @brief Concatenate two strings
/// @param t_rit_str_1
/// @param t_rit_str_2
/// @param t_allocator Custom allocator for the function to use
/// @return rit_str*
rit_str *rit_str_concat(rit_str const *t_rit_str_1, rit_str const *t_rit_str_2,
                        rit_str_allocator *t_allocator);

/// @brief Free a rit_str
/// @param t_rit_str
/// @param t_allocator Custom allocator for the function to use
inline void rit_str_free(rit_str *t_rit_str, rit_str_allocator *t_allocator) {
  t_allocator->free(t_allocator->m_ctx, t_rit_str);
}

/// @brief Create a string view of a string or substring
/// A string view is a non owning reference to a string.
/// Which means this string view holds a reference to a
/// string only, it does not copy the string and allocate
/// memory.
/// A substring is a portion of a bigger string, with this
/// function you can create a string view not of the
/// whole string but substring.
/// @param t_str The string
/// @param t_index Starting index of the string
/// @param t_size Length of the string
/// @return rit_str_view
#define rit_str_view_create(t_str, t_index, t_size) \
  rit_str_view_create_with_location(__FILE__, __LINE__, t_str, t_index, t_size)

/// @internal
rit_str_view rit_str_view_create_with_location(const char *t_file, int t_line,
                                               const char *t_str,
                                               size_t t_index, size_t t_size);

/// @brief Prints the string or substring that has been assigned to the string
/// view
/// @param t_rit_str_view The string view
/// @return void
void rit_str_view_print(const rit_str_view *const t_rit_str_view);
/// @brief Concatenate two strings from two string views
/// @param t_rit_str_view_1
/// @param t_rit_str_view_2
/// @param t_allocator Custom allocator for the function to use
/// @return rit_str*
rit_str *rit_str_view_concat(rit_str_view const *t_rit_str_view_1,
                             rit_str_view const *t_rit_str_view_2,
                             rit_str_allocator *t_allocator);

#ifdef RIT_STR_IMPLEMENTATION

#include <string.h>

rit_str *rit_str_alloc_with_location(const char *t_file, int t_line,
                                     size_t t_capacity,
                                     rit_str_allocator *t_allocator) {
  rit_str *result = (rit_str *)t_allocator->alloc(
      t_allocator->m_ctx, sizeof(rit_str) + t_capacity + 1);
  if (!result) {
    fprintf(stderr, "String allocation failed, file: %s, line: %d\n", t_file,
            t_line);
    exit(1);
  }
  result->m_capacity = t_capacity;
  return result;
}

void rit_str_realloc_with_location(const char *t_file, int t_line, rit_str **t_rit_str, size_t t_capacity,
                     rit_str_allocator *t_allocator) {
  if (t_capacity > (*t_rit_str)->m_capacity) {
    *t_rit_str = t_allocator->realloc(t_allocator->m_ctx, *t_rit_str,
                                      (*t_rit_str)->m_capacity, t_capacity);
    if (!(*t_rit_str)) {
      fprintf(stderr, "String reallocation failed, file: %s, line: %d\n", t_file, t_line);
      exit(1);
    }
    (*t_rit_str)->m_capacity = t_capacity;
  }
}

rit_str *rit_str_create_with_location(const char *t_file, int t_line,
                                      const char *t_str,
                                      rit_str_allocator *t_allocator) {
  size_t capacity = DEFAULT_STRING_CAP;
  size_t size = strlen(t_str);
  if (size >= capacity) {
    capacity = size * 2;
  }

  rit_str *result =
      rit_str_alloc_with_location(t_file, t_line, capacity, t_allocator);
  result->m_size = size;
  rit_str_set(result, t_str, t_allocator);

  return result;
}

rit_str *rit_str_concat(rit_str const *t_rit_str_1, rit_str const *t_rit_str_2,
                        rit_str_allocator *t_allocator) {
  size_t capacity = DEFAULT_STRING_CAP;
  size_t size = t_rit_str_1->m_size + t_rit_str_2->m_size;
  if (size >= capacity) {
    capacity = size * 2;
  }
  rit_str *result = rit_str_alloc(capacity, t_allocator);
  result->m_size = size;
  size_t index = 0;

  for (size_t i = 0; i < t_rit_str_1->m_size; ++i, ++index) {
    result->m_str[index] = t_rit_str_1->m_str[i];
  }
  for (size_t i = 0; i < t_rit_str_2->m_size; ++i, ++index) {
    result->m_str[index] = t_rit_str_2->m_str[i];
  }
  result->m_str[index] = '\0';
  return result;
}

rit_str_view rit_str_view_create_with_location(const char *t_file, int t_line,
                                               const char *t_str,
                                               size_t t_index, size_t t_size) {
  if (t_size > strlen(t_str)) {
    fprintf(
        stderr,
        "The string is bigger than the specified size, file: %s, line: %d\n",
        t_file, t_line);
    exit(1);
  }
  rit_str_view result;
  result.m_index = t_index;
  result.m_size = t_size;
  result.m_str = t_str;
  return result;
}

rit_str *rit_str_view_concat(rit_str_view const *t_rit_str_view_1,
                             rit_str_view const *t_rit_str_view_2,
                             rit_str_allocator *t_allocator) {
  size_t capacity = DEFAULT_STRING_CAP;
  size_t size = t_rit_str_view_1->m_size + t_rit_str_view_2->m_size;
  if (size >= capacity) {
    capacity = size * 2;
  }
  rit_str *result = rit_str_alloc(capacity, t_allocator);
  result->m_size = size;
  size_t index = 0;

  for (size_t i = 0; i < t_rit_str_view_1->m_size; ++i, ++index) {
    result->m_str[index] = t_rit_str_view_1->m_str[i];
  }
  for (size_t i = 0; i < t_rit_str_view_2->m_size; ++i, ++index) {
    result->m_str[index] = t_rit_str_view_2->m_str[i];
  }
  result->m_str[index] = '\0';
  return result;
}

void rit_str_view_print(const rit_str_view *const t_rit_str_view) {
  size_t index = t_rit_str_view->m_index;

  for (size_t i = 0; i < t_rit_str_view->m_size; ++index, ++i) {
    printf("%c", t_rit_str_view->m_str[index]);
  }
}

#endif  // RIT_STR_IMPLEMENTATION
#endif  // RIT_STR_INCLUDED

/*
The MIT License (MIT)

Copyright 2024 Ritchiel Reza

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the “Software”), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
