// LICENSE
// See end of the file for license information.
#ifndef RIT_STR_H_INCLUDED
#define RIT_STR_H_INCLUDED

#define _CRT_SECURE_NO_WARNINGS

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEFAULT_STR_CAP 16

///////////////////////////////////////////////////////////////////////////////
/////////////////////////////////DECLARATION///////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/// @brief Custom allocator interface.
/// Functions allocating memory takes a custom allocator based off this
/// interface as a parameter.
typedef struct {
  void *(*alloc)(void *, size_t);
  void (*free)(void *, void *);
  void *(*realloc)(void *, void *, size_t, size_t);
  void *m_ctx;  // The arena, stack or etc where the memory would be allocated,
                // NULL if none
} rit_str_allocator;

/// @brief Holds the metadata about the string.
/// This metadata struct is stored as header of the string.
typedef struct {
  size_t m_size;
  size_t m_capacity;
} rit_str_metadata;

/// @brief Non owning reference to a string
struct rit_str_view {
  size_t m_size;
  char *m_str;
};

/// @brief Get the pointer to the metadata header struct of a string.
/// @param t_rit_str The string
#define rit_str_get_metadata(t_rit_str) (&((rit_str_metadata *)t_rit_str)[-1])

#define rit_str_alloc(t_size, t_allocator) \
  rit_str_alloc_with_location(__FILE__, __LINE__, t_size, t_allocator)

/// @internal
char *rit_str_alloc_with_location(const char *t_file, int t_line, size_t t_size,
                                  rit_str_allocator *t_allocator);

/// @brief Set the capacity of a string.
#define rit_str_reserve(t_rit_str, t_new_capacity, t_allocator) \
  rit_str_realloc(__FILE__, __LINE__, &t_rit_str, t_new_capacity, t_allocator)

/// @internal
void rit_str_realloc(const char *t_file, int t_line, char **t_rit_str,
                     size_t t_new_capacity, rit_str_allocator *t_allocator);

#define rit_str_swap(t_rit_str, t_rit_str_other) \
  do {                                           \
    void *tmp = t_rit_str;                       \
    t_rit_str = t_rit_str_other;                 \
    t_rit_str_other = tmp;                       \
  } while (0)

/// @brief Makes a non-binding request to make the capacity of a string equal to
/// its size. In this library this is definied as a no-op function.
inline void rit_str_shrink_to_fit(char *t_rit_str) { (void)t_rit_str; }

/// @brief Returns a pointer to a null-terminated character array with data
/// equivalent to those stored in the string.
inline const char *rit_str_cstr(char *t_rit_str) {
  const char *retptr = t_rit_str;
  return retptr;
}

/// @brief Returns a pointer to a null-terminated character array with data
/// equivalent to those stored in the string.
inline const char *rit_str_data(char *t_rit_str) {
  return rit_str_cstr(t_rit_str);
}

inline void rit_str_free(char *t_rit_str, rit_str_allocator *t_allocator) {
  t_allocator->free(t_allocator->m_ctx, t_rit_str);
}

inline size_t rit_str_size(char *t_rit_str) {
  return rit_str_get_metadata(t_rit_str)->m_size;
}

inline size_t rit_str_capacity(char *t_rit_str) {
  return rit_str_get_metadata(t_rit_str)->m_capacity;
}

/// @internal
inline bool rit_str_index_bounds_check(const char *t_file, int t_line,
                                       char *t_rit_str, size_t t_index) {
  if (t_index < rit_str_size(t_rit_str)) return true;
  fprintf(stderr, "Error: string index is out of bounds, file: %s, line: %d\n",
          t_file, t_line);
  exit(EXIT_FAILURE);
}

/// @param Check if a string is empty.
inline bool rit_str_empty(char *t_rit_str) {
  return rit_str_size(t_rit_str) == 0;
}

/// @brief Empty out a string.
inline void rit_str_clear(char *t_rit_str) {
  rit_str_get_metadata(t_rit_str)->m_size = 0;
  t_rit_str[0] = '\0';
}

/// @brief Initialize a rit_str.
#define rit_str(t_rit_str, t_cstr, t_allocator)                               \
  char *t_rit_str = rit_str_alloc_with_location(__FILE__, __LINE__,           \
                                                strlen(t_cstr), t_allocator); \
  strcpy(t_rit_str, t_cstr)

#define rit_str_at(t_rit_str, t_index)                                 \
  (rit_str_index_bounds_check(__FILE__, __LINE__, t_rit_str, t_index)) \
      ? t_rit_str[t_index]                                             \
      : t_rit_str[t_index]

#define rit_str_set(t_rit_str, t_index, t_char)                             \
  if (rit_str_index_bounds_check(__FILE__, __LINE__, t_rit_str, t_index)) { \
    t_rit_str[t_index] = t_char;                                            \
  }

/// @brief Get the pointer to the first element of an array
#define rit_str_begin(t_rit_str) (&(t_rit_str[0]))
/// @brief Get the pointer to the past-the-end element of an array
#define rit_str_end(t_rit_str) (&(t_rit_str[rit_str_size(t_rit_str)]))

