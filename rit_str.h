#ifndef RIT_STR_INCLUDED
#define RIT_STR_INCLUDED

#include <stddef.h>
#include <stdio.h>

#include "arena_allocator.h"

#define DEFAULT_STRING_SIZE 256

typedef struct rit_str rit_str;
typedef struct rit_str_view rit_str_view;

/// @brief A string structure holding a string and it's length
struct rit_str {
  size_t m_size;
  char m_str[];
};

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
/// @param t_arena The arena where the string will be allocated
/// @param t_str The string to be allocated
/// @return rit_str*
rit_str *rit_str_create(Arena *t_arena, const char *t_str);

/// @brief Allocate some reserver space for a new string
/// @param t_arena The arena where the string will be allocated
/// @param t_size_in_bytes The amount of reserverd space in bytes
/// @return rit_str*
rit_str *rit_str_reserve(Arena *t_arena, size_t t_size_in_bytes);

/// @brief Copy a c string to rit_str
/// @param t_rit_str
/// @param t_str The c string
/// @return void
void rit_str_copy(rit_str *t_rit_str, const char *t_str);

/// @brief Concatenate two strings
/// @param t_arena The arena where the new string will be allocated
/// @param t_rit_str_1
/// @param t_rit_str_2
/// @return rit_str*
rit_str *rit_str_concat(Arena *t_arena, rit_str const *t_rit_str_1,
                        rit_str const *t_rit_str_2);

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
rit_str_view rit_str_view_create(char const *t_str, size_t t_index,
                                 size_t t_size);

/// @brief Prints the string or substring that has been assigned to the string
/// view
/// @param t_rit_str_view The string view
/// @return void
void rit_str_view_print(rit_str_view const *t_rit_str_view);

/// @brief Concatenate two strings from two string views
/// @param t_arena The arena where the string is going to be allocated
/// @param t_rit_str_view_1
/// @param t_rit_str_view_2
/// @return rit_str*
rit_str *rit_str_view_concat(Arena *t_arena,
                             rit_str_view const *t_rit_str_view_1,
                             rit_str_view const *t_rit_str_view_2);

#ifdef RIT_STR_IMPLEMENTATION

#include <string.h>

#define ARENA_ALLOCATOR_IMPLEMENTATION
#include "arena_allocator.h"

rit_str *rit_str_reserve(Arena *t_arena, size_t t_size_in_bytes) {
  rit_str *result =
      (rit_str *)arena_alloc(t_arena, sizeof(rit_str) + t_size_in_bytes);
  result->m_size = t_size_in_bytes - 1;
  return result;
}

void rit_str_copy(rit_str *t_rit_str, const char *t_str) {
  if (t_rit_str->m_size < strlen(t_str)) {
    fprintf(stderr, "The provided string is too big to copy\n");
    return;
  }
  size_t i;

  for (i = 0; i < strlen(t_str); ++i) {
    t_rit_str->m_str[i] = t_str[i];
  }
  t_rit_str->m_str[i] = '\0';
}

rit_str *rit_str_create(Arena *t_arena, const char *t_str) {
  size_t size = DEFAULT_STRING_SIZE;
  if (strlen(t_str) + 1 > DEFAULT_STRING_SIZE) {
    size = strlen(t_str) + 1;
  }

  rit_str *result = rit_str_reserve(t_arena, size);

  rit_str_copy(result, t_str);

  return result;
}

rit_str *rit_str_concat(Arena *t_arena, rit_str const *t_rit_str_1,
                        rit_str const *t_rit_str_2) {
  size_t size = DEFAULT_STRING_SIZE;
  if (t_rit_str_1->m_size + t_rit_str_2->m_size + 1 > DEFAULT_STRING_SIZE) {
    size = t_rit_str_1->m_size + t_rit_str_2->m_size + 1;
  }
  rit_str *result = (rit_str *)arena_alloc(t_arena, sizeof(rit_str) + size);
  result->m_size = size - 1;
  size_t index = 0;

  for (size_t i = 0; i < t_rit_str_1->m_size; ++i, ++index) {
    result->m_str[index] = t_rit_str_1->m_str[i];
  }
  for (size_t i = 0; i < t_rit_str_2->m_size; ++i, ++index) {
    result->m_str[index] = t_rit_str_2->m_str[i];
  }
  result->m_str[size - 1] = '\0';
  return result;
}

rit_str_view rit_str_view_create(char const *t_str, size_t t_index,
                                 size_t t_size) {
  rit_str_view result;
  result.m_index = t_index;
  result.m_size = t_size;
  result.m_str = t_str;
  return result;
}

rit_str *rit_str_view_concat(Arena *t_arena,
                             rit_str_view const *t_rit_str_view_1,
                             rit_str_view const *t_rit_str_view_2) {
  size_t size = DEFAULT_STRING_SIZE;
  if (t_rit_str_view_1->m_size + t_rit_str_view_2->m_size + 1 >
      DEFAULT_STRING_SIZE) {
    size = t_rit_str_view_1->m_size + t_rit_str_view_2->m_size + 1;
  }
  rit_str *result = (rit_str *)arena_alloc(t_arena, sizeof(rit_str) + size);
  result->m_size = size - 1;
  size_t index = 0;

  for (size_t i = 0; i < t_rit_str_view_1->m_size; ++i, ++index) {
    result->m_str[index] = t_rit_str_view_1->m_str[i];
  }
  for (size_t i = 0; i < t_rit_str_view_2->m_size; ++i, ++index) {
    result->m_str[index] = t_rit_str_view_2->m_str[i];
  }
  result->m_str[size - 1] = '\0';
  return result;
}

void rit_str_view_print(rit_str_view const *t_rit_str_view) {
  size_t index = t_rit_str_view->m_index;

  for (size_t i = 0; i < t_rit_str_view->m_size; ++index, ++i) {
    printf("%c", t_rit_str_view->m_str[index]);
  }
}

#endif  // RIT_STR_IMPLEMENTATION
#endif  // RIT_STR_INCLUDED
