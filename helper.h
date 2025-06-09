#include <stdexcept>
constexpr size_t MaxSize = 256 * 1024; // 256 KB
constexpr size_t alignment = sizeof(void*);
constexpr size_t MaxIndex = MaxSize / alignment;
size_t getIndex(size_t size){
    if(size <= 0){
        throw std::invalid_argument("Size must be greater than 0");
    }

    return (size - 1) / alignment;
}