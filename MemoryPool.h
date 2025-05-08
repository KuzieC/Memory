#pragma once

#include <cstdint>
struct Block {
    Block* next;
};

static const size_t BLOCK_SIZE = 4096;
static const size_t SLOT_SIZE = 8;
class MemoryPool{
    char* currBlock = nullptr;
    char* endBlock = nullptr;
    Block* firstBlock = nullptr;
    size_t alighPad(char* cur, size_t align);
public:
    void* allocate(size_t n, size_t align);
    void deallocate();

    ~MemoryPool() {
        deallocate();
    }
};
