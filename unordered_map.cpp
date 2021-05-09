#include "unordered_map.h"

template<typename T, typename Allocator = std::allocator<T>>
class _List {
  public:
    explicit _List(const Allocator& alloc = Allocator());
    _List(size_t count);
    _List(size_t count, const T& value, const Allocator& alloc = Allocator());
    _List(const _List& another);
    _List(_List&& another);
    _List& operator=(const _List& another);
    _List& operator=(_List&& another);
    ~_List();

    Allocator get_allocator() const;
    size_t size() const;

    void push_back(const T& x);
    void push_front(const T& x);
    void pop_back();
    void pop_front();

  private:
    struct Node {
        T value;
        Node* next;
        Node* prev;

        Node();
        Node(const T& value);
        Node(const Node& another);
        template<typename... Args>
        Node(Args&&... args);
    };

    using NodeAlloc = typename std::allocator_traits<Allocator>::template rebind_alloc<Node>;
    using AllocTraits = std::allocator_traits<NodeAlloc>;
    using BaseAllocTraits = std::allocator_traits<Allocator>;

    Node* base;
    NodeAlloc alloc;
    Allocator base_alloc;
    size_t _size;

    template<bool isConst>
    struct common_iterator {
      private:
        Node* adress;
      public:
        using difference_type = std::ptrdiff_t;
        using value_type = std::conditional_t<isConst, const T, T>;
        using pointer = std::conditional_t<isConst, const T*, T*>;
        using reference = std::conditional_t<isConst, const T&, T&>;
        using iterator_category = std::bidirectional_iterator_tag;

        common_iterator();
        common_iterator(Node* x);
        common_iterator(const common_iterator<false>& it);
        bool operator==(common_iterator<isConst> x);
        bool operator!=(common_iterator<isConst> x);
        reference operator*();
        pointer operator->();
        common_iterator<isConst>& operator++();
        common_iterator<isConst> operator++(int);
        common_iterator<isConst>& operator--();
        common_iterator<isConst> operator--(int);


        friend class _List<T, Allocator>;
    };
  public:
    typedef common_iterator<false> iterator;
    typedef common_iterator<true> const_iterator;

    typedef std::reverse_iterator<iterator> reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

    iterator begin() const;
    iterator end() const;
    const_iterator cbegin() const;
    const_iterator cend() const;
    reverse_iterator rbegin() const;
    reverse_iterator rend() const;
    const_reverse_iterator crbegin() const;
    const_reverse_iterator crend() const;

    template<bool isConst>
    iterator insert(common_iterator<isConst> it, const T& value);
    template<bool isConst>
    iterator insert(common_iterator<isConst> it);
    template<bool isConst, bool isConst1>
    void insert(common_iterator<isConst> it, common_iterator<isConst1> from,
            common_iterator<isConst1> to, _List& aonther);
    template<bool isConst, typename... Args>
    iterator insert(common_iterator<isConst> it, Args&&... args);
    template<bool isConst>
    void erase(common_iterator<isConst> it);
};

// *****
// _List
// *****

template<typename T, typename Allocator>
_List<T, Allocator>::_List(const Allocator& alloc): alloc(alloc), base_alloc(alloc), _size(0) {
    base = AllocTraits::allocate(this->alloc, 1);
    base->next = base->prev = base;
}

template<typename T, typename Allocator>
_List<T, Allocator>::_List(size_t count): alloc(Allocator()), base_alloc(Allocator()), _size(0) {
    base = AllocTraits::allocate(this->alloc, 1);
    base->next = base->prev = base;
    for (size_t i = 0; i < count; ++i) {
        insert(begin());
    }
}

template<typename T, typename Allocator>
_List<T, Allocator>::_List(size_t count, const T& value, const Allocator& alloc):
        alloc(alloc), base_alloc(alloc), _size(0) {
    base = AllocTraits::allocate(this->alloc, 1);
    base->next = base->prev = base;
    for (size_t i = 0; i < count; ++i) {
        insert(begin(), value);
    }
}

