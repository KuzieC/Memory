#include <iostream>
#include <thread>
#include <vector>
#include <atomic>
#include "MemoryBucket.h"

// Constants for test configuration
constexpr size_t TEST_NTIMES = 10000;
constexpr size_t TEST_NWORKS = 10;
constexpr size_t TEST_ROUNDS = 100;

class P1 { int id_; };
class P2 { int id_[5]; };
class P3 { int id_[10]; };
class P4 { int id_[20]; };
// Large object > 512 bytes
class P5 { char data[1024]; };

void BenchmarkMemoryPoolSmall(size_t ntimes, size_t nworks, size_t rounds)
{
    std::vector<std::thread> vthread(nworks);
    std::atomic<size_t> total_costtime{0};
    for (size_t k = 0; k < nworks; ++k)
    {
        vthread[k] = std::thread([&]() {
            for (size_t j = 0; j < rounds; ++j)
            {
                size_t begin1 = clock();
                for (size_t i = 0; i < ntimes; i++)
                {
                    P1* p1 = static_cast<P1*>(MemoryBucket::allocate(sizeof(P1)));
                    new (p1) P1();
                    p1->~P1();
                    MemoryBucket::deallcoate(p1, sizeof(P1));

                    P2* p2 = static_cast<P2*>(MemoryBucket::allocate(sizeof(P2)));
                    new (p2) P2();
                    p2->~P2();
                    MemoryBucket::deallcoate(p2, sizeof(P2));

                    P3* p3 = static_cast<P3*>(MemoryBucket::allocate(sizeof(P3)));
                    new (p3) P3();
                    p3->~P3();
                    MemoryBucket::deallcoate(p3, sizeof(P3));

                    P4* p4 = static_cast<P4*>(MemoryBucket::allocate(sizeof(P4)));
                    new (p4) P4();
                    p4->~P4();
                    MemoryBucket::deallcoate(p4, sizeof(P4));
                }
                size_t end1 = clock();
                total_costtime += end1 - begin1;
            }
        });
    }
    for (auto& t : vthread)
    {
        t.join();
    }
    printf("[Memory pool - Small Objects] %lu threads, %lu rounds, %lu alloc/free per round, total time: %lu ms\n", nworks, rounds, ntimes, total_costtime.load());
}

void BenchmarkMemoryPoolLarge(size_t ntimes, size_t nworks, size_t rounds)
{
    std::vector<std::thread> vthread(nworks);
    std::atomic<size_t> total_costtime{0};
    for (size_t k = 0; k < nworks; ++k)
    {
        vthread[k] = std::thread([&]() {
            for (size_t j = 0; j < rounds; ++j)
            {
                size_t begin1 = clock();
                for (size_t i = 0; i < ntimes; i++)
                {
                    P5* p5 = static_cast<P5*>(MemoryBucket::allocate(sizeof(P5)));
                    new (p5) P5();
                    p5->~P5();
                    MemoryBucket::deallcoate(p5, sizeof(P5));
                }
                size_t end1 = clock();
                total_costtime += end1 - begin1;
            }
        });
    }
    for (auto& t : vthread)
    {
        t.join();
    }
    printf("[Memory pool - Large Objects] %lu threads, %lu rounds, %lu alloc/free per round, total time: %lu ms\n", nworks, rounds, ntimes, total_costtime.load());
}

void BenchmarkNewSmall(size_t ntimes, size_t nworks, size_t rounds)
{
    std::vector<std::thread> vthread(nworks);
    std::atomic<size_t> total_costtime{0};
    for (size_t k = 0; k < nworks; ++k)
    {
        vthread[k] = std::thread([&]() {
            for (size_t j = 0; j < rounds; ++j)
            {
                size_t begin1 = clock();
                for (size_t i = 0; i < ntimes; i++)
                {
                    P1* p1 = new P1;
                    delete p1;
                    P2* p2 = new P2;
                    delete p2;
                    P3* p3 = new P3;
                    delete p3;
                    P4* p4 = new P4;
                    delete p4;
                }
                size_t end1 = clock();
                total_costtime += end1 - begin1;
            }
        });
    }
    for (auto& t : vthread)
    {
        t.join();
    }
    printf("[new/delete - Small Objects] %lu threads, %lu rounds, %lu alloc/free per round, total time: %lu ms\n", nworks, rounds, ntimes, total_costtime.load());
}

void BenchmarkNewLarge(size_t ntimes, size_t nworks, size_t rounds)
{
    std::vector<std::thread> vthread(nworks);
    std::atomic<size_t> total_costtime{0};
    for (size_t k = 0; k < nworks; ++k)
    {
        vthread[k] = std::thread([&]() {
            for (size_t j = 0; j < rounds; ++j)
            {
                size_t begin1 = clock();
                for (size_t i = 0; i < ntimes; i++)
                {
                    P5* p5 = new P5;
                    delete p5;
                }
                size_t end1 = clock();
                total_costtime += end1 - begin1;
            }
        });
    }
    for (auto& t : vthread)
    {
        t.join();
    }
    printf("[new/delete - Large Objects] %lu threads, %lu rounds, %lu alloc/free per round, total time: %lu ms\n", nworks, rounds, ntimes, total_costtime.load());
}

int main()
{
    MemoryBucket::initMemoryPool();
    BenchmarkMemoryPoolSmall(TEST_NTIMES, TEST_NWORKS, TEST_ROUNDS);
    BenchmarkMemoryPoolLarge(TEST_NTIMES, TEST_NWORKS, TEST_ROUNDS);
    std::cout << "===========================================================================" << std::endl;
    BenchmarkNewSmall(TEST_NTIMES, TEST_NWORKS, TEST_ROUNDS);
    BenchmarkNewLarge(TEST_NTIMES, TEST_NWORKS, TEST_ROUNDS);
    return 0;
}