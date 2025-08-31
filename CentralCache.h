/**
 * @file CentralCache.h
 * @brief Central memory cache managing memory spans from page cache
 * 
 * This file implements the central cache that sits between thread caches
 * and the page cache, managing memory spans and providing thread-safe
 * allocation and deallocation.
 */

#pragma once
#include "helper.h"
#include <mutex>
#include <atomic>

/**
 * @brief Central cache for coordinating memory between threads and page cache
 * 
 * CentralCache manages memory spans obtained from the page cache and distributes
 * them to thread caches. It uses atomic operations and spinlocks to provide
 * thread-safe access while minimizing contention.
 */
class CentralCache {
public:
    /**
     * @brief Get the singleton CentralCache instance
     * @return Pointer to the global CentralCache instance
     */
    static CentralCache* get() {
        static CentralCache CentralCache;
        return &CentralCache;
    }

    /**
     * @brief Get memory from central cache for a specific size index
     * @param index Size index corresponding to allocation size
     * @return Pointer to allocated memory block
     */
    void* getCentralCache(size_t index);
    
    /**
     * @brief Return memory blocks to central cache
     * @param ptr Pointer to first block in chain
     * @param index Size index for the memory blocks
     * @param count Number of blocks being returned
     */
    void returnCentralCache(void* ptr, size_t index, size_t count);
    
private:
    /**
     * @brief Private constructor for singleton pattern
     */
    CentralCache() {
        for (auto& slot : CentralFreeList) {
            slot.store(nullptr, std::memory_order_relaxed);
        }
        for (auto& flag : CentralFreeListLock) {
            flag.clear();
        }
    }

    /**
     * @brief Get memory span from page cache
     * @param size Size of memory span needed
     * @return Pointer to memory span
     */
    void* getPageCache(size_t size);
    
    /** @brief Array of atomic pointers for free lists of different sizes */
    std::array<std::atomic<void*>, MaxIndex> CentralFreeList;
    
    /** @brief Array of atomic flags for spinlock protection of free lists */
    std::array<std::atomic_flag, MaxIndex> CentralFreeListLock;
};  