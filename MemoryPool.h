#pragma once

#include <cstdint>
struct Block {
    Block* next;
};

static const size_t BLOCK_SIZE = 4096;
static const size_t SLOT_SIZE = 8;
class MemoryPool{
    uint8_t* currBlock = nullptr;
    uint8_t* endBlock = nullptr;
    Block* firstBlock = nullptr;

    void* allocate(size_t n, size_t align);
    void deallocate();
    size_t alighPad(char* cur, size_t align);
    ~MemoryPool() {
        deallocate();
    }
};
