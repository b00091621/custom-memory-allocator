# Thread-Safe Custom Memory Allocator (C++17)

A low-level implementation of dynamic memory management (`custom_malloc` / `custom_free`) in C++ using POSIX `sbrk()` system calls, explicit doubly-linked free-list tracking, adjacent block coalescing, and strict 16-byte SIMD alignment guarantees.

## Architecture Overview

```text
+---------------------+-------------------------------+---------------------+
| Block Header        | Payload Memory                | Next Block Header   |
| (size, free status, | (16-Byte Boundary Aligned     | ...                 |
|  prev, next pointers|  User Data)                   |                     |
+---------------------+-------------------------------+---------------------+
```

## Core Features
- **16-Byte Boundary Alignment:** Ensures addresses returned are compatible with modern AVX/SIMD instructions.
- **First-Fit Search Algorithm:** Traverses the explicit doubly-linked heap list to find the first suitable unallocated memory chunk.
- **Bi-Directional Coalescing:** Dynamically merges adjacent free blocks during `deallocate()` to mitigate heap fragmentation.
- **Thread Safety:** Protected via `std::mutex` guard locks for concurrent allocation support.
- **Zero External Dependencies:** Native POSIX heap extension via standard OS system calls.

## Project Structure
```
custom-memory-allocator/
├── CMakeLists.txt
├── include/
│   └── allocator.hpp
├── src/
│   └── allocator.cpp
└── tests/
    └── test_main.cpp
```

## How to Build & Run Unit Tests

```bash
# Clone the repository
git clone [https://github.com/b00091621/custom-memory-allocator.git](https://github.com/b00091621/custom-memory-allocator.git)
cd custom-memory-allocator

# Build with CMake
mkdir build && cd build
cmake ..
make

# Run the test executable
./allocator_tests
```
