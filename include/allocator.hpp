#ifndef ALLOCATOR_HPP
#define ALLOCATOR_HPP

#include <cstddef>
#include <mutex>

namespace custom_mem {

struct BlockHeader {
    size_t size;          // Size of the usable memory block (excluding header)
    bool is_free;         // Allocation flag
    BlockHeader* next;    // Next block in memory order
    BlockHeader* prev;    // Previous block in memory order
};

constexpr size_t ALIGNMENT = 16;
constexpr size_t HEADER_SIZE = (sizeof(BlockHeader) + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1);

class CustomAllocator {
public:
    static CustomAllocator& instance();

    void* allocate(size_t size);
    void deallocate(void* ptr);

    // Debugging utility to inspect heap status
    void print_heap_status() const;

private:
    CustomAllocator() = default;
    ~CustomAllocator();

    CustomAllocator(const CustomAllocator&) = delete;
    CustomAllocator& operator=(const CustomAllocator&) = delete;

    BlockHeader* request_space(BlockHeader* last, size_t size);
    BlockHeader* find_free_block(size_t size);
    void coalesce(BlockHeader* block);

    BlockHeader* heap_start_ = nullptr;
    BlockHeader* heap_end_ = nullptr;
    mutable std::mutex lock_;
};

// C-style wrapper API for convenience
void* custom_malloc(size_t size);
void custom_free(void* ptr);

} // namespace custom_mem

#endif // ALLOCATOR_HPP
