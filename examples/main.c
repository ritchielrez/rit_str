#define _CRT_SECURE_NO_WARNINGS

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define nullptr (void *)0

void *libc_malloc(void *t_ctx, size_t t_size_in_bytes) {
  (void)t_ctx;
  return malloc(t_size_in_bytes);
}

void libc_free(void *t_ctx, void *t_ptr) {
  (void)t_ctx;
  free(t_ptr);
}

void *libc_realloc(void *t_ctx, void *t_old_ptr, size_t t_old_size_in_bytes,
                   size_t t_new_size_in_bytes) {
  (void)t_ctx;
  (void)t_old_size_in_bytes;
  return realloc(t_old_ptr, t_new_size_in_bytes);
}

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

/// @brief Get the pointer to the metadata header struct of a string.
/// @param t_rit_str The string
#define rit_str_get_metadata(t_rit_str) (&((rit_str_metadata *)t_rit_str)[-1])

/// @internal
inline char *rit_str_alloc(const char *t_file, int t_line, size_t t_size,
                           rit_str_allocator *t_allocator) {
  rit_str_metadata *arr = (rit_str_metadata *)t_allocator->alloc(
      t_allocator->m_ctx, sizeof(rit_str_metadata) + t_size * 2);
  if (!arr) {
    fprintf(stderr, "Error: allocation failed, file: %s, line: %d\n", t_file,
            t_line);
    exit(EXIT_FAILURE);
  }
  arr->m_size = t_size;
  arr->m_capacity = t_size * 2;
  arr += 1;
  return (char *)arr;
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

/// @brief Set the capacity of a string.
#define rit_str_reserve(t_rit_str, t_new_capacity, t_allocator) \
  rit_str_realloc(__FILE__, __LINE__, &t_rit_str, t_new_capacity, t_allocator)

/// @internal
inline void rit_str_realloc(const char *t_file, int t_line, char **t_rit_str,
                            size_t t_new_capacity,
                            rit_str_allocator *t_allocator) {
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

#define rit_str_swap(t_rit_str, t_rit_str_other) \
  do {                                           \
    void *tmp = t_rit_str;                       \
    t_rit_str = t_rit_str_other;                 \
    t_rit_str_other = tmp;                       \
  } while (0)

/// @brief Makes a non-binding request to make the capacity of a string equal to
/// its size. In this library this is definied as a no-op function.
inline void shrink_to_fit(char *t_rit_str) { (void)t_rit_str; }

/// @brief Initialize a rit_str.
#define rit_str(t_rit_str, t_cstr, t_allocator)                       \
  char *t_rit_str =                                                   \
      rit_str_alloc(__FILE__, __LINE__, strlen(t_cstr), t_allocator); \
  strcpy(t_rit_str, t_cstr)

#define rit_str_at(t_rit_str, t_index)                                 \
  (rit_str_index_bounds_check(__FILE__, __LINE__, t_rit_str, t_index)) \
      ? t_rit_str[t_index]                                             \
      : t_rit_str[t_index]

#define rit_str_set(t_rit_str, t_index, t_char)                             \
  if (rit_str_index_bounds_check(__FILE__, __LINE__, t_rit_str, t_index)) { \
    t_rit_str[t_index] = t_char;                                            \
  }

/// @param t_rit_str Where to copy
/// @param t_rit_str_other What to copy
#define rit_str_copy(t_rit_str, t_rit_str_other, t_allocator)                 \
  rit_str_reserve(t_rit_str_other, rit_str_capacity(t_rit_str), t_allocator); \
  rit_str_get_metadata(t_rit_str_other)->m_size = rit_str_size(t_rit_str);    \
  for (size_t i = 0; i < rit_str_size(t_rit_str); i++) {                      \
    rit_str_set(t_rit_str_other, i, rit_str_at(t_rit_str, i));                \
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

#define rit_str_append(t_rit_str, t_count, t_char, t_allocator) \
  for (size_t i = 1; i <= t_count; i++) {                       \
    rit_str_push_back(t_rit_str, t_char, t_allocator);          \
  }

#define rit_str_resize(t_rit_str, t_count, t_char, t_allocator) \
  rit_str_clear(t_rit_str);                                     \
  rit_str_append(t_rit_str, t_count, t_char, t_allocator)

/// @brief Insert a value in the array before t_pos.
#define rit_str_insert(t_rit_str, t_pos, t_char, t_allocator)     \
  rit_str_push_back(t_rit_str, 0, t_allocator);                   \
  for (size_t i = rit_str_size(t_rit_str) - 1; i >= t_pos; i--) { \
    rit_str_set(t_rit_str, i, rit_str_at(t_rit_str, i - 1));      \
    rit_str_set(t_rit_str, i - 1, 0);                             \
  }                                                               \
  rit_str_set(t_rit_str, t_pos - 1, t_char)

/// @brief Remove the value in the array at t_pos.
#define rit_str_erase(t_rit_str, t_pos)                          \
  for (size_t i = t_pos + 1; i < rit_str_size(t_rit_str); i++) { \
    rit_str_set(t_rit_str, i - 1, rit_str_at(t_rit_str, i));     \
  }                                                              \
  rit_str_pop_back(t_rit_str)

inline void rit_str_replace(char *t_rit_str, size_t t_pos, size_t t_count,
                            const char *t_cstr,
                            rit_str_allocator *t_allocator) {
  size_t substr_size = t_count - t_pos;
  size_t cstr_size = strlen(t_cstr);
  if (substr_size < cstr_size) {
    for (size_t count = 1; count <= cstr_size - substr_size; ++count) {
      rit_str_insert(t_rit_str, t_pos + 1, ' ', t_allocator);
    }
  } else if (substr_size > cstr_size) {
    for (size_t count = 0; count <= substr_size - cstr_size; ++count) {
      rit_str_erase(t_rit_str, t_pos);
    }
  }
  for (size_t i = t_pos, j = 0; i < t_pos + cstr_size; ++i, ++j) {
    t_rit_str[i] = t_cstr[j];
  }
}

rit_str_allocator allocator = {libc_malloc, libc_free, libc_realloc, nullptr};

int main() {
  rit_str(str, "hello world", &allocator);
  rit_str(str2, "hello world2", &allocator);
  rit_str_push_back(str, '1', &allocator);
  rit_str_pop_back(str);
  rit_str_insert(str, 1, 't', &allocator);
  rit_str_erase(str, 0);
  rit_str_replace(str, 6, 11, "hell", &allocator);
  printf("size: %zu, cap: %zu, str: %s\n", rit_str_size(str),
         rit_str_capacity(str), str);
  rit_str_clear(str);
  printf("rit_str_empty(str) = %s\n", rit_str_empty(str) ? "true" : "false");
  rit_str_resize(str2, 12, 'h', &allocator);
  printf("size: %zu, cap: %zu, str: ", rit_str_size(str2),
         rit_str_capacity(str2));
  for (char *it = rit_str_begin(str2); it < rit_str_end(str2); it++) {
    putchar(*it);
  }
  printf("\n");
}