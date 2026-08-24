cmake_minimum_required(VERSION 3.14)
project(CustomMemoryAllocator CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -O2")

include_directories(include)

# Compile allocator static library
add_library(custom_allocator STATIC src/allocator.cpp)

# Build test suite
add_executable(allocator_tests tests/test_main.cpp)
target_link_libraries(allocator_tests PRIVATE custom_allocator pthread)
