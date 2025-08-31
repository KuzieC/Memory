/**
 * @file PageCache.h
 * @brief Page-level memory management with span tracking
 * 
 * This file implements the lowest level of the memory allocation system,
 * managing large memory spans from the operating system and tracking
 * them for efficient allocation and deallocation.
 */

#pragma once
#include "helper.h"
#include <mutex>
#include <map>

/**
 * @brief Memory span representing a contiguous range of pages
 * 
 * A Span represents a contiguous block of memory pages that can be
 * subdivided for smaller allocations or used as a unit for large allocations.
 */
class Span{
public:
    /**
     * @brief Construct a new Span object
     * @param start Starting address of the span
     * @param pageNum Number of pages in this span
     * @throws std::bad_alloc if start address is null
     */
    Span(void* start, size_t pageNum);
    
    /**
     * @brief Destroy the Span object and free its memory
     */
    ~Span();
    
    /** @brief Starting address of this memory span */
    void* startAdd;
    
    /** @brief Number of pages in this span */
    size_t PageNum;
    
    /** @brief Pointer to next span in linked list */
    Span* next;
};

/**
 * @brief Page cache managing large memory spans from the OS
 * 
 * PageCache is responsible for obtaining large memory spans from the
 * operating system and managing them efficiently. It maintains spans
 * of different sizes and can split or merge them as needed.
 */
class PageCache {
public:
    /**
     * @brief Get the singleton PageCache instance
     * @return Pointer to the global PageCache instance
     */
    static PageCache* get() {
        static PageCache pageCache;
        return &pageCache;
    }

    /**
     * @brief Get a memory span of specified page count
     * @param PageNum Number of pages needed
     * @return Pointer to the start of the allocated span
     */
    void* getPageCache(size_t PageNum);
    
    /**
     * @brief Return a memory span back to the page cache
     * @param ptr Pointer to start of span to return
     * @param PageNum Number of pages in the span
     * @note Currently not implemented
     */
    void returnPageCache(void*ptr, size_t PageNum);
    
private:
    /**
     * @brief Private constructor for singleton pattern
     */
    PageCache() = default;
    
    /**
     * @brief Allocate a new span from the operating system
     * @param PageNum Number of pages to allocate
     * @return Pointer to newly allocated Span
     */
    Span* Alloc(size_t PageNum);
    
    /** @brief Mutex protecting access to span maps */
    std::mutex mutex_;
    
    /** @brief Map from page count to available spans */
    std::map<size_t, Span*> spanMap;
    
    /** @brief Map from address to span for deallocation tracking */
    std::map<void*, Span*> FreeSpanMap;
};