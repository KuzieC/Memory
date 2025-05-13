#include "MemoryPool.h"

MemoryPool::MemoryPool()
    : SLOT_SIZE(0)
    , freeList(nullptr)
    , currBlock(nullptr)
    , endBlock(nullptr)
    , firstBlock(nullptr){

}


void MemoryPool::init(size_t size) {
    SLOT_SIZE = size;
    firstBlock = nullptr;
    currBlock = nullptr;
    endBlock = nullptr;
    freeList = nullptr;
}
size_t MemoryPool::alighPad(char* cur, size_t align) {
    return (align - (reinterpret_cast<uintptr_t>(cur) % align))%align;
}

void* MemoryPool::popFreeList() {
    auto oldHead = freeList.load(std::memory_order_acquire);
    if(oldHead == nullptr) return nullptr;
    Slot* newHead = oldHead->next.load(std::memory_order_relaxed);
    while(!freeList.compare_exchange_weak(oldHead, newHead,
        std::memory_order_acquire, std::memory_order_relaxed)){
            if(oldHead == nullptr) return nullptr;
            newHead = oldHead->next.load(std::memory_order_relaxed);
    }
    return oldHead;
}

void MemoryPool::pushFreeList(Slot* b) {
    auto oldHead = freeList.load(std::memory_order_relaxed);
    b->next.store(oldHead, std::memory_order_relaxed);
    while(!freeList.compare_exchange_weak(oldHead, b,
        std::memory_order_release, std::memory_order_relaxed)){
            b->next.store(oldHead, std::memory_order_relaxed);
    }
}

void* MemoryPool::allocate(size_t n, size_t align) {
    //check if we can fit n in the current Slot
    if( n > BLOCK_SIZE - sizeof(SLOT_SIZE)) {
        return operator new(n);
    }
    void* newFreeBlock = popFreeList();
    if(newFreeBlock != nullptr){
        return newFreeBlock;
    }
    std::lock_guard<std::mutex> lock(firstBlockMutex);
    if(!currBlock || currBlock + n >= endBlock) {
        Slot* newBlock = static_cast<Slot*>(operator new(BLOCK_SIZE));
        //link fristBlock pointer
        newBlock->next = firstBlock;
        firstBlock = newBlock;   
        char* newCurr = reinterpret_cast<char*>(newBlock) + sizeof(Slot);
        size_t padding = alighPad(newCurr, align);
        currBlock = newCurr + padding;
        endBlock = reinterpret_cast<char*>(newBlock) + BLOCK_SIZE;
        //freeList = nullptr;
    }

    void* ret = currBlock;

    char* newCurr = reinterpret_cast<char*>(currBlock) + n;
    size_t padding = alighPad(newCurr, align);
    currBlock = reinterpret_cast<char*>(newCurr + padding); //move curr pointer

    return ret; //return the pointer to the allocated memory

    //move curr and end pointer

    //add padding and move curr
}

void MemoryPool::deallocate(Slot* b) {
    if(!b) return;
    pushFreeList(b);
}

// void MemoryPool::deallocateAll() {
//     while(firstBlock) {
//         auto next = firstBlock->next;
//         operator delete(firstBlock);
//         firstBlock = next;
//     }
// }