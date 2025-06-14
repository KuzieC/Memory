#include "CentralCache.h"
#include "PageCache.h"

void* CentralCache::getCentralCache(size_t index) {
    if(index > MaxIndex) {
        return nullptr; 
    }

    while(CentralFreeListLock[index].test_and_set(std::memory_order_acquire)) {
        std::this_thread::yield(); 
    }
    void* ptr = nullptr;
    try {
        ptr = CentralFreeList[index].load(std::memory_order_relaxed);
        if(ptr) {
            void* next = *reinterpret_cast<void**>(ptr);
            *reinterpret_cast<void**>(ptr) = nullptr;
            CentralFreeList[index].store(nullptr, std::memory_order_release);
        }
        else{
            size_t blockSize = (index + 1) * alignment;
            ptr = getPageCache(blockSize);
            if(!ptr) {
                throw std::bad_alloc();
            }
            char* start = static_cast<char*>(ptr);
            size_t numBlock = (SpanPage * PageSize) / blockSize;
            for(size_t i = 0; i < numBlock - 1; ++i) {
                void* currBlock = start + i * blockSize;
                void* nextBlock = start + (i + 1) * blockSize;
                *reinterpret_cast<void**>(currBlock) = nextBlock;
            }
            void* lastBlock = start + (numBlock - 1) * blockSize;
            *reinterpret_cast<void**>(lastBlock) = nullptr; 
        }
    }
    catch(...){
        CentralFreeListLock[index].clear(std::memory_order_release);
        throw; 
    }
    CentralFreeListLock[index].clear(std::memory_order_release);
    return ptr;
}

void CentralCache::returnCentralCache(void* ptr, size_t index, size_t count) {
    if(index > MaxIndex || !ptr || count == 0) {
        return;
    }

    void* start = ptr;
    while(ptr != nullptr && count > 0) {
        count--;
        ptr = *reinterpret_cast<void**>(ptr);
    }

    while(CentralFreeListLock[index].test_and_set(std::memory_order_acquire)) {
        std::this_thread::yield(); 
    }

    try{
        void* current = CentralFreeList[index].load(std::memory_order_relaxed);
        *reinterpret_cast<void**>(ptr) = current;
        CentralFreeList[index].store(start, std::memory_order_release);
    }
    catch(...) {
        CentralFreeListLock[index].clear(std::memory_order_release);
        throw; 
    }
    CentralFreeListLock[index].clear(std::memory_order_release);
}

void* CentralCache::getPageCache(size_t size) {
    if ( size <= PageSize * SpanPage) {
        //PageCache
        return PageCache::get()->getPageCache(SpanPage);
    }
    else{
        // PageCache (size + PageSize - 1) / PageSize
        return PageCache::get()->getPageCache((size + PageSize - 1) / PageSize);
    }
}