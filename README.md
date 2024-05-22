# rit_str - A custom string type written in C

## A working implementation of C++ like string and string view in C!
### WARNING: For the latest updates, check out this [repo](https://github.com/ritchielrez/libraries).

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
to save heap allocation calls as string views are stored in stack memory. Check out this 
[repo](https://github.com/ritchielrez/c-string-test) to learn more about how you may use 
this feature.

### How to use this library

Just copy and past the file `rit_str.h` in your project. Then add these lines to your
source files: 
```cpp
#define C_STRING_TEST_IMPLEMENTATION
#include "rit_str.h"
```

This library also required [this]() single header only library to be included in your project.

### Known issues
##### Not everything is yet implemented, such as:
- string copy, like `strcopy`
- probably a better way to convert a `rit_str` to `rit_str_view`

Feel free to open a new GitHub issue or pull request to suggest any changes!!
