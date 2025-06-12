#include "ThreadCache.h"
#include "CentralCache.h"
void* ThreadCache::allocate(size_t size){
    if(size <= 0){
        return nullptr; 
    }

    if(size > MaxSize){
        return ::operator new(size);
    }

    size_t index = getIndex(size);

    if(auto ptr = freeList[index]){
        freeListSize[index]--;
        freeList[index] = *reinterpret_cast<void**>(ptr);
        return ptr;
    }

    return getCentralCache(index);
}

void ThreadCache::deallocate(void* ptr, size_t size){
    if(size <= 0 || !ptr) return;

    if(size > MaxSize){
        ::operator delete(ptr);
        return;
    }

    size_t index = getIndex(size);

    *reinterpret_cast<void**>(ptr) = freeList[index];
    freeList[index] = ptr;
    freeListSize[index]++;

    if(freeListSize[index] > 64){
        returnCentralCache(freeList[index], size);
    }
}   

void ThreadCache::returnCentralCache(void* ptr, size_t size){
    size_t index = getIndex(size);
    size_t totalNum = freeListSize[index];
    size_t keptNum = totalNum / 4;
    
    void* end = ptr;
    for(size_t i = 0; i < keptNum; ++i){
        end = *reinterpret_cast<void**>(end);
    }

    void* returnPtr = *reinterpret_cast<void**>(end);
    *reinterpret_cast<void**>(end) = nullptr;
    freeList[index] = end;
    freeListSize[index] = keptNum;
    if(returnPtr){
        // Return the rest of the memory to the central cache
        CentralCache::get()->returnCentralCache(returnPtr, index, totalNum - keptNum);
    }
}

void* ThreadCache::getCentralCache(size_t index){
    void* start = CentralCache::get()->getCentralCache(index);
    if(!start) {
        throw std::bad_alloc();
    }
    void* next = *reinterpret_cast<void**>(start);
    *reinterpret_cast<void**>(start) = nullptr;
    freeList[index] = next;
    size_t count = 0;
    while(next) {
        count++;
        next = *reinterpret_cast<void**>(next);
    }
    freeListSize[index] = count;
    return start;
}