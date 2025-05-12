#include "MemoryBucket.h"
#include <iostream>
#include <chrono>
#include <vector>
#include <new>
#include <cstring>

// A small custom data type
struct SmallData {
    int a;
    char b;
    SmallData(int x, char y) : a(x), b(y) {}
};

// A large custom data type
struct LargeData {
    double arr[128];
    char name[128];
    LargeData(double v, const char* n) {
        for (int i = 0; i < 128; ++i) arr[i] = v + i;
        strncpy(name, n, sizeof(name));
        name[sizeof(name) - 1] = '\0';
    }
};

int main() {
    const size_t numObjects = 1000000;
    std::vector<SmallData*> smallObjs;
    std::vector<LargeData*> largeObjs;

    // Initialize the memory pool bucket
    MemoryBucket::initMemoryPool();

    // --- Small Object Test: Standard operator new ---
    std::cout << "\n[Small Object Test: Standard operator new]" << std::endl;
    auto start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < numObjects; ++i) {
        smallObjs.push_back(new SmallData(i, 'a'));
    }
    for (auto obj : smallObjs) {
        delete obj;
    }
    smallObjs.clear();
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "Standard operator new: "
              << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
              << " ms" << std::endl;

    // --- Small Object Test: MemoryBucket ---
    std::cout << "[Small Object Test: MemoryBucket]" << std::endl;
    start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < numObjects; ++i) {
        void* memory = MemoryBucket::allocate(sizeof(SmallData));
        smallObjs.push_back(new (memory) SmallData(i, 'a'));
    }
    for (auto obj : smallObjs) {
        obj->~SmallData();
        MemoryBucket::deallcoate(obj, sizeof(SmallData));
    }
    smallObjs.clear();
    end = std::chrono::high_resolution_clock::now();
    std::cout << "MemoryBucket custom pool: "
              << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
              << " ms" << std::endl;

    // --- Large Object Test: Standard operator new ---
    std::cout << "\n[Large Object Test: Standard operator new]" << std::endl;
    start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < numObjects; ++i) {
        largeObjs.push_back(new LargeData(i * 0.1, "LargeObject"));
    }
    for (auto obj : largeObjs) {
        delete obj;
    }
    largeObjs.clear();
    end = std::chrono::high_resolution_clock::now();
    std::cout << "Standard operator new: "
              << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
              << " ms" << std::endl;

    // --- Large Object Test: MemoryBucket ---
    std::cout << "[Large Object Test: MemoryBucket]" << std::endl;
    start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < numObjects; ++i) {
        void* memory = MemoryBucket::allocate(sizeof(LargeData));
        largeObjs.push_back(new (memory) LargeData(i * 0.1, "LargeObject"));
    }
    for (auto obj : largeObjs) {
        obj->~LargeData();
        MemoryBucket::deallcoate(obj, sizeof(LargeData));
    }
    largeObjs.clear();
    end = std::chrono::high_resolution_clock::now();
    std::cout << "MemoryBucket custom pool: "
              << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
              << " ms" << std::endl;

    return 0;
}