template<typename T, typename Allocator>
_List<T, Allocator>::_List(const _List& another): _size(0) {
    alloc = AllocTraits::select_on_container_copy_construction(another.alloc);
    base_alloc = AllocTraits::select_on_container_copy_construction(another.base_alloc);

    base = AllocTraits::allocate(alloc, 1);
    base->next = base->prev = base;
    iterator it = another.begin();
    while (it != another.end()) {
        push_back(*it);
        ++it;
    }
}

template<typename T, typename Allocator>
_List<T, Allocator>::_List(_List&& another): _size(another._size) {
    alloc = another.alloc;
    base_alloc = another.base_alloc;

    base = another.base;
    another.base = AllocTraits::allocate(another.alloc, 1);
    another.base->next = another.base->prev = another.base;
    another._size = 0;
}

template<typename T, typename Allocator>
_List<T, Allocator>& _List<T, Allocator>::operator=(const _List& another) {
    if (another.base == base) return *this;
    while (_size != 0) {
        erase(begin());
    }
    AllocTraits::deallocate(alloc, base, 1);

    if (AllocTraits::propagate_on_container_copy_assignment::value) {
        alloc = another.alloc;
    }

    base = AllocTraits::allocate(alloc, 1);
    base->next = base->prev = base;
    iterator it = another.begin();
    while (it != another.end()) {
        push_back(*it);
        ++it;
    }
    return *this;
}

template<typename T, typename Allocator>
_List<T, Allocator>& _List<T, Allocator>::operator=(_List&& another) {
    if (another.base == base) return *this;
    while (_size != 0) {
        erase(begin());
    }
    AllocTraits::deallocate(alloc, base, 1);


    alloc = another.alloc;
    base_alloc = another.base_alloc;

    base = another.base;
    another.base = AllocTraits::allocate(alloc, 1);
    another.base->next = another.base->prev = another.base;

    _size = another._size;
    another._size = 0;
    return *this;
}

template<typename T, typename Allocator>
_List<T, Allocator>::~_List() {
    while (_size != 0) {
        erase(begin());
    }
    AllocTraits::deallocate(alloc, base, 1);
}

// *****

template<typename T, typename Allocator>
Allocator _List<T, Allocator>::get_allocator() const {
    return alloc;
}

template<typename T, typename Allocator>
size_t _List<T, Allocator>::size() const {
    return _size;
}

// *****

template <typename T, typename Allocator>
void _List<T, Allocator>::push_back(const T& value) {
    insert(end(), value);
}

template <typename T, typename Allocator>
void _List<T, Allocator>::push_front(const T& value) {
    insert(begin(), value);
}

template <typename T, typename Allocator>
void _List<T, Allocator>::pop_back() {
    erase(--end());
}

template <typename T, typename Allocator>
void _List<T, Allocator>::pop_front() {
    erase(begin());
}

// *****

template<typename T, typename Allocator>
typename _List<T, Allocator>::iterator _List<T, Allocator>::begin() const {
    return iterator(base->next);
}

template<typename T, typename Allocator>
typename _List<T, Allocator>::iterator _List<T, Allocator>::end() const {
    return iterator(base);
}

template<typename T, typename Allocator>
typename _List<T, Allocator>::const_iterator _List<T, Allocator>::cbegin() const {
    return const_iterator(base->next);
}

template<typename T, typename Allocator>
typename _List<T, Allocator>::const_iterator _List<T, Allocator>::cend() const {
    return const_iterator(base);
}

template<typename T, typename Allocator>
typename _List<T, Allocator>::reverse_iterator _List<T, Allocator>::rbegin() const {
    return reverse_iterator(base);
}

template<typename T, typename Allocator>
typename _List<T, Allocator>::reverse_iterator _List<T, Allocator>::rend() const {
    return reverse_iterator(base->next);
}

template<typename T, typename Allocator>
typename _List<T, Allocator>::const_reverse_iterator _List<T, Allocator>::crbegin() const {
    return const_reverse_iterator(base);
}

