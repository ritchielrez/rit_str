#define _CRT_SECURE_NO_WARNINGS

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RIT_STR_IMPLEMENTATION
#include "../rit_str.h"

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

rit_str_allocator allocator = {libc_malloc, libc_free, libc_realloc, nullptr};

int main() {
  rit_str(str, "hello world", &allocator);

  char *str2 = rit_str_alloc(strlen("Hello world"), &allocator);
  // If the size of substring is set to 0, then size is actually set to
  // strlen("Hello World") - index, in this case
  rit_str_copy_cstr(str2, 0, 0, "Hello world", &allocator);
  // rit_str_copy_rit_str(str2, 0, 0, str, &allocator);
  rit_str_append_str(str2, "ello world", &allocator);

  rit_str_push_back(str, '1', &allocator);
  rit_str_pop_back(str);
  rit_str_insert(str, 0, 1, 't', &allocator);
  rit_str_erase(str, 0, 1);
  rit_str_replace(str, 0, 5, "hell", &allocator);
  printf("size: %zu, cap: %zu, str: ", rit_str_size(str),
         rit_str_capacity(str));
  for (char *it = rit_str_begin(str); it < rit_str_end(str); it++) {
    putchar(*it);
  }
  printf("\n");
  rit_str_clear(str);
  printf("rit_str_empty(str) = %s\n", rit_str_empty(str) ? "true" : "false");

  rit_str_resize(str2, 12, 'h', &allocator);
  printf("size: %zu, cap: %zu, str: %s\n", rit_str_size(str2),
         rit_str_capacity(str2), str2);
}
