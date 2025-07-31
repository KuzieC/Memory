#pragma once
#include <stdexcept>
#include <array>
#ifdef _WIN32
#include <windows.h>
#else
#include <stdlib.h>
#include <sys/mman.h>
#endif

constexpr size_t MaxSize = 256 * 1024; // 256 KB
constexpr size_t alignment = sizeof(void*);
constexpr size_t MaxIndex = MaxSize / alignment;
constexpr size_t SpanPage = 8;
constexpr size_t PageSize = 4096;

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
inline void* VirtualAlloc(void* addr, size_t size, int, int) {
    void* ptr = aligned_alloc(PageSize, size);
    if (!ptr) throw std::bad_alloc();
    return ptr;
}
inline void VirtualFree(void* addr, size_t, int) {
    free(addr);
}
#endif
