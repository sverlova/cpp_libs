#include <iostream>
#include <vector>
#include <memory>
#include <stdexcept>
#include <cmath>

template<typename T, typename Allocator>
class _List;

template<typename Key, typename Value, typename Hash = std::hash<Key>,
        typename Equal = std::equal_to<Key>, typename Alloc =
        std::allocator<std::pair<const Key, Value>>>
class UnorderedMap {
  public:
    using NodeType = std::pair<const Key, Value>;

  private:
    using BucketItem = typename _List<NodeType, Alloc>::iterator;
    using BucketAlloc = typename std::allocator_traits<Alloc>::template rebind_alloc<BucketItem>;
    using AllocTraits = std::allocator_traits<BucketAlloc>;

    using ListType = _List<NodeType, Alloc>;

    template<bool isConst>
    struct common_iterator {
      private:
        typename ListType::iterator list_node;
      public:
        using difference_type = std::ptrdiff_t;
        using value_type = std::conditional_t<isConst, const NodeType, NodeType>;
        using pointer = std::conditional_t<isConst, const NodeType*, NodeType*>;
        using reference = std::conditional_t<isConst, const NodeType&, NodeType&>;
        using iterator_category = std::forward_iterator_tag;

        common_iterator();
        common_iterator(typename ListType::iterator it);
        common_iterator(const common_iterator<false>& it);
        bool operator==(common_iterator<isConst> x);
        bool operator!=(common_iterator<isConst> x);
        reference operator*();
        pointer operator->();
        common_iterator<isConst>& operator++();
        common_iterator<isConst> operator++(int);

        friend class UnorderedMap<Key, Value, Hash, Equal, Alloc>;
    };

  public:
    using Iterator = common_iterator<false>;
    using ConstIterator = common_iterator<true>;

    UnorderedMap();
    UnorderedMap(const UnorderedMap& another);
    UnorderedMap(UnorderedMap&& another);
    UnorderedMap<Key, Value, Hash, Equal, Alloc>& operator=(const UnorderedMap& another);
    UnorderedMap<Key, Value, Hash, Equal, Alloc>& operator=(UnorderedMap&& another);
    ~UnorderedMap();

    Value& operator[](const Key& key);
    Value& at(const Key& key);
    Value at(const Key& key) const;

    size_t size() const;

    Iterator begin();
    Iterator end();
    ConstIterator cbegin() const;
    ConstIterator cend() const;

    std::pair<Iterator, bool> insert(const NodeType& x);
    std::pair<Iterator, bool> insert(NodeType&& x);
    template<typename P>
    std::pair<Iterator, bool> insert(P&& x);
    template<typename InputIterator>
    void insert(InputIterator begin, InputIterator end);
    template<typename... Args>
    std::pair<Iterator, bool> emplace(Args&&... args);

    template<bool isConst>
    Iterator erase(common_iterator<isConst> it);
    template<bool isConst>
    Iterator erase(common_iterator<isConst> begin, common_iterator<isConst> end);

    Iterator find(const Key& key);

    void reserve(size_t n);
    size_t max_size() const;
    float load_factor() const;
    float max_load_factor() const;
    void max_load_factor(float x);

  private:
    BucketAlloc alloc;
    size_t bucket_count;
    size_t _size;
    float _max_load_factor;
    ListType items;
    BucketItem* pool;

    inline static const size_t START_BUCKET_COUNT = 1009;
    inline static const float STANDART_MAX_LOAD_FACTOR = 0.35;
    inline static const size_t MAX_SIZE = UINT32_MAX;

    void rehash(size_t min_bucket_count);
};
