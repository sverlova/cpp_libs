#include <iostream>
#include <vector>

template<size_t chunkSize>
class FixedAllocator {
  public:
    static FixedAllocator<chunkSize>& get_instance();
    ~FixedAllocator();

    template<size_t anotherSize>
    bool operator==(const FixedAllocator<anotherSize>& another);

    void* allocate();
    void deallocate(void* ptr);
  private:
    FixedAllocator();

    void add_pool();

    static const size_t POOL_SIZE = 1000;
    std::vector<void*> pools;
    std::vector<void*> next_free;
};

template<typename T>
class FastAllocator {
  public:
    FastAllocator();
    template<typename U>
    FastAllocator(const FastAllocator<U>&);
    FastAllocator<T>& operator=(const FastAllocator<T>&);

    ~FastAllocator();

    template<typename U>
    bool operator==(const FastAllocator<U>& another);
    template<typename U>
    bool operator!=(const FastAllocator<U>& another);

    T* allocate(size_t n);
    void deallocate(T* ptr, size_t n);

    using value_type = T;
  private:
    inline static const size_t FIXED_ALLOC_SIZE_1 = 8;
    inline static const size_t FIXED_ALLOC_SIZE_2 = 24;
    inline static const size_t FIXED_ALLOC_SIZE_3 = 48;
    FixedAllocator<FIXED_ALLOC_SIZE_1>& fixed_alloc_1;
    FixedAllocator<FIXED_ALLOC_SIZE_2>& fixed_alloc_2;
    FixedAllocator<FIXED_ALLOC_SIZE_3>& fixed_alloc_3;

    template<typename U>
    friend class FastAllocator;
};
