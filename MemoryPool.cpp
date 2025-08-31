/**
 * @file MemoryPool.cpp
 * @brief Implementation of lock-free memory pool for fixed-size allocations
 * 
 * This file implements the core memory pool functionality using atomic operations
 * for high-performance, thread-safe memory allocation and deallocation.
 */

#include "MemoryPool.h"

MemoryPool::MemoryPool()
    : freeList(nullptr)
    , currBlock(nullptr)
    , endBlock(nullptr)
    , firstBlock(nullptr)
    , SLOT_SIZE(0){

}


void MemoryPool::init(size_t size) {
    SLOT_SIZE = size;
    firstBlock = nullptr;
    currBlock = nullptr;
    endBlock = nullptr;
    freeList = nullptr;
}

size_t MemoryPool::alignPad(char* cur, size_t align) {
    return (align - (reinterpret_cast<uintptr_t>(cur) % align))%align;
}

void* MemoryPool::popFreeList() {
    auto oldHead = freeList.load(std::memory_order_acquire);
    if(oldHead == nullptr) return nullptr;
    Slot* newHead = oldHead->next.load(std::memory_order_relaxed);
    while(!freeList.compare_exchange_weak(oldHead, newHead,
        std::memory_order_acquire, std::memory_order_relaxed)){
            if(oldHead == nullptr) return nullptr;
            newHead = oldHead->next.load(std::memory_order_relaxed);
    }
    return oldHead;
}

void MemoryPool::pushFreeList(Slot* b) {
    auto oldHead = freeList.load(std::memory_order_relaxed);
    b->next.store(oldHead, std::memory_order_relaxed);
    while(!freeList.compare_exchange_weak(oldHead, b,
        std::memory_order_release, std::memory_order_relaxed)){
            b->next.store(oldHead, std::memory_order_relaxed);
    }
}

void* MemoryPool::allocate(size_t n, size_t align) {
    // Check if we can fit n in the current block
    if( n > BLOCK_SIZE - sizeof(SLOT_SIZE)) {
        return operator new(n);
    }
    void* newFreeBlock = popFreeList();
    if(newFreeBlock != nullptr){
        return newFreeBlock;
    }
    std::lock_guard<std::mutex> lock(firstBlockMutex);
    if(!currBlock || currBlock + n >= endBlock) {
        Slot* newBlock = static_cast<Slot*>(operator new(BLOCK_SIZE));
        // Link firstBlock pointer
        newBlock->next = firstBlock;
        firstBlock = newBlock;   
        char* newCurr = reinterpret_cast<char*>(newBlock) + sizeof(Slot);
        size_t padding = alignPad(newCurr, align);
        currBlock = newCurr + padding;
        endBlock = reinterpret_cast<char*>(newBlock) + BLOCK_SIZE;
    }

    void* ret = currBlock;

    char* newCurr = reinterpret_cast<char*>(currBlock) + n;
    size_t padding = alignPad(newCurr, align);
    currBlock = reinterpret_cast<char*>(newCurr + padding); // Move curr pointer

    return ret; // Return the pointer to the allocated memory
}

void MemoryPool::deallocate(Slot* b) {
    if(!b) return;
    pushFreeList(b);
}

// void MemoryPool::deallocateAll() {
//     while(firstBlock) {
//         auto next = firstBlock->next;
//         operator delete(firstBlock);
//         firstBlock = next;
//     }
// }