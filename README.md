# rit_str - A custom string type written in C

## A working implementation of C++ like string and string view in C!

This is an example project trying to implement C++ like **heap allocated strings**
and **non owning string refereces(string views)** in C.

### Features
- Heap allocated strings:  
This allows resizing of the strings to assign bigger strings if needed.

- String views:  
Allocating in the heap is always costly, and when we want a read only reference
to a string, we can use string views, which holds a const pointer to a string in it.
This allows for less heap memory to be allocated especially when we are passing strings 
to functions where they may not be changed in anyway.

- Count/size based strings:  
Hey, we now do not need to call `strlen()` everytime when we want a string's length!
Alos count based strings makes looping through the characters of a string easier,
because we no longer need to check for null character to stop the iteration, we
just now have the length/count/size stored inside the string itself.

- Count/size based string views:  
These have the same benifits as count based strings, but more! Suppose we have a string
like this: `Hello.txt`. If we just want the substring `Hello` and store it, we can allocate
a new string and store it. But with string views, we can hold a pointer to the whole string 
in a string view, yet hold the substring in that same string view. Because it is count based, 
we can set the count to 5 in this case, and using `rit_str_view_print()` we can print the 
substring only, even though the string view holds a pointer to the whole string. This allows 
to save heap allocation calls as string views are stored in stack memory.

### How to use this library

Just copy and past the file `rit_str.h` in your project. Then add these lines to your
source files: 
```cpp
#define RIT_STR_IMPLEMENTATION
#include "rit_str.h"
```
Example usage:
```cpp
#include <stdlib.h>

#define RIT_STR_IMPLEMENTATION
#include "rit_str.h"

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
  rit_str *str = rit_str_create("Test", &allocator);
  rit_str_view str_view = rit_str_view_create(str->m_str, 0, str->m_size);
  rit_str_view str_view2 = rit_str_view_create("Hello", 0, 5);
  rit_str *str2 = rit_str_view_concat(&str_view, &str_view2, &allocator);
  rit_str *str3 = rit_str_concat(str, str2, &allocator);
  printf("str: %s, size: %zu \n", str->m_str, str->m_size); // Output: str: Test, size: 4
  rit_str_view_print(&str_view); // Output: Test
  printf("\n%s\n", str2->m_str); // Output: TestHello
  rit_str_free(str, &allocator);

  rit_str *str4 = rit_str_create("John Doe", &allocator);
  rit_str_set(str4, "John Doeeeeeeeeeeeeeeeeeee", &allocator);
  printf("str: %s, size: %zu \n", str4->m_str, str4->m_size); // Output: str: John Doeeeeeeeeeeeeeeeeeee, size: 26
  return 0;
}
```

### Known issues
##### Not everything is yet implemented, such as:
- Functions to get the string from `rit_str` and `rit_str_view`
- Remove the need for `rit_str_view_print()`
- string copy, like `strcopy`
- probably a better way to convert a `rit_str` to `rit_str_view`

Feel free to open a new GitHub issue or pull request to suggest any changes!!
