#pragma once
#include "helper.h"
#include <mutex>
class CentralCache {
public:
    static CentralCache* get() {
        static CentralCache CentralCache;
        return &CentralCache;
    }

    void* getCentralCache(size_t index);
    void returnCentralCache(void* ptr, size_t size);
private:
    CentralCache() {
        for (auto& slot : CentralFreeList) {
            slot.store(nullptr, std::memory_order_relaxed);
        }        
        CentralFreeListLock.fill(std::atomic_flag{});
    }
    // Atomic pointer for single pointer operation
    std::array<std::atomic<void*>, MaxIndex> CentralFreeList;
    // Spinlock for each size freelist mutual exclusioN
    std::array<std::atomic_flag, MaxIndex> CentralFreeListLock;
};  