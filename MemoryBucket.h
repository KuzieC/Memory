#include "MemoryPool.h"

struct MemoryBucket {
    static MemoryPool memorypool[64];
    MemoryBucket();
    static void* allocate(size_t size);
    static void deallcoate(void* ptr, size_t size);
    static void initMemoryPool();
};