template<typename T, typename Allocator>
typename _List<T, Allocator>::const_reverse_iterator _List<T, Allocator>::crend() const {
    return const_reverse_iterator(base->next);
}

// *****

template<typename T, typename Allocator>
template<bool isConst>
typename _List<T, Allocator>::iterator _List<T, Allocator>::insert(common_iterator<isConst> it, const T& value) {
    Node* node = AllocTraits::allocate(alloc, 1);
    BaseAllocTraits::construct(base_alloc, &node->value, value);
    node->prev = node->next = node;
    node->prev = it.adress->prev;
    node->next = it.adress;
    it.adress->prev = node;
    node->prev->next = node;
    ++_size;
    return iterator(node);
}

template<typename T, typename Allocator>
template<bool isConst>
typename _List<T, Allocator>::iterator _List<T, Allocator>::insert(common_iterator<isConst> it) {
    Node* node = AllocTraits::allocate(alloc, 1);
    AllocTraits::construct(alloc, node);
    node->prev = it.adress->prev;
    node->next = it.adress;
    it.adress->prev = node;
    node->prev->next = node;
    ++_size;
    return iterator(node);
}


template<typename T, typename Allocator>
template<bool isConst, bool isConst1>
void _List<T, Allocator>::insert(common_iterator<isConst> it,
        common_iterator<isConst1> from, common_iterator<isConst1> to, _List& another) {
    for (auto iter = from; iter != to; ++iter) {
        ++_size;
        --another._size;
    }
    --to;
    --it;
    from.adress->prev->next = to.adress->next;
    to.adress->next->prev = from.adress->prev;
    from.adress->prev = it.adress;
    to.adress->next = it.adress->next;
    it.adress->next = from.adress;
    to.adress->next->prev = to.adress;
}

template<typename T, typename Allocator>
template<bool isConst, typename... Args>
typename _List<T, Allocator>::iterator _List<T, Allocator>::insert(common_iterator<isConst> it, Args&&... args) {
    Node* node = AllocTraits::allocate(alloc, 1);
    BaseAllocTraits::construct(base_alloc, &node->value, std::forward<Args>(args)...);
    node->prev = node->next = node;
    node->prev = it.adress->prev;
    node->next = it.adress;
    it.adress->prev = node;
    node->prev->next = node;
    ++_size;
    return iterator(node);
}

template<typename T, typename Allocator>
template<bool isConst>
void _List<T, Allocator>::erase(common_iterator<isConst> it) {
    it.adress->next->prev = it.adress->prev;
    it.adress->prev->next = it.adress->next;
    AllocTraits::destroy(alloc, it.adress);
    AllocTraits::deallocate(alloc, it.adress, 1);
    --_size;
}

// *****
// _List::iterator

template<typename T, typename Allocator>
template<bool isConst>
_List<T, Allocator>::common_iterator<isConst>::common_iterator() {}

template<typename T, typename Allocator>
template<bool isConst>
_List<T, Allocator>::common_iterator<isConst>::common_iterator(Node* x): adress(x) {}

template<typename T, typename Allocator>
template<bool isConst>
_List<T, Allocator>::common_iterator<isConst>::common_iterator(const common_iterator<false>& it): adress(it.adress) {}

template<typename T, typename Allocator>
template<bool isConst>
bool _List<T, Allocator>::common_iterator<isConst>::operator==(_List<T, Allocator>::common_iterator<isConst> x) {
    return adress == x.adress;
}

template<typename T, typename Allocator>
template<bool isConst>
bool _List<T, Allocator>::common_iterator<isConst>::operator!=(_List<T, Allocator>::common_iterator<isConst> x) {
    return !operator==(x);
}

template<typename T, typename Allocator>
template<bool isConst>
std::conditional_t<isConst, const T&, T&> _List<T, Allocator>::common_iterator<isConst>::operator*() {
    return adress->value;
}

template<typename T, typename Allocator>
template<bool isConst>
std::conditional_t<isConst, const T*, T*> _List<T, Allocator>::common_iterator<isConst>::operator->() {
    return &(adress->value);
}

