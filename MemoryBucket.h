/**
 * @file MemoryBucket.h
 * @brief High-level memory allocator using segregated memory pools
 * 
 * This file implements the main interface for the memory allocation system,
 * providing fast allocation for small objects through a collection of memory pools.
 */

#pragma once
#include "MemoryPool.h"

/**
 * @brief High-performance memory allocator using segregated pools
 * 
 * MemoryBucket manages an array of 64 MemoryPool instances, each handling
 * allocations of different sizes (8 bytes to 512 bytes in 8-byte increments).
 * For larger allocations, it falls back to standard operator new/delete.
 */
struct MemoryBucket {
    /** @brief Array of 64 memory pools for different allocation sizes */
    static MemoryPool memorypool[64];
    
    /**
     * @brief Default constructor
     */
    MemoryBucket();
    
    /**
     * @brief Allocate memory of specified size
     * @param size Number of bytes to allocate
     * @return Pointer to allocated memory, or nullptr if size is 0
     * 
     * For sizes <= 512 bytes, uses appropriate memory pool.
     * For larger sizes, uses standard operator new.
     */
    static void* allocate(size_t size);
    
    /**
     * @brief Deallocate previously allocated memory
     * @param ptr Pointer to memory to deallocate
     * @param size Size of the memory block being deallocated
     * 
     * For sizes <= 512 bytes, returns memory to appropriate pool.
     * For larger sizes, uses standard operator delete.
     */
    static void deallocate(void* ptr, size_t size);
    
    /**
     * @brief Initialize all memory pools
     * 
     * Must be called before using the allocator. Initializes all 64 pools
     * with their respective slot sizes (8, 16, 24, ..., 512 bytes).
     */
    static void initMemoryPool();
};