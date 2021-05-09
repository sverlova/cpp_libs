#include <iostream>
#include <vector>
#include <memory>

template<typename T, typename Allocator = std::allocator<T>>
class List {
  public:
    explicit List(const Allocator& _alloc = Allocator());
    List(size_t count);
    List(size_t count, const T& value, const Allocator& _alloc = Allocator());
    List(const List& another);
    List& operator=(const List& another);
    ~List();

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
    };

    using NodeAlloc = typename std::allocator_traits<Allocator>::template rebind_alloc<Node>;
    using AllocTraits = std::allocator_traits<NodeAlloc>;

    Node* _base;
    NodeAlloc _alloc;
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


        friend class List<T, Allocator>;
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
    template<bool isConst>
    void erase(common_iterator<isConst> it);
};
