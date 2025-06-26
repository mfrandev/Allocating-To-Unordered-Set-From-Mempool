#include <unordered_map>

#include "Mempool.h"
#include "MyClass.h"
#include "MyAllocator.h"

// MAP_SIZE + 1 because mempool ring buffer implementation relies on keeping one place empty to detect "fullness"
template<typename T>
Mempool<T, MAP_SIZE + 1> MyAllocator<T>::_mempool;

/**
 * Use the custom allocator to allocate mempool managed memory to the unordered map, rather than heap memory.
 */
void noHeapAlloc() {
    std::unordered_map
    <
        uint64_t, 
        MyClass, 
        std::hash<uint64_t>, 
        std::equal_to<uint64_t>, 
        MyAllocator<std::pair<const uint64_t, MyClass>>
    > noAlloc;
    
    for(uint64_t i = 0; i < MAP_SIZE; i++) {
        noAlloc.emplace(i, MyClass(i, i+3));
    }

    for(uint64_t i = 0; i < MAP_SIZE; i++) {
        noAlloc.erase(i);
        noAlloc.emplace(i, MyClass(i, i+3));
    }
}

/**
 * std::unordered_map uses the default allocator.
 * Note: constexpr uint64_t MAP_SIZE = 1000000UL;
 */
void heapAlloc() {
    std::unordered_map<uint64_t, MyClass> alloc;

    for(uint64_t i = 0; i < MAP_SIZE; i++) {
        alloc.emplace(i, MyClass(i, i+3));
    }

    for(uint64_t i = 0; i < MAP_SIZE; i++) {
        alloc.erase(i);
        alloc.emplace(i, MyClass(i, i+3));
    }
}

/**
 * Run both versions of the map operations function for analysis
 */
int main() {

    noHeapAlloc();
    heapAlloc();

    return 0;
}