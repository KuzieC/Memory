#include "MemoryBucket.h"

MemoryPool MemoryBucket::memorypool[64];

void MemoryBucket::initMemoryPool() {
    for (int i = 0; i < 64; ++i) {
        MemoryBucket::memorypool[i].init((i + 1) * 8); 
    }
}

void* MemoryBucket::allocate(size_t size) {
    if(size <= 0) return nullptr;
    if(size > 512) return operator new(size);
    size_t index = (size + 7) / 8 - 1; 
    return MemoryBucket::memorypool[index].allocate(size);
}

void MemoryBucket::deallcoate(void* ptr, size_t size) {
    if(!ptr) return;
    if(size > 512) {
        operator delete(ptr);
        return;
    }
    size_t index = (size+7) / 8 - 1;
    MemoryBucket::memorypool[index].deallocate(static_cast<Slot*>(ptr));
}