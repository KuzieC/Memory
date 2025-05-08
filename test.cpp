#include "MemoryPool.h"
#include <iostream>
#include <chrono>
#include <cstring>
#include <vector>
#include <new> // For placement new

// A custom data type for testing
struct CustomData {
    int id;
    double value;
    char name[32];

    CustomData(int i, double v, const char* n) : id(i), value(v) {
        strncpy(name, n, sizeof(name));
        name[sizeof(name) - 1] = '\0';
    }
};

int main() {
    const size_t numObjects = 1000000; // Number of objects to allocate
    std::vector<CustomData*> objects;

    // Test 1: Using standard operator new
    auto start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < numObjects; ++i) {
        objects.push_back(new CustomData(i, i * 0.1, "Standard"));
    }
    for (auto obj : objects) {
        delete obj;
    }
    objects.clear();
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "Standard operator new: "
              << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
              << " ms" << std::endl;

    // Test 2: Using custom memory pool
    MemoryPool pool;
    start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < numObjects; ++i) {
        void* memory = pool.allocate(sizeof(CustomData), alignof(CustomData));
        objects.push_back(new (memory) CustomData(i, i * 0.1, "MemoryPool"));
    }
    for (auto obj : objects) {
        obj->~CustomData(); // Call destructor explicitly
    }
    pool.deallocate();
    end = std::chrono::high_resolution_clock::now();
    std::cout << "Custom memory pool: "
              << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
              << " ms" << std::endl;

    return 0;
}