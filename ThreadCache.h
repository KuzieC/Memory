#pragma once
#include "helper.h"

class ThreadCache{
public:
    static ThreadCache* get(){
        static thread_local ThreadCache threadCache;
        return &threadCache;
    }

    void* allocate(size_t size);
    void deallocate(void* ptr, size_t size);
private:
    void* getCentralCache(size_t size);
    void returnCentralCache(void* ptr, size_t size);
    ThreadCache(){
        freeList.fill(nullptr);
    }
    std::array<void*, MaxIndex> freeList;
    std::array<size_t, MaxIndex> freeListSize;
};