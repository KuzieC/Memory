/**
 * @file ThreadCache.h
 * @brief Thread-local cache for high-performance memory allocation
 * 
 * This file implements a thread-local caching layer that reduces contention
 * on the central cache by maintaining per-thread free lists.
 */

#pragma once
#include "helper.h"

/**
 * @brief Thread-local memory cache for improved allocation performance
 * 
 * ThreadCache provides thread-local storage for frequently allocated objects,
 * reducing the need to access the central cache and improving performance
 * in multi-threaded applications.
 */
class ThreadCache{
public:
    /**
     * @brief Get the thread-local ThreadCache instance
     * @return Pointer to the thread-local ThreadCache instance
     * 
     * Uses thread_local storage to ensure each thread has its own cache.
     */
    static ThreadCache* get(){
        static thread_local ThreadCache threadCache;
        return &threadCache;
    }

    /**
     * @brief Allocate memory from thread cache
     * @param size Number of bytes to allocate
     * @return Pointer to allocated memory, or nullptr if size is invalid
     * 
     * First tries to satisfy allocation from local free list.
     * Falls back to central cache if local list is empty.
     */
    void* allocate(size_t size);
    
    /**
     * @brief Deallocate memory to thread cache
     * @param ptr Pointer to memory to deallocate
     * @param size Size of the memory block
     * 
     * Adds memory back to local free list. If list becomes too large,
     * returns excess memory to central cache.
     */
    void deallocate(void* ptr, size_t size);
    
private:
    /**
     * @brief Get memory from central cache when local cache is empty
     * @param size Size index for the allocation
     * @return Pointer to allocated memory
     */
    void* getCentralCache(size_t size);
    
    /**
     * @brief Return excess memory to central cache
     * @param ptr Pointer to memory chain to return
     * @param size Size of memory blocks
     */
    void returnCentralCache(void* ptr, size_t size);
    
    /**
     * @brief Private constructor for singleton pattern
     */
    ThreadCache(){
        freeList.fill(nullptr);
    }
    
    /** @brief Array of free list heads for different sizes */
    std::array<void*, MaxIndex> freeList;
    
    /** @brief Array tracking size of each free list */
    std::array<size_t, MaxIndex> freeListSize;
};