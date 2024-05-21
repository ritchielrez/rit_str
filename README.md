# rit_str - A custom string type written in C

## A working implementation of C++ like string and string view in C!

This is an example project trying to implement C++ like **heap allocated strings**
and **non owning string refereces(string views)** in C.

### Features
- Supports custom allocators  
If you prefer to use your own custom allocator for heap allocations, you can tell
this library to use it. In fact, it is required to specify what allocator you
want to use for this library to work.

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

### How to use a custom allocator
I will spare you the details and will instead show you the allocator interface 
implementation and some usage code.

**Allocater interface implementation**:
```cpp
/// @brief Custom allocator interface.
/// Functions allocating memory takes a custom allocator based off this
/// interface as a parameter.
typedef struct {
  void *(*alloc)(void *, size_t);
  void (*free)(void *, void *);
  void *(*realloc)(void *, void *, size_t, size_t);
  void *m_ctx;  // The arena, stack or etc where the memory would be allocated,
                // NULL if none
} rstr_allocator;
```

**Example usage**:
```cpp
#include <stdlib.h>
#define RIT_STR_IMPLEMENTATION
#include "rit_str.h"
#define ARENA_ALLOCATOR_IMPLEMENTATION
#include "arena_allocator.h"

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

int main() {
  rstr_allocator ctx_allocator = {libc_malloc,
                                    libc_free,
                                    libc_realloc, NULL};
  rstr *str = rit_str_create("Test", &allocator);
}
```
Only thing here to explain is maybe what is `t_ctx` doing here. `t_ctx` points
to a context which is an arena, a stack etc(depending on what allocator you are
using) where the memory will be allocated . For the generic heap allocator 
avalaible in C, there is no context. So, we are not doing anything with `t_ctx` 
and also initialzing `m_ctx` as NULL.

### How to use this library
Just copy and past the file `rit_str.h` in your project. Then add these lines to your
source files: 
```cpp
#define RIT_STR_IMPLEMENTATION
#include "rit_str.h"
```
For some example usage code, check out [this](./examples/).

### Known issues
##### Not everything is yet implemented, such as:
- probably a better way to convert a `rit_str` to `rit_str_view`

Feel free to open a new GitHub issue or pull request to suggest any changes!!
