#include "allocator.hpp"
#include <iostream>
#include <vector>
#include <thread>
#include <cassert>

void test_basic_allocation() {
    std::cout << "[RUNNING] Basic Allocation Test..." << std::endl;
    
    int* val = static_cast<int*>(custom_mem::custom_malloc(sizeof(int)));
    assert(val != nullptr);
    *val = 42;
    assert(*val == 42);
    
    // Check 16-byte alignment
    assert(reinterpret_cast<uintptr_t>(val) % 16 == 0);

    custom_mem::custom_free(val);
    std::cout << "[PASSED] Basic Allocation Test" << std::endl;
}

void test_coalescing() {
    std::cout << "[RUNNING] Coalescing & Fragmentation Test..." << std::endl;

    void* p1 = custom_mem::custom_malloc(128);
    void* p2 = custom_mem::custom_malloc(256);
    void* p3 = custom_mem::custom_malloc(128);

    custom_mem::CustomAllocator::instance().print_heap_status();

    // Freeing adjacent blocks should coalesce them into a single block
    custom_mem::custom_free(p1);
    custom_mem::custom_free(p2);

    custom_mem::CustomAllocator::instance().print_heap_status();

    custom_mem::custom_free(p3);
    std::cout << "[PASSED] Coalescing Test" << std::endl;
}

void test_multithreading() {
    std::cout << "[RUNNING] Multi-threaded Allocation Test..." << std::endl;

    auto worker = []() {
        for (int i = 0; i < 100; ++i) {
            void* ptr = custom_mem::custom_malloc(64);
            std::this_thread::sleep_for(std::chrono::microseconds(10));
            custom_mem::custom_free(ptr);
        }
    };

    std::vector<std::thread> threads;
    for (int i = 0; i < 8; ++i) {
        threads.emplace_back(worker);
    }

    for (auto& t : threads) {
        t.join();
    }

    std::cout << "[PASSED] Multi-threaded Allocation Test" << std::endl;
}

int main() {
    test_basic_allocation();
    test_coalescing();
    test_multithreading();
    
    std::cout << "\nAll allocator tests completed successfully!" << std::endl;
    return 0;
}
