#include "allocator.hpp"
#include <unistd.h>
#include <iostream>

namespace custom_mem {

CustomAllocator& CustomAllocator::instance() {
    static CustomAllocator instance;
    return instance;
}

CustomAllocator::~CustomAllocator() {
    // Reset process heap break pointer if heap was initialized
    if (heap_start_) {
        brk(heap_start_);
    }
}

// Align requested size to 16-byte boundary
static size_t align_size(size_t size) {
    return (size + ALIGNMENT - 1) & ~(ALIGNMENT - 1);
}

BlockHeader* CustomAllocator::find_free_block(size_t size) {
    BlockHeader* current = heap_start_;
    while (current) {
        if (current->is_free && current->size >= size) {
            return current;
        }
        current = current->next;
    }
    return nullptr;
}

BlockHeader* CustomAllocator::request_space(BlockHeader* last, size_t size) {
    size_t total_size = HEADER_SIZE + size;
    
    void* cur_brk = sbrk(total_size);
    if (cur_brk == (void*)-1) {
        return nullptr; // sbrk failed
    }

    auto* block = reinterpret_cast<BlockHeader*>(cur_brk);
    block->size = size;
    block->is_free = false;
    block->next = nullptr;
    block->prev = last;

    if (last) {
        last->next = block;
    }

    return block;
}

void* CustomAllocator::allocate(size_t size) {
    if (size == 0) return nullptr;

    size_t aligned_size = align_size(size);
    std::lock_guard<std::mutex> guard(lock_);

    if (!heap_start_) {
        // First allocation
        BlockHeader* block = request_space(nullptr, aligned_size);
        if (!block) return nullptr;
        heap_start_ = block;
        heap_end_ = block;
        return reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(block) + HEADER_SIZE);
    }

    // First-fit lookup
    BlockHeader* block = find_free_block(aligned_size);
    if (!block) {
        // Expand heap break boundary
        block = request_space(heap_end_, aligned_size);
        if (!block) return nullptr;
        heap_end_ = block;
    } else {
        block->is_free = false;
    }

    return reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(block) + HEADER_SIZE);
}

void CustomAllocator::coalesce(BlockHeader* block) {
    // Merge forward if next block is free
    if (block->next && block->next->is_free) {
        block->size += HEADER_SIZE + block->next->size;
        block->next = block->next->next;
        if (block->next) {
            block->next->prev = block;
        } else {
            heap_end_ = block;
        }
    }

    // Merge backward if previous block is free
    if (block->prev && block->prev->is_free) {
        block->prev->size += HEADER_SIZE + block->size;
        block->prev->next = block->next;
        if (block->next) {
            block->next->prev = block->prev;
        } else {
            heap_end_ = block->prev;
        }
    }
}

void CustomAllocator::deallocate(void* ptr) {
    if (!ptr) return;

    std::lock_guard<std::mutex> guard(lock_);

    auto* block = reinterpret_cast<BlockHeader*>(reinterpret_cast<uintptr_t>(ptr) - HEADER_SIZE);
    block->is_free = true;

    // Coalesce adjacent free blocks to prevent fragmentation
    coalesce(block);
}

void CustomAllocator::print_heap_status() const {
    std::lock_guard<std::mutex> guard(lock_);
    std::cout << "\n=== Heap Status Dump ===" << std::endl;
    BlockHeader* curr = heap_start_;
    int index = 0;
    while (curr) {
        std::cout << "Block " << index++ << " | Addr: " << curr
                  << " | Size: " << curr->size
                  << " | Free: " << (curr->is_free ? "YES" : "NO") << std::endl;
        curr = curr->next;
    }
    std::cout << "========================\n" << std::endl;
}

void* custom_malloc(size_t size) {
    return CustomAllocator::instance().allocate(size);
}

void custom_free(void* ptr) {
    CustomAllocator::instance().deallocate(ptr);
}

} // namespace custom_mem