template<typename T, typename Allocator>
template<bool isConst>
_List<T, Allocator>::template common_iterator<isConst>& _List<T, Allocator>::common_iterator<isConst>::operator++() {
    adress = adress->next;
    return *this;
}

template<typename T, typename Allocator>
template<bool isConst>
_List<T, Allocator>::template common_iterator<isConst> _List<T, Allocator>::common_iterator<isConst>::operator++(int) {
    _List<T, Allocator>::common_iterator<isConst> ret = *this;
    adress = adress->next;
    return ret;
}

template<typename T, typename Allocator>
template<bool isConst>
_List<T, Allocator>::template common_iterator<isConst>& _List<T, Allocator>::common_iterator<isConst>::operator--() {
    adress = adress->prev;
    return *this;
}

template<typename T, typename Allocator>
template<bool isConst>
_List<T, Allocator>::template common_iterator<isConst> _List<T, Allocator>::common_iterator<isConst>::operator--(int) {
    _List<T, Allocator>::common_iterator<isConst> ret = *this;
    adress = adress->prev;
    return ret;
}

// *****
// _List::Node

template<typename T, typename Allocator>
_List<T, Allocator>::Node::Node(): next(this), prev(this) {}

template<typename T, typename Allocator>
_List<T, Allocator>::Node::Node(const T& value): value(value), next(this), prev(this) {}

template<typename T, typename Allocator>
_List<T, Allocator>::Node::Node(const _List<T, Allocator>::Node& another):
        value(another.value), next(this), prev(this) {}

template<typename T, typename Allocator>
template<typename... Args>
_List<T, Allocator>::Node::Node(Args&&... args):
        value(std::forward<Args>(args)...), next(this), prev(this) {}

// *****************
// UNORDERED_MAP
// *****************

// *****
// UNORDEREDMAP
// *****

template<typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
UnorderedMap<Key, Value, Hash, Equal, Alloc>::UnorderedMap(): alloc(BucketAlloc()),
        bucket_count(START_BUCKET_COUNT), _size(0),
        _max_load_factor(STANDART_MAX_LOAD_FACTOR), items(ListType()),
        pool(AllocTraits::allocate(alloc, bucket_count)) {
    for (size_t i = 0; i < bucket_count; ++i) {
        pool[i] = items.end();
    }
}

template<typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
UnorderedMap<Key, Value, Hash, Equal, Alloc>::UnorderedMap(const UnorderedMap& another):
        bucket_count(another.bucket_count), _size(0),
        _max_load_factor(another._max_load_factor), items(ListType()) {

    alloc = AllocTraits::select_on_container_copy_construction(another.alloc);
    pool = AllocTraits::allocate(alloc, bucket_count);

    for (size_t i = 0; i < bucket_count; ++i) {
        pool[i] = items.end();
    }

    for (auto it = another.items.begin(); it != another.items.end(); ++it) {
        insert(*it);
    }
}

template<typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
UnorderedMap<Key, Value, Hash, Equal, Alloc>::UnorderedMap(UnorderedMap&& another):
        alloc(another.alloc),
        bucket_count(another.bucket_count), _size(another._size),
        _max_load_factor(another._max_load_factor), items(std::move(another.items)),
        pool(another.pool) {

    another._max_load_factor = STANDART_MAX_LOAD_FACTOR;
    another.items = ListType();
    another.bucket_count = START_BUCKET_COUNT;
    another._size = 0;
    another.pool = AllocTraits::allocate(alloc, another.bucket_count);
    for (size_t i = 0; i < another.bucket_count; ++i) {
        another.pool[i] = another.items.end();
    }
}

template<typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
UnorderedMap<Key, Value, Hash, Equal, Alloc>&
        UnorderedMap<Key, Value, Hash, Equal, Alloc>::operator=(const UnorderedMap& another) {

    AllocTraits::deallocate(alloc, pool, bucket_count);

    if (AllocTraits::propagate_on_container_copy_assignment::value) {
        alloc = another.alloc;
    }

    bucket_count = another.bucket_count;
    _max_load_factor = another._max_load_factor;
    pool = AllocTraits::allocate(alloc, bucket_count);

    for (size_t i = 0; i < bucket_count; ++i) {
        pool[i] = items.end();
    }

    _size = 0;
    for (auto it = another.items.begin(); it != another.items.end(); ++it) {
        insert(*it);
    }
    return *this;
}