/// @brief Get the first element of an array
#define rit_str_front(t_rit_str) (t_rit_str[0])
/// @brief Get the last element of an array
#define rit_str_back(t_rit_str) (t_rit_str[rit_str_size(t_rit_str) - 1])

#define rit_str_push_back(t_rit_str, t_char, t_allocator)           \
  t_rit_str[rit_str_size(t_rit_str)] = t_char;                      \
  if (rit_str_capacity(t_rit_str) <= rit_str_size(t_rit_str) + 1) { \
    rit_str_reserve(t_rit_str, (rit_str_size(t_rit_str) + 1) * 2,   \
                    t_allocator);                                   \
  }                                                                 \
  rit_str_get_metadata(t_rit_str)->m_size++;                        \
  t_rit_str[rit_str_size(t_rit_str)] = '\0'

#define rit_str_pop_back(t_rit_str)          \
  rit_str_get_metadata(t_rit_str)->m_size--; \
  t_rit_str[rit_str_size(t_rit_str)] = '\0'

#define rit_str_append_char(t_rit_str, t_count, t_char, t_allocator) \
  for (size_t i = 1; i <= t_count; i++) {                            \
    rit_str_push_back(t_rit_str, t_char, t_allocator);               \
  }

#define rit_str_append_str(t_rit_str, t_cstr, t_allocator) \
  for (size_t i = 0; i < strlen(t_cstr); i++) {            \
    rit_str_push_back(t_rit_str, t_cstr[i], t_allocator);  \
  }

/// @brief Remove elements from the end of the string
#define rit_str_remove(t_rit_str, t_count) \
  for (size_t i = 1; i <= t_count; i++) {  \
    rit_str_pop_back(t_rit_str);           \
  }

/// @brief Changes the number of characters stored
#define rit_str_resize(t_rit_str, t_count, t_char, t_allocator) \
  rit_str_clear(t_rit_str);                                     \
  rit_str_append_char(t_rit_str, t_count, t_char, t_allocator)

/// @brief Insert characters in the array at t_index.
#define rit_str_insert(t_rit_str, t_index, t_count, t_char, t_allocator) \
  do {                                                                   \
    rit_str_append_char(t_rit_str, t_count, t_char, t_allocator);        \
    for (size_t i = rit_str_size(t_rit_str) - 1; i >= t_index + t_count; \
         i--) {                                                          \
      rit_str_set(t_rit_str, i, rit_str_at(t_rit_str, i - t_count));     \
      rit_str_set(t_rit_str, i - t_count, t_char);                       \
    }                                                                    \
  } while (0)

/// @brief Remove characters in the array at t_index.
#define rit_str_erase(t_rit_str, t_index, t_count)                         \
  do {                                                                     \
    for (size_t i = t_index + t_count; i < rit_str_size(t_rit_str); i++) { \
      rit_str_set(t_rit_str, i - t_count, rit_str_at(t_rit_str, i));       \
    }                                                                      \
    rit_str_remove(t_rit_str, t_count);                                    \
  } while (0)

/// @param t_rit_str Where to assign
/// @param t_cstr What to assign
inline void rit_str_assign(char *t_rit_str, char *t_cstr,
                           rit_str_allocator *t_allocator) {
  rit_str_clear(t_rit_str);
  for (size_t i = 0; i < strlen(t_cstr); i++) {
    rit_str_push_back(t_rit_str, t_cstr[i], t_allocator);
  }
}

/// @param t_rit_str Where to copy
/// @param t_rit_str_other What to copy
#define rit_str_copy(t_rit_str, t_index, t_count, t_rit_str_other,            \
                     t_allocator)                                             \
  rit_str_copy_with_location(__FILE__, __LINE__, t_rit_str, t_index, t_count, \
                             t_rit_str_other, t_allocator)

///@internal
void rit_str_copy_with_location(const char *t_file, int t_line, char *t_rit_str,
                                size_t t_index, size_t t_count,
                                char *t_rit_str_other,
                                rit_str_allocator *t_allocator);

/// @param t_index Starting index of the substring
/// @param t_count Number of characters in the substring
#define rit_str_replace(t_rit_str, t_index, t_count, t_cstr, t_allocator) \
  rit_str_replace_with_location(__FILE__, __LINE__, t_rit_str, t_index,   \
                                t_count, t_cstr, t_allocator)

/// @internal
void rit_str_replace_with_location(const char *t_file, int t_line,
                                   char *t_rit_str, size_t t_index,
                                   size_t t_count, const char *t_cstr,
                                   rit_str_allocator *t_allocator);

/// @param t_str A c string or rit_str
#define rit_str_view(t_rit_str_view, t_str) \
  struct rit_str_view t_rit_str_view = {.m_size = strlen(t_str), .m_str = t_str}

#define rit_str_view_size(t_rit_str_view) t_rit_str_view.m_size

