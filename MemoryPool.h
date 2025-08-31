/**
 * @file MemoryPool.h
 * @brief Lock-free memory pool implementation for fixed-size allocations
 * 
 * This file implements a high-performance memory pool that uses atomic operations
 * for lock-free allocation and deallocation of fixed-size memory blocks.
 */

#pragma once

#include <cstdint>
#include <atomic>
#include <memory>
#include <mutex>

/**
 * @brief Node structure for the lock-free linked list in memory pools
 * 
 * Each slot represents a free memory block that can be atomically linked
 * to form a lock-free free list.
 */
struct Slot {
    /** @brief Atomic pointer to the next slot in the free list */
    std::atomic<Slot*> next;
};

/** @brief Default block size for memory pool allocations (4096 bytes) */
static const size_t BLOCK_SIZE = 4096;

/**
 * @brief High-performance memory pool for fixed-size allocations
 * 
 * This class implements a memory pool that allocates large blocks and subdivides
 * them into fixed-size slots. It uses lock-free atomic operations for the free list
 * to minimize contention in multi-threaded environments.
 */
class MemoryPool{
    /** @brief Lock-free atomic pointer to the head of the free list */
    std::atomic<Slot*> freeList;
    
    /** @brief Pointer to current allocation position in active block */
    char* currBlock;
    
    /** @brief Pointer to end of current active block */
    char* endBlock;
    
    /** @brief Pointer to first allocated block (for cleanup) */
    Slot* firstBlock;
    
    /** @brief Size of individual slots in this pool */
    size_t SLOT_SIZE;
    
    /**
     * @brief Calculate padding needed for memory alignment
     * @param cur Current memory address
     * @param align Required alignment boundary
     * @return Number of bytes of padding needed
     */
    size_t alignPad(char* cur, size_t align);
    
    /**
     * @brief Atomically pop a slot from the free list
     * @return Pointer to free slot, or nullptr if list is empty
     */
    void* popFreeList();
    
    /**
     * @brief Atomically push a slot to the free list
     * @param b Slot to add to the free list
     */
    void pushFreeList(Slot* b);
    
    /** @brief Mutex for protecting block allocation operations */
    std::mutex firstBlockMutex;

public:
    /**
     * @brief Allocate memory from the pool
     * @param n Number of bytes to allocate
     * @param align Memory alignment requirement (default: 8 bytes)
     * @return Pointer to allocated memory
     */
    void* allocate(size_t n, size_t align = 8);
    
    /**
     * @brief Return memory to the pool
     * @param b Slot to deallocate
     */
    void deallocate(Slot* b);
    
    /**
     * @brief Deallocate all memory blocks (currently unused)
     */
    void deallocateAll();
    
    /**
     * @brief Initialize the memory pool with a specific slot size
     * @param Slot_size Size of each slot in bytes
     */
    void init(size_t Slot_size);
    
    /**
     * @brief Default constructor
     */
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
