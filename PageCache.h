#pragma once
#include "helper.h"
#include <mutex>
#include <map>
class Span{
public:
    Span(void* start, size_t pageNum);
    ~Span();
    void* startAdd;
    size_t PageNum;
    Span* next;

};

class PageCache {
public:
    static PageCache* get() {
        static PageCache pageCache;
        return &pageCache;
    }

    void* getPageCache(size_t PageNum);
    void returnPageCache(void*ptr, size_t PageNum);
private:
    PageCache() = default;
    Span* Alloc(size_t PageNum);
    std::mutex mutex_;
    std::map<size_t, Span*> spanMap;
    std::map<void*, Span*> FreeSpanMap;
};