template<typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
UnorderedMap<Key, Value, Hash, Equal, Alloc>&
        UnorderedMap<Key, Value, Hash, Equal, Alloc>::operator=(UnorderedMap&& another) {

    AllocTraits::deallocate(alloc, pool, bucket_count);

    alloc = another.alloc;
    bucket_count = another.bucket_count;
    _size = another._size,
    _max_load_factor = another._max_load_factor;
    items = std::move(another.items);
    pool = another.pool;

    another._max_load_factor = STANDART_MAX_LOAD_FACTOR;
    another.items = ListType();
    another.bucket_count = START_BUCKET_COUNT;
    another._size = 0;
    another.pool = AllocTraits::allocate(alloc, another.bucket_count);
    for (size_t i = 0; i < another.bucket_count; ++i) {
        another.pool[i] = another.items.end();
    }

    return *this;
}

template<typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
UnorderedMap<Key, Value, Hash, Equal, Alloc>::~UnorderedMap() {
    AllocTraits::deallocate(alloc, pool, bucket_count);
}

// *****

template<typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
Value& UnorderedMap<Key, Value, Hash, Equal, Alloc>::operator[](const Key& key) {
    Iterator it = find(key);
    if (it == end()) {
        it = insert({key, Value()}).first;
    }
    return it->second;
}

template<typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
Value& UnorderedMap<Key, Value, Hash, Equal, Alloc>::at(const Key& key) {
    Iterator it = find(key);
    if (it == end()) {
        throw std::out_of_range("out of range");
    }
    return it->second;
}

template<typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
Value UnorderedMap<Key, Value, Hash, Equal, Alloc>::at(const Key& key) const {
    Iterator it = find(key);
    if (it == end()) {
        throw std::out_of_range("out of range");
    }
    return it->second;
}

template<typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
size_t UnorderedMap<Key, Value, Hash, Equal, Alloc>::size() const {
    return _size;
}

// *****

template<typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
typename UnorderedMap<Key, Value, Hash, Equal, Alloc>::Iterator
        UnorderedMap<Key, Value, Hash, Equal, Alloc>::begin() {
    return Iterator(items.begin());
}

template<typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
typename UnorderedMap<Key, Value, Hash, Equal, Alloc>::Iterator
        UnorderedMap<Key, Value, Hash, Equal, Alloc>::end() {
    return Iterator(items.end());
}

template<typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
typename UnorderedMap<Key, Value, Hash, Equal, Alloc>::ConstIterator
        UnorderedMap<Key, Value, Hash, Equal, Alloc>::cbegin() const {
    return ConstIterator(items.begin());
}

template<typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
typename UnorderedMap<Key, Value, Hash, Equal, Alloc>::ConstIterator
        UnorderedMap<Key, Value, Hash, Equal, Alloc>::cend() const {
    return ConstIterator(items.end());
}

// *****

template<typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
std::pair<typename UnorderedMap<Key, Value, Hash, Equal, Alloc>::Iterator, bool>
        UnorderedMap<Key, Value, Hash, Equal, Alloc>::insert(const NodeType& x) {
    return emplace(x);
}

template<typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
std::pair<typename UnorderedMap<Key, Value, Hash, Equal, Alloc>::Iterator, bool>
        UnorderedMap<Key, Value, Hash, Equal, Alloc>::insert(NodeType&& x) {
    return emplace(std::move(x));
}

template<typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
template<typename P>
std::pair<typename UnorderedMap<Key, Value, Hash, Equal, Alloc>::Iterator, bool>
        UnorderedMap<Key, Value, Hash, Equal, Alloc>::insert(P&& x) {
    return emplace(std::forward<P>(x));
}


