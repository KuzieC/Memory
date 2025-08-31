/**
 * @file helper.h
 * @brief Common helper functions, constants, and platform abstractions for memory management
 * 
 * This file contains utility functions and constants used throughout the memory allocator system,
 * including platform-specific memory allocation wrappers and size calculation helpers.
 */

#pragma once
#include <stdexcept>
#include <array>
#ifdef _WIN32
#include <windows.h>
#else
#include <stdlib.h>
#include <sys/mman.h>
#endif

/** @brief Maximum size for small object allocations (256 KB) */
constexpr size_t MaxSize = 256 * 1024; // 256 KB

/** @brief Memory alignment requirement (size of pointer) */
constexpr size_t alignment = sizeof(void*);

/** @brief Maximum index for memory pools based on MaxSize and alignment */
constexpr size_t MaxIndex = MaxSize / alignment;

/** @brief Number of pages per span in the page cache */
constexpr size_t SpanPage = 8;

/** @brief Standard page size (4096 bytes) */
constexpr size_t PageSize = 4096;

/**
 * @brief Calculate index for a given allocation size
 * @param size The size in bytes to calculate index for
 * @return The index corresponding to the allocation size
 * @throws std::invalid_argument if size is 0 or negative
 */
inline size_t getIndex(size_t size){
    if(size <= 0){
        throw std::invalid_argument("Size must be greater than 0");
    }

    return (size - 1) / alignment;
}

#ifndef _WIN32
#ifndef MEM_COMMIT
#define MEM_COMMIT 0
#endif
#ifndef MEM_RESERVE
#define MEM_RESERVE 0
#endif
#ifndef MEM_RELEASE
#define MEM_RELEASE 0
#endif
#ifndef PAGE_READWRITE
#define PAGE_READWRITE 0
#endif

/**
 * @brief Cross-platform wrapper for Windows VirtualAlloc function
 * @param addr Starting address for the allocation (unused on non-Windows platforms)
 * @param size Size in bytes to allocate
 * @param flAllocationType Allocation type flags (unused on non-Windows)
 * @param flProtect Memory protection flags (unused on non-Windows)
 * @return Pointer to allocated memory
 * @throws std::bad_alloc if allocation fails
 */
inline void* VirtualAlloc(void* addr, size_t size, int, int) {
    (void)addr; // Suppress unused parameter warning
    void* ptr = aligned_alloc(PageSize, size);
    if (!ptr) throw std::bad_alloc();
    return ptr;
}

/**
 * @brief Cross-platform wrapper for Windows VirtualFree function
 * @param addr Pointer to memory to be freed
 * @param size Size parameter (unused on non-Windows platforms)
 * @param dwFreeType Free type flags (unused on non-Windows)
 */
inline void VirtualFree(void* addr, size_t, int) {
    free(addr);
}
#endif
