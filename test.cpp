#include <iostream>
#include <thread>
#include <vector>
#include <atomic>
#include <chrono>
#include "MemoryBucket.h"

constexpr size_t TOTAL_ALLOCATIONS = 100000;

class P1 { int id_; };
class P5 { char data[1024]; };

enum class AllocType { Pool, NewDelete };

// Generic benchmarking function
size_t Benchmark(size_t nthreads, size_t total_allocs, size_t rounds, bool large, AllocType allocType) {
    std::atomic<size_t> total_costtime{0};
    size_t allocsPerThread = total_allocs / nthreads;
    std::vector<std::thread> threads(nthreads);

    for (size_t k = 0; k < nthreads; ++k) {
        threads[k] = std::thread([&, k]() {
            for (size_t j = 0; j < rounds; ++j) {
                auto begin = std::chrono::high_resolution_clock::now();
                for (size_t i = 0; i < allocsPerThread; ++i) {
                    if (allocType == AllocType::Pool) {
                        if (large) {
                            P5* p = static_cast<P5*>(MemoryBucket::allocate(sizeof(P5)));
                            new (p) P5();
                            p->~P5();
                            MemoryBucket::deallcoate(p, sizeof(P5));
                        } else {
                            P1* p = static_cast<P1*>(MemoryBucket::allocate(sizeof(P1)));
                            new (p) P1();
                            p->~P1();
                            MemoryBucket::deallcoate(p, sizeof(P1));
                        }
                    } else {
                        if (large) {
                            P5* p = new P5();
                            delete p;
                        } else {
                            P1* p = new P1();
                            delete p;
                        }
                    }
                }
                auto end = std::chrono::high_resolution_clock::now();
                total_costtime += std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
            }
        });
    }

    for (auto& t : threads) t.join();
    return total_costtime.load();
}

void RunTestSweep() {
    constexpr size_t rounds = 100;
    std::vector<size_t> threadCounts = {1, 2, 4, 8, 10};

    for (bool large : {false, true}) {
        std::string sizeLabel = large ? "Large" : "Small";
        std::cout << "\n=== " << sizeLabel << " Object Allocations ===\n";
        std::cout << "Threads,Pool_ms,Pool_ns_per_alloc,New_ms,New_ns_per_alloc\n";

        for (auto nthreads : threadCounts) {
            MemoryBucket::initMemoryPool();
            size_t t1 = Benchmark(nthreads, TOTAL_ALLOCATIONS, rounds, large, AllocType::Pool);
            size_t t2 = Benchmark(nthreads, TOTAL_ALLOCATIONS, rounds, large, AllocType::NewDelete);
            double ns_per_alloc_pool = (double)t1 * 1e6 / TOTAL_ALLOCATIONS / rounds;
            double ns_per_alloc_new = (double)t2 * 1e6 / TOTAL_ALLOCATIONS / rounds;
            std::cout << nthreads << "," << t1 << "," << ns_per_alloc_pool << "," << t2 << "," << ns_per_alloc_new << "\n";
        }
    }
}

int main() {
    MemoryBucket::initMemoryPool();
    RunTestSweep();
    return 0;
}
