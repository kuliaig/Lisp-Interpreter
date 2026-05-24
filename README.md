# Scheme-like interpreter in C

Studying project: interpreter Scheme-like language with implementation of basic functions.

## Features

- Numbers, strings, characters, boolean values, nil
- Lists and dot pairs (car, cdr, cons, null?, pair?, list?)
- Vectors (via plugin)
- Quote, if, define, lambda, set!, and, or, map
- Closures, lambda functions, functions as first-order objects
- Memoization (enabled/disabled via define/define-no-memo)
- Tab completion
- Multiline input
- Loading scripts from a file (open <file>)
- Dynamic libraries (load-plugin)

## Restrictions

- Recursion is limited to a depth of 5000 calls (stack overflow protection)
- Integers in the range `[-9223372036854775808, 9223372036854775807]`

## Build

### Windows 

mingw32-make

### Linux

make

## Launch

### Windows 

.\lisp               # REPL
.\lisp --help        # Help (you can watch there all functions and special forms)
.\lisp open test.scm # Run the script

### Linux

./lisp               # REPL
./lisp --help        # Help (you can watch there all functions and special forms)
./lisp open test.scm # Run the script

## Plugin API

The interpreter supports dynamic libraries (`.dll` on Windows, `.so` on Linux). Memoization for downloadable features is disabled by default.

### How to create a plugin

1. Include the header file `liblisp.h`
2. Implement functions with the signature `lisp_object* func(lisp_object* args)`
3. Export `PluginFunc* init_plugin(void)` — returns an array `{name, func}` ending with `{NULL, NULL}`

### Example

See `vectorlib.c` for a plugin for working with vectors.

```c
#include "liblisp.h"

static lisp_object* my_func(lisp_object* args) {
 // ...
}

EXPORT PluginFunc* init_plugin(void) {
 static PluginFunc funcs[] = {
 {"my-func", my_func},
 {NULL, NULL}
 };
 return funcs;
}
```

### Compilation

gcc -shared my_plugin.c -o my_plugin.dll -I.

## Cleaning

mingw32-make clean

