# lyra

[![CMake Linux](https://github.com/LyraProject/lyra/actions/workflows/cmake-linux.yml/badge.svg)](https://github.com/LyraProject/lyra/actions/workflows/cmake-linux.yml)
[![CMake Windows](https://github.com/LyraProject/lyra/actions/workflows/cmake-windows.yml/badge.svg)](https://github.com/LyraProject/lyra/actions/workflows/cmake-windows.yml)

Lyra is a general purpose library written in C. Anyone is welcome to add functionality, as long as it follows the [design principles](#philosophy--design-principles) below.

CI/CD pipelines test builds with `clang`, `gcc`, `tcc` all on Linux, and MSVC on Windows.

## How to build

### Option 1: CMake

You can use Lyra from cmake by adding it as a subdirectory: `add_subdirectory(lyra)`. You can `target_link_libraries(<your target> lyra)` to link to it and add all include directories. You can also use `lyra_s` instead of `lyra` to link statically.

### Option 2: Manual build

You can integrate Lyra source into any build system. Open the [CMakeLists.txt](./CMakeLists.txt) which ships with Lyra's source code. In there, you can see a variable called `LYRA_SRCS` - all those source files are the files Lyra needs. Simply add these to your build system of choice. No extra definitions or similar are needed.

If your environment supports C11's `<threads.h>`, you can add the `src/lyra_tsque.c` to the `LYRA_SRCS` (the CMakeLists.txt does this as well).

## Configuration

### Before Compilation

You can (but do NOT have to) configure Lyra before compiling it, by changing / supplying some `#define`s to the code. You can do this by editing the CMakeLists.txt (if you're building with CMake), or by editing any of the headers.

Here is a (possibly incomplete) list of `#define`able configuration options:

- `LYRA_ASSERT(x)` is a macro which is used to check some pre- and post-conditions, for example checking `!lyra_cbuf_is_empty()` at the beginning of `lyra_cbuf_pop()` (to avoid successfully `pop()`-ing an uninitialized value). You may override `LYRA_ASSERT` with anything, including a simple definition that does nothing (to turn it off).

### Normal Use

You must configure an allocator and deallocator function (usually `malloc` and `free`) when initializing a module. For this, you must supply function pointers to the arguments of the initializer of a module. Sounds complicated, but it's very simple. See the beginning of [example1](./examples/example1.c) or [example2](./examples/example2.c) for this. All other such configuration options are also set in these initialization arguments.

There is NO global state, so you don't have to keep track of anything else.

## Philosophy / Design Principles

The Lyra library tries to be as simple and user-friendly as possible, while maintaining very good performance. We test the library for memory leaks & UB, and always compile on multiple different compilers with lots of warnings enabled. We strive to document all code thoroughly (Doxygen-compatible).

Lyra and all modules in Lyra must follow some simple rules:

- No dependencies except libc.
- Allow for custom allocators.
- Headers build (mostly) without warings when `#include`d from C++.
- Namespacing everything with the `lyra_` prefix.
- Module initialization via a `lyra_<module>_init()` function, which:
    - Takes one `lyra_<module>_init_args*` argument
    - Allocates a `lyra_<module>` or `lyra_<module>_ctx` (depending on the use-case) on the heap via a supplied `alloc()` function (supplied inthe `..._args` struct), and makes any allocations at that time.
    - Returns `NULL` on failure.
- Module dinitialization / cleanup via `lyra_<module>_deinit()` function, which:
    - Can always take `NULL`
    - Deallocates all memory allocated by `lyra_<module>_init()` and any other memory allocated in the supplied structure.
- Incomplete type for the implementation details; `lyra_<module>` or `lyra_<module>_ctx` and friends are incomplete in the headers, so that they may only be passed around as opaque pointers. This is to avoid some ABI incompatibility when/if implementation details change, and to avoid the user touching internal bookkeeping variables.
- Document **ALL** user-facing types, functions and macros with a `///` comment.


## Lyra Slab Allocator

Header: [`lyra_slab.h`](./src/lyra_slab.h)

Example: [`example1`](./examples/example1.c)

Summary: The lyra slab allocator is a very primitive slab allocator. It allocates all memory at the beginning, and then allows allocations of a fixed size. Once all memory is used, it will refuse allocations. Memory can be free'd, of course.

Example use case: Caching values.

## Lyra Circular Buffer

Header: [`lyra_cbuf.h`](./src/lyra_cbuf.h)

Example: [`example2`](./examples/example2.c)

Summary: A fixed-size circular buffer (FIFO). The user can push() (insert) and pop (get & remove) values. Once full, new values will override the old values.

Example use case: Fixed-size queue for received data which not yet processed.

## Lyra Threadsafe Queue

[only available with C11's `<threads.h>`, enable/disable in CMakeLists by setting `LYRA_THREADS` to `ON` or `OFF`]

Header: [`lyra_tsque.h`](./src/lyra_tsque.h)

Example: [`example3`](./examples/example3.c)

Summary: A threadsafe fixed-size queue (based on `lyra_cbuf`). It uses a mutex and a condition variable to synchronize between threads.


