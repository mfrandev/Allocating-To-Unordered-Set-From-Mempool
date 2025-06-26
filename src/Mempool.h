#ifndef MEMPOOL_H_
#define MEMPOOL_H_

#include <cmath>
#include <cassert>
#include <atomic>

/**
     * Memory Pool for static allocation of frequently produced messages in the SPSC model.
     * To avoid needing mutex locks, hold invariant that MempoolSize must not exceed the size of the queue.
     * See https://rigtorp.se/ringbuffer/ for lockfree ring buffer implementation reference.
     */
    template <typename T, const size_t MempoolSize>
    class Mempool {

        private:
            alignas(T) std::byte buffer[sizeof(T) * MempoolSize];
            T* isFreeBufferMap[MempoolSize];

            // On AMD64/x86_64 and ARM CPUs, cache lines are 64 bytes
            alignas(64) std::atomic<unsigned long> _allocatorIndex{0};
            alignas(64) unsigned long _allocatorIndexCached{0}; 
            alignas(64) std::atomic<unsigned long> _deallocatorIndex{0};
            alignas(64) unsigned long _deallocatorIndexCached{0};

            // If 2, then this type is always lock free, not determined at runtime
            static_assert(ATOMIC_LONG_LOCK_FREE == 2);

        public:
            Mempool() {
                for(size_t i = 0; i < MempoolSize; i++)
                    isFreeBufferMap[i] = reinterpret_cast<T*>(buffer + (i * sizeof(T)));
            }

            /**
             * Return a pointer to a readily available resource
             */
            T* allocate() {
                auto const allocatorIndex = _allocatorIndex.load(std::memory_order_relaxed);
                auto nextAllocatorIndex = allocatorIndex + 1;
                if(nextAllocatorIndex == MempoolSize) 
                    nextAllocatorIndex = 0;
                if(nextAllocatorIndex == _deallocatorIndexCached) {
                    _deallocatorIndexCached = _deallocatorIndex.load(std::memory_order_acquire);
                    if(_deallocatorIndexCached == nextAllocatorIndex) return nullptr;
                }
                T* resource = isFreeBufferMap[allocatorIndex];
                _allocatorIndex.store(nextAllocatorIndex, std::memory_order_release);
                return resource;
            }

            /**
             * Take an allocated resource and return it to the memory pool
             */
            void deallocate(T* item) {
                auto const deallocatorIndex = _deallocatorIndex.load(std::memory_order_relaxed);
                if(deallocatorIndex == _allocatorIndexCached) {
                    _allocatorIndexCached = _allocatorIndex.load(std::memory_order_acquire);
                    if(deallocatorIndex == _allocatorIndexCached) return;
                }
                isFreeBufferMap[deallocatorIndex] = item;
                auto nextDeallocatorIndex = deallocatorIndex + 1;
                if(nextDeallocatorIndex == MempoolSize)
                    nextDeallocatorIndex = 0;
                _deallocatorIndex.store(nextDeallocatorIndex, std::memory_order_release);
            }
    };

#endif // MEMPOOL_H_