template<typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
template<typename... Args>
std::pair<typename UnorderedMap<Key, Value, Hash, Equal, Alloc>::Iterator, bool>
        UnorderedMap<Key, Value, Hash, Equal, Alloc>::emplace(Args&&... args) {
    ListType elem;
    elem.insert(elem.begin(), std::forward<Args>(args)...);
    NodeType& x = *elem.begin();
    Iterator it = find(x.first);
    Iterator ret = end();
    bool inserted = false;
    if (it == end()) {
        if (1.0 * (_size + 1) / bucket_count > _max_load_factor) {
            rehash(2 * bucket_count);
        }
        size_t index = Hash{}(x.first) % bucket_count;
        typename ListType::iterator lit = pool[index];
        if (lit != items.end()) {
            items.insert(lit, elem.begin(), elem.end(), elem);
            pool[index] = --lit;
        } else {
            items.insert(items.end(), elem.begin(), elem.end(), elem);
            pool[index] = --items.end();
        }
        ++_size;
        inserted = true;
        ret = pool[index];
    }
    return {ret, inserted};
}


template<typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
template<typename InputIterator>
void UnorderedMap<Key, Value, Hash, Equal, Alloc>::
        insert(InputIterator begin, InputIterator end) {
    for (InputIterator it = begin; it != end; ++it) {
        insert(*it);
    }
}

template<typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
template<bool isConst>
typename UnorderedMap<Key, Value, Hash, Equal, Alloc>::Iterator
        UnorderedMap<Key, Value, Hash, Equal, Alloc>::erase(common_iterator<isConst> it) {
    size_t index = Hash{}(it->first) % bucket_count;
    if (Equal{}(pool[index]->first, it->first)) {
        ++pool[index];
        if (pool[index] != items.end() && (Hash{}(pool[index]->first) % bucket_count != index)) {
            pool[index] = items.end();
        }
    }
    Iterator ret = Iterator(it.list_node);
    ++ret;
    items.erase(it.list_node);
    --_size;
    return ret;
}

template<typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
template<bool isConst>
typename UnorderedMap<Key, Value, Hash, Equal, Alloc>::Iterator
        UnorderedMap<Key, Value, Hash, Equal, Alloc>::
        erase(common_iterator<isConst> begin, common_iterator<isConst> end) {
    for (ConstIterator it = begin; it != end; ) {
        ConstIterator copy = it;
        ++copy;
        erase(it);
        it = copy;
    }
    return end;
}

template<typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
typename UnorderedMap<Key, Value, Hash, Equal, Alloc>::Iterator
        UnorderedMap<Key, Value, Hash, Equal, Alloc>::find(const Key& key) {
    size_t index = Hash{}(key) % bucket_count;
    typename ListType::iterator it = pool[index];
    while (it != items.end() && !Equal{}(it->first, key) &&
            (Hash{}(it->first) % bucket_count) == index) {
        ++it;
    }
    if (it != items.end() && !Equal{}(it->first, key)) {
        it = items.end();
    }
    return Iterator(it);
}

// *****

template<typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
void UnorderedMap<Key, Value, Hash, Equal, Alloc>::reserve(size_t n) {
    rehash(std::ceil(n / max_load_factor()));
}

template<typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
size_t UnorderedMap<Key, Value, Hash, Equal, Alloc>::max_size() const {
    return MAX_SIZE;
}

template<typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
float UnorderedMap<Key, Value, Hash, Equal, Alloc>::load_factor() const {
    return 1.0  * _size / bucket_count;
}

template<typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
float UnorderedMap<Key, Value, Hash, Equal, Alloc>::max_load_factor() const {
    return _max_load_factor;
}

template<typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
void UnorderedMap<Key, Value, Hash, Equal, Alloc>::max_load_factor(float x) {
    _max_load_factor = x;
    if (load_factor() > _max_load_factor) {
        size_t k = 2;
        while (1.0 * _size / (bucket_count * k) > _max_load_factor) k *= 2;
        rehash(bucket_count * k);
    }
}