/// @internal
bool rit_str_view_index_bounds_check(const char *t_file, int t_line,
                                     struct rit_str_view *t_rit_str_view,
                                     size_t t_index) {
  if (t_index < rit_str_view_size((*t_rit_str_view))) return true;
  fprintf(stderr,
          "Error: string_view index is out of bounds, file: %s, line: %d\n",
          t_file, t_line);
  exit(EXIT_FAILURE);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////IMPLEMENTATION//////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#ifdef RIT_STR_IMPLEMENTATION

char *rit_str_alloc_with_location(const char *t_file, int t_line, size_t t_size,
                                  rit_str_allocator *t_allocator) {
  size_t capacity = DEFAULT_STR_CAP < t_size * 2 ? t_size * 2 : DEFAULT_STR_CAP;
  rit_str_metadata *arr = (rit_str_metadata *)t_allocator->alloc(
      t_allocator->m_ctx, sizeof(rit_str_metadata) + capacity);
  if (!arr) {
    fprintf(stderr, "Error: allocation failed, file: %s, line: %d\n", t_file,
            t_line);
    exit(EXIT_FAILURE);
  }
  arr->m_size = t_size;
  arr->m_capacity = capacity;
  arr += 1;
  return (char *)arr;
}

void rit_str_realloc(const char *t_file, int t_line, char **t_rit_str,
                     size_t t_new_capacity, rit_str_allocator *t_allocator) {
  if (t_new_capacity > rit_str_capacity(*t_rit_str)) {
    rit_str_metadata *arr = (rit_str_metadata *)t_allocator->realloc(
        t_allocator->m_ctx, rit_str_get_metadata(*t_rit_str),
        sizeof(rit_str_metadata) + rit_str_capacity(*t_rit_str),
        sizeof(rit_str_metadata) + t_new_capacity);
    if (!arr) {
      fprintf(stderr, "Error: reallocation failed, file: %s, line: %d\n",
              t_file, t_line);
      exit(EXIT_FAILURE);
    }
    arr += 1;
    *t_rit_str = (char *)arr;
    rit_str_get_metadata(*t_rit_str)->m_capacity = t_new_capacity;
  }
}

void rit_str_copy_with_location(const char *t_file, int t_line, char *t_rit_str,
                                size_t t_index, size_t t_count,
                                char *t_rit_str_other,
                                rit_str_allocator *t_allocator) {
  if (t_index > rit_str_size(t_rit_str_other)) {
    fprintf(stderr,
            "Error: starting index of substring out of bounds of the string, "
            "file: %s, line: %d\n",
            t_file, t_line);
    exit(EXIT_FAILURE);
  } else if (t_count == 0) {
    t_count = rit_str_size(t_rit_str_other) - t_index;
  } else if (t_index + t_count > rit_str_size(t_rit_str_other)) {
    fprintf(stderr,
            "Error: size of substring greater than the string, file: %s, line: "
            "%d\n",
            t_file, t_line);
    exit(EXIT_FAILURE);
  }
  if (rit_str_size(t_rit_str) < t_count) {
    size_t size = t_count - rit_str_size(t_rit_str);
    rit_str_append_char(t_rit_str, size, ' ', t_allocator);
  } else if (rit_str_size(t_rit_str) > t_count) {
    size_t size = rit_str_size(t_rit_str) - t_count;
    rit_str_remove(t_rit_str, size);
  }
  for (size_t i = 0, j = t_index; i < t_count; i++, j++) {
    rit_str_set(t_rit_str, i, rit_str_at(t_rit_str_other, j));
  }
}

void rit_str_replace_with_location(const char *t_file, int t_line,
                                   char *t_rit_str, size_t t_index,
                                   size_t t_count, const char *t_cstr,
                                   rit_str_allocator *t_allocator) {
  if (t_index > rit_str_size(t_rit_str)) {
    fprintf(stderr,
            "Error: starting index of substring out of bounds of the string, "
            "file: %s, line: %d\n",
            t_file, t_line);
    exit(EXIT_FAILURE);
  } else if (t_count == 0) {
    fprintf(stderr,
            "Error: size of substring cannot be 0, file: %s, line: %d\n",
            t_file, t_line);
    exit(EXIT_FAILURE);
  } else if (t_index + t_count > rit_str_size(t_rit_str)) {
    fprintf(stderr,
            "Error: size of substring greater than the string, file: %s, line: "
            "%d\n",
            t_file, t_line);
    exit(EXIT_FAILURE);
  }
  if (t_count < strlen(t_cstr)) {
    size_t count = strlen(t_cstr) - t_count;
    rit_str_insert(t_rit_str, t_index, count, ' ', t_allocator);
  } else if (t_count > strlen(t_cstr)) {
    size_t count = t_count - strlen(t_cstr);
    rit_str_erase(t_rit_str, t_index, count);
  }
  for (size_t i = t_index, j = 0; j < strlen(t_cstr); i++, j++) {
    rit_str_set(t_rit_str, i, t_cstr[j]);
  }
}

#endif  // RIT_STR_IMPLEMENTATION
#endif  // RIT_STR_H_INCLUDED
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
