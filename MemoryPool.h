#pragma once

struct Block {
    Block* next;
};

class MemoryPool{
    Block* currBlock;
    Block* endBlock;
    Block* firstBlock;

    void* allocate(size_t n);
    void deallocate();
};