// *****
// UnorderedMap::Iterator

template<typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
template<bool isConst>
UnorderedMap<Key, Value, Hash, Equal, Alloc>::common_iterator<isConst>::common_iterator() {}

template<typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
template<bool isConst>
UnorderedMap<Key, Value, Hash, Equal, Alloc>::common_iterator<isConst>::
        common_iterator(typename ListType::iterator it): list_node(it) {}

template<typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
template<bool isConst>
UnorderedMap<Key, Value, Hash, Equal, Alloc>::common_iterator<isConst>::
        common_iterator(const common_iterator<false>& it): list_node(it.list_node) {}

template<typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
template<bool isConst>
bool UnorderedMap<Key, Value, Hash, Equal, Alloc>::common_iterator<isConst>::
        operator==(UnorderedMap<Key, Value, Hash, Equal, Alloc>::common_iterator<isConst> x) {
    return list_node == x.list_node;
}

template<typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
template<bool isConst>
bool UnorderedMap<Key, Value, Hash, Equal, Alloc>::common_iterator<isConst>::
        operator!=(UnorderedMap<Key, Value, Hash, Equal, Alloc>::common_iterator<isConst> x) {
    return !operator==(x);
}

template<typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
template<bool isConst>
typename UnorderedMap<Key, Value, Hash, Equal, Alloc>::template common_iterator<isConst>::reference
        UnorderedMap<Key, Value, Hash, Equal, Alloc>::common_iterator<isConst>::operator*() {
    return *list_node;
}

template<typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
template<bool isConst>
typename UnorderedMap<Key, Value, Hash, Equal, Alloc>::template common_iterator<isConst>::pointer
        UnorderedMap<Key, Value, Hash, Equal, Alloc>::common_iterator<isConst>::operator->() {
    return list_node.operator->();
}

template<typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
template<bool isConst>
UnorderedMap<Key, Value, Hash, Equal, Alloc>::template common_iterator<isConst>&
        UnorderedMap<Key, Value, Hash, Equal, Alloc>::common_iterator<isConst>::operator++() {
    ++list_node;
    return *this;
}

template<typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
template<bool isConst>
UnorderedMap<Key, Value, Hash, Equal, Alloc>::template common_iterator<isConst>
        UnorderedMap<Key, Value, Hash, Equal, Alloc>::common_iterator<isConst>::operator++(int) {
    UnorderedMap<Key, Value, Hash, Equal, Alloc>::common_iterator<isConst> ret = *this;
    ++list_node;
    return ret;
}

template<typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
void UnorderedMap<Key, Value, Hash, Equal, Alloc>::rehash(size_t min_bucket_count) {
    for ( ; ; ++min_bucket_count) {
        bool prime = true;
        for (size_t div = 2; div * div <= min_bucket_count; ++div) {
            if (min_bucket_count % div == 0) {
                prime = false;
                break;
            }
        }
        if (prime) break;
    }
    if (min_bucket_count < bucket_count) return;

    AllocTraits::deallocate(alloc, pool, bucket_count);
    bucket_count = min_bucket_count;
    pool = AllocTraits::allocate(alloc, bucket_count);
    ListType old_items = std::move(items);
    items = ListType();
    for (size_t i = 0; i < bucket_count; ++i) {
        pool[i] = items.end();
    }
    for (typename ListType::iterator iter = old_items.begin(); iter != old_items.end(); ) {
        typename ListType::iterator iter_copy = iter;
        typename ListType::iterator iter_copy_plus_one = iter_copy;
        ++iter_copy_plus_one;
        ++iter;
        NodeType& x = *iter_copy;
        size_t index = Hash{}(x.first) % bucket_count;
        typename ListType::iterator lit = pool[index];
        if (lit != items.end()) {
            items.insert(lit, iter_copy, iter_copy_plus_one, old_items);
            pool[index] = --lit;
        } else {
            items.insert(items.end(), iter_copy, iter_copy_plus_one, old_items);
            pool[index] = --items.end();
        }
    }
}
