#include "PageCache.h"
Span::Span(void* start, size_t pageNum)
    : startAdd(start), PageNum(pageNum), next(nullptr) {
    if (!startAdd) {
        throw std::bad_alloc();
    }
}

Span::~Span() {
    if (startAdd) {
        ::VirtualFree(
            startAdd,
            0,
            MEM_RELEASE
        );
        startAdd = nullptr;
    }
}
void* PageCache::getPageCache(size_t PageNum) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = spanMap.find(PageNum);

    if (it != spanMap.end()) {
        Span* span = it->second;
        if(span->next) {
            spanMap[PageNum] = span->next; 
        }
        else{
            spanMap.erase(it); 
        }
        if(span->PageNum > PageNum) {
            Span* newSpan = new Span(static_cast<char*>(span->startAdd) + PageNum * PageSize, span->PageNum - PageNum);
            span->PageNum = PageNum;
            newSpan->next = spanMap[newSpan->PageNum];
            spanMap[newSpan->PageNum] = newSpan;
        }
        FreeSpanMap[span->startAdd] = span;
        return span->startAdd;
    }
    else{
        Span* span = Alloc(PageNum);
        spanMap[PageNum] = span;
        FreeSpanMap[span->startAdd] = span;
        return span->startAdd;
    }
}

void PageCache::returnPageCache(void* ptr, size_t PageNum) {
    
}
Span* PageCache::Alloc(size_t PageNum) {
    size_t size = PageNum * PageSize;
    void* ptr = ::VirtualAlloc(
        nullptr,
        size,
        MEM_COMMIT | MEM_RESERVE,
        PAGE_READWRITE
    );
    if (!ptr) {
        throw std::bad_alloc();
    }
    Span* span = new Span(ptr, PageNum);
    return span;
}