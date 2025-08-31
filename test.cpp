/**
 * @file test.cpp
 * @brief Performance benchmarking program for the memory allocation system
 * 
 * This program benchmarks the performance of the custom memory pool allocator
 * against standard operator new/delete across multiple thread counts and
 * allocation sizes to demonstrate performance improvements.
 */

#include <iostream>
#include <thread>
#include <vector>
#include <atomic>
#include <chrono>
#include "MemoryBucket.h"

/** @brief Total number of allocations per benchmark run */
constexpr size_t TOTAL_ALLOCATIONS = 100000;

/**
 * @brief Small test object for performance testing
 */
class P1 { 
    /** @brief Single integer member for minimal size */
    int id_; 
};

/**
 * @brief Large test object for performance testing
 */
class P5 { 
    /** @brief Large data array to test large object allocation */
    char data[1024]; 
};

/**
 * @brief Enumeration for allocation method selection
 */
enum class AllocType { 
    Pool,      /**< Use custom memory pool */
    NewDelete  /**< Use standard new/delete */
};

/**
 * @brief Generic benchmarking function for memory allocation performance
 * @param nthreads Number of threads to use for benchmark
 * @param total_allocs Total number of allocations to perform
 * @param rounds Number of benchmark rounds to average
 * @param large Whether to use large objects (P5) or small objects (P1)
 * @param allocType Allocation method to benchmark
 * @return Total time in milliseconds for all allocations
 */
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
                            MemoryBucket::deallocate(p, sizeof(P5));
                        } else {
                            P1* p = static_cast<P1*>(MemoryBucket::allocate(sizeof(P1)));
                            new (p) P1();
                            p->~P1();
                            MemoryBucket::deallocate(p, sizeof(P1));
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

/**
 * @brief Run comprehensive performance test sweep
 * 
 * Executes benchmarks across different thread counts and object sizes,
 * comparing custom memory pool performance against standard allocation.
 */
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

/**
 * @brief Main function - entry point for benchmark program
 * @return Exit status code
 */
int main() {
    MemoryBucket::initMemoryPool();
    RunTestSweep();
    return 0;
}
