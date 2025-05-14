#pragma once

#include <cstdint>
#include <atomic>
#include <memory>
#include <mutex>
struct Slot {
    std::atomic<Slot*> next;
};

static const size_t BLOCK_SIZE = 4096;
//static const size_t SLOT_SIZE = 8;
class MemoryPool{
    std::atomic<Slot*> freeList;
    char* currBlock;
    char* endBlock;
    Slot* firstBlock;
    size_t SLOT_SIZE;
    size_t alighPad(char* cur, size_t align);
    void* popFreeList();
    void pushFreeList(Slot* b);
    std::mutex firstBlockMutex;

public:
    void* allocate(size_t n, size_t align = 8);
    void deallocate(Slot* b);
    void deallocateAll();
    void init(size_t Slot_size);
    MemoryPool();
};

/**
[Small Object Test]
Standard operator new: 75800 ms   
Custom memory pool: 275 ms        

[Large Object Test]
Standard operator new: 123478 ms  
Custom memory pool: 86214 ms

[Small Object Test: Standard operator new]
Standard operator new: 67170 ms
[Small Object Test: MemoryBucket]
MemoryBucket custom pool: 94 ms

[Large Object Test: Standard operator new]
Standard operator new: 75716 ms
[Large Object Test: MemoryBucket]
MemoryBucket custom pool: 80244 ms
 */
