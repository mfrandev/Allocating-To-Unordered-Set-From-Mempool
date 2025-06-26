#ifndef MY_ALLOCATOR_H_
#define MY_ALLOCATOR_H_

#include <cstdint>

// Determine size of std::unordered_map we're analyzing
constexpr uint64_t MAP_SIZE = 1'000'000;

/**
 * Custom allocator wrapper for the mempool class
 */
template<typename T>
class MyAllocator {
    public:

        // stdlib needs this internally
        using value_type = T;

        MyAllocator() noexcept = default;

        // Rebind support for U -> T construction
        template<typename U>
        MyAllocator(const MyAllocator<U>& other) noexcept {}

        // Allocate from the memory pool (i.e., pop)
        T* allocate(size_t size) {
            return _mempool.allocate();
        }

        // Deallocate back to the memory pool (i.e., push)
        void deallocate(T* ptr, size_t size) noexcept {
            _mempool.deallocate(ptr);
        }   

        // Trivially true because all instances of "MyAllocator" share the same Mempool (static)
        template<typename U>
        bool operator ==(const MyAllocator<U>& other) const noexcept {
            return true;
        }

        // Likewise, this will trivially be false for all cases
        template<typename U>
        bool operator !=(const MyAllocator<U>& other) const noexcept {
            return !(*this == other);
        }

    private:

        // MAP_SIZE + 1 because mempool ring buffer implementation relies on keeping one place empty to detect "fullness"
        static Mempool<T, MAP_SIZE + 1> _mempool;
};

#endif // MY_ALLOCATOR_H_