#include "MemoryPool.h"


size_t MemoryPool::alighPad(char* cur, size_t align) {
    return (align - (reinterpret_cast<uintptr_t>(cur) % align))%align;
}
void* MemoryPool::allocate(size_t n, size_t align) {
    //check if we can fit n in the current block
    if(!currBlock || currBlock + n >= endBlock) {
        Block* newBlock = static_cast<Block*>(operator new(BLOCK_SIZE));
        //link fristBlock pointer
        newBlock->next = firstBlock;
        firstBlock = newBlock;   
        char* newCurr = reinterpret_cast<char*>(newBlock) + sizeof(Block);
        size_t padding = alighPad(newCurr, align);
        currBlock = newCurr + padding;
        endBlock = reinterpret_cast<char*>(newBlock) + BLOCK_SIZE;
    }

    void* ret = currBlock;

    char* newCurr = reinterpret_cast<char*>(currBlock) + n;
    size_t padding = alighPad(newCurr, align);
    currBlock = reinterpret_cast<char*>(newCurr + padding); //move curr pointer

    return ret; //return the pointer to the allocated memory

    //move curr and end pointer

    //add padding and move curr
}

void MemoryPool::deallocate() {
    while(firstBlock) {
        auto next = firstBlock->next;
        operator delete(firstBlock);
        firstBlock = next;
    }
}