#include "fastallocator.h"

// *****
// FIXED ALLOC
// *****

template<size_t chunkSize>
FixedAllocator<chunkSize>& FixedAllocator<chunkSize>::get_instance() {
    static FixedAllocator<chunkSize> _alloc;
    return _alloc;
}

template<size_t chunkSize>
FixedAllocator<chunkSize>::FixedAllocator(): pools(std::vector<void*>(0)),
        next_free(std::vector<void*>(0)) {
    add_pool();
}

template<size_t chunkSize>
FixedAllocator<chunkSize>::~FixedAllocator() {
    for (size_t i = 0; i < pools.size(); ++i) {
        ::operator delete[](pools[i], POOL_SIZE);
    }
}

// *****

template<size_t chunkSize>
template<size_t anotherSize>
bool FixedAllocator<chunkSize>::operator==(const FixedAllocator<anotherSize>& another) {
    return chunkSize == anotherSize;
}

template<size_t chunkSize>
void* FixedAllocator<chunkSize>::allocate() {
    if (next_free.empty()) {
        add_pool();
    }
    void* ret = next_free.back();
    next_free.pop_back();
    return ret;
}

template<size_t chunkSize>
void FixedAllocator<chunkSize>::deallocate(void* ptr) {
    next_free.push_back(ptr);
}

template<size_t chunkSize>
void FixedAllocator<chunkSize>::add_pool() {
    pools.push_back(::operator new(POOL_SIZE * chunkSize));
    for (size_t i = 0; i < POOL_SIZE; ++i) {
        next_free.push_back(reinterpret_cast<void*>(reinterpret_cast<char*>(pools.back())
                + i * chunkSize * sizeof(char)));
    }
}

// *****
// FAST ALLOC
// *****

template<typename T>
FastAllocator<T>::FastAllocator():
        fixed_alloc_1(FixedAllocator<FIXED_ALLOC_SIZE_1>::get_instance()),
        fixed_alloc_2(FixedAllocator<FIXED_ALLOC_SIZE_2>::get_instance()),
        fixed_alloc_3(FixedAllocator<FIXED_ALLOC_SIZE_3>::get_instance()) {}

template<typename T>
template<typename U>
FastAllocator<T>::FastAllocator(const FastAllocator<U>&): FastAllocator<T>() {}

template<typename T>
FastAllocator<T>& FastAllocator<T>::operator=(const FastAllocator<T>&) {
    return *this;
}


template<typename T>
FastAllocator<T>::~FastAllocator() = default;

// *****

template<typename T>
template<typename U>
bool FastAllocator<T>::operator==(const FastAllocator<U>& another) {
    return true;
}

template<typename T>
template<typename U>
bool FastAllocator<T>::operator!=(const FastAllocator<U>& another) {
    return !(*this == another);
}

template<typename T>
T* FastAllocator<T>::allocate(size_t n) {
    size_t byte_count = n * sizeof(T);
    if (byte_count <= FIXED_ALLOC_SIZE_1) {
        return reinterpret_cast<T*>(fixed_alloc_1.allocate());
    } else if (byte_count <= FIXED_ALLOC_SIZE_2) {
        return reinterpret_cast<T*>(fixed_alloc_2.allocate());
    } else if (byte_count <= FIXED_ALLOC_SIZE_3) {
        return reinterpret_cast<T*>(fixed_alloc_3.allocate());
    } else {
        return reinterpret_cast<T*>(::operator new(byte_count));
    }
}

template<typename T>
void FastAllocator<T>::deallocate(T* ptr, size_t n) {
    size_t byte_count = n * sizeof(T);
    if (byte_count <= FIXED_ALLOC_SIZE_1) {
        fixed_alloc_1.deallocate(reinterpret_cast<void*>(ptr));
    } else if (byte_count <= FIXED_ALLOC_SIZE_2) {
        fixed_alloc_2.deallocate(reinterpret_cast<void*>(ptr));
    } else if (byte_count <= FIXED_ALLOC_SIZE_3) {
        fixed_alloc_3.deallocate(reinterpret_cast<void*>(ptr));
    } else {
        ::operator delete(ptr);
    }
}
