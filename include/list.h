#ifndef PGSTL_LIST_H
#define PGSTL_LIST_H

#include "allocator.h"
#include "iterator.h"

namespace pgstl {

struct ListNodeBase {
    ListNodeBase *_next;
    ListNodeBase *_prev;

    static void swap(ListNodeBase &a, ListNodeBase &b) {
        const ListNodeBase temp(a);
        a = b;
        b = temp;

        if (a._next == &b)
            a._next = a._prev = &a;
        else
            a._next->_prev = a._prev->_next = &a;

        if (b._next == &a)
            b._next = b._prev = &b;
        else
            b._next->_prev = b._prev->_next = &b;
    }
};

template<class T>
struct ListNode : ListNodeBase {
    T _data;
};

template<class T>
struct ListIterator {
    using Self = ListIterator<T>;

    using value_type = T;
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using pointer = T *;
    using reference = T &;
    using iterator_category = bidirectional_iterator_tag;

    ListNodeBase *_node;

    ListIterator() : _node(nullptr) {}
    ListIterator(ListNodeBase *x) : _node(x) {}

    bool operator==(const Self &x) const { return _node == x._node; }
    bool operator!=(const Self &x) const { return _node != x._node; }

    reference operator*() const { return static_cast<ListNode<T> *>(_node)->_data; }
    pointer operator->() const { return &(operator*()); }

    Self &operator++() {
        _node = _node->_next;
        return *this;
    }

    Self operator++(int) {
        Self tmp = *this;
        _node = _node->_next;
        return tmp;
    }

    Self &operator--() {
        _node = _node->_prev;
        return *this;
    }
    Self operator--(int) {
        Self tmp = *this;
        _node = _node->_prev;
        return tmp;
    }
};

template<class T>
struct ListConstIterator {
    using Self = ListConstIterator<T>;
    using iterator = ListIterator<T>;

    using value_type = T;
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using pointer = const T *;
    using reference = const T &;
    using iterator_category = bidirectional_iterator_tag;

    const ListNodeBase *_node;

    ListConstIterator() : _node(nullptr) {}
    ListConstIterator(const ListNodeBase *x) : _node(x) {}
    ListConstIterator(const iterator &x) : _node(x._node) {}

    bool operator==(const Self &x) const { return _node == x._node; }
    bool operator!=(const Self &x) const { return _node != x._node; }

    reference operator*() const { return (static_cast<const ListNode<T> *>(_node))->_data; }
    pointer operator->() const { return &(operator*()); }

    Self &operator++() {
        _node = _node->_next;
        return *this;
    }

    Self operator++(int) {
        Self tmp = *this;
        _node = _node->_next;
        return tmp;
    }

    Self &operator--() {
        _node = _node->_prev;
        return *this;
    }
    Self operator--(int) {
        Self tmp = *this;
        _node = _node->_prev;
        return tmp;
    }
};

template<class T, class Allocator = allocator<T>>
class list {
public:
    using value_type = T;
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using iterator = ListIterator<T>;
    using const_iterator = ListConstIterator<T>;
    using reverse_iterator = pgstl::reverse_iterator<iterator>;
    using const_reverse_iterator = pgstl::reverse_iterator<const_iterator>;
    using reference = T &;
    using const_reference = const T &;
    using pointer = T *;
    using const_pointer = const T *;

    using NodeAllocator = typename Allocator::template rebind<ListNode<T>>::other;
    using allocator_type = Allocator;

protected:
    ListNodeBase *createNode() { return nodeAllocator.allocate(1); }
    void deleteNode(ListNodeBase *p) {
        nodeAllocator.deallocate(static_cast<ListNode<T> *>(p), 1);
    }

    ListNodeBase *constructNode(const T &x) {
        ListNodeBase *p = createNode();
        allocator.construct(&(static_cast<ListNode<T> *>(p)->_data), x);
        return p;
    }
    void destroyNode(ListNodeBase *p) {
        allocator.destroy(&(static_cast<ListNode<T> *>(p)->_data));
        deleteNode(p);
    }

    void initList() {
        _node = createNode();
        _node->_next = _node;
        _node->_prev = _node;
    }

    void transfer(iterator position, iterator first, iterator last) {
        if (position != last) {
            last._node->_prev->_next = position._node;
            first._node->_prev->_next = last._node;
            position._node->_prev->_next = first._node;

            ListNodeBase *tmp = position._node->_prev;
            position._node->_prev = last._node->_prev;
            last._node->_prev = first._node->_prev;
            first._node->_prev = tmp;
        }
    }

public:
    explicit list(const allocator_type &alloc = allocator_type()) :
            _node(nullptr), allocator(alloc) {
        initList();
    }
    explicit list(size_type n,
                  const value_type &val = value_type(),
                  const allocator_type &alloc = allocator_type()) :
            _node(nullptr),
            allocator(alloc) {
        initList();
        assign(n, val);
    }
    list(const T *first, const T *last,
         const allocator_type &alloc = allocator_type()) :
            _node(nullptr),
            allocator(alloc) {
        initList();
        assign(first, last);
    }
    list(const_iterator first, const_iterator last,
         const allocator_type &alloc = allocator_type()) :
            _node(nullptr), allocator(alloc) {
        initList();
        assign(first, last);
    }
    list(const list &x) : _node(x._node), allocator(x.allocator) {}

    ~list() {
        clear();
        deleteNode(_node);
    }

    list &operator=(const list &x) {
        if (this != &x) {
            clear();
            for (const_iterator iter = x.begin(); iter != x.end(); ++iter) {
                push_back(*iter);
            }
        }
        return *this;
    }

    iterator begin() { return _node->_next; }
    iterator end() { return _node; }
    const_iterator begin() const { return _node->_next; }
    const_iterator end() const { return _node; }

    reverse_iterator rbegin() { return reverse_iterator(end()); }
    const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }

    reverse_iterator rend() { return reverse_iterator(begin()); }
    const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }

    bool empty() const { return _node->_next == _node; }
    size_type size() const {
        return distance(begin(), end());
    }
    size_type max_size() const {
        nodeAllocator.max_size();
    }

    reference front() { return *begin(); }
    const_reference front() const { return *begin(); }
    reference back() {
        iterator tmp = end();
        --tmp;
        return *tmp;
    }
    const_reference back() const {
        iterator tmp = end();
        --tmp;
        return *tmp;
    }

    void assign(iterator first, iterator last) {
        clear();
        for (; first != last; ++first)
            push_back(*first);
    }
    void assign(const T *first, const T *last) {
        clear();
        for (; first != last; ++first)
            push_back(*first);
    }
    void assign(size_type n, const value_type &val) {
        clear();
        for (; n; --n)
            push_back(val);
    }

    iterator insert(iterator position, const T &x) {
        ListNodeBase *tmp = constructNode(x);
        tmp->_next = position._node;
        tmp->_prev = position._node->_prev;

        position._node->_prev->_next = tmp;
        position._node->_prev = tmp;
        return tmp;
    }
    void insert(iterator position, size_type n, const value_type &val) {
        list tmp(n, val, allocator);
        splice(position, tmp);
    }
    template<class InputIterator>
    void insert(iterator position, InputIterator first, InputIterator last) {
        list tmp(first, last, allocator);
        splice(position, tmp);
    }
    void push_front(const T &x) { insert(begin(), x); }
    void push_back(const T &x) { insert(end(), x); }

    iterator erase(iterator position) {
        ListNodeBase *next_node = position._node->_next;
        ListNodeBase *prev_node = position._node->_prev;
        prev_node->_next = next_node;
        next_node->_prev = prev_node;
        destroyNode(position._node);
        return iterator(next_node);
    }
    iterator erase(iterator first, iterator last) {
        while (first != last)
            first = erase(first);
        return last;
    }
    void pop_front() { erase(begin()); }
    void pop_back() {
        iterator tmp = end();
        erase(--tmp);
    }

    void resize(size_type n, value_type val = value_type()) {
        iterator cur = begin();
        while (cur != end() && n != 0) {
            ++cur;
            --n;
        }

        if (n != 0)
            insert(end(), n, val);
        else
            erase(cur, end());
    }

    void clear() {
        ListNodeBase *cur = _node->_next;

        while (cur != _node) {
            ListNodeBase *tmp = cur;
            cur = cur->_next;
            destroyNode(tmp);
        }

        _node->_next = _node;
        _node->_prev = _node;
    }

    void remove(const T &value) {
        iterator first = begin();
        iterator last = end();

        while (first != last) {
            iterator next = first;
            ++next;
            if (*first == value)
                erase(first);
            first = next;
        }
    }

    void unique() {
        if (empty()) return;

        iterator first = begin();
        iterator last = end();
        iterator next = first;

        while (++next != last) {
            if (*first == *next)
                erase(next);
            else
                first = next;
            next = first;
        }
    }

    void splice(iterator position, list &x) {
        if (!x.empty())
            transfer(position, x.begin(), x.end());
    }

    void splice(iterator position, list &x, iterator i) {
        iterator j = i;
        ++j;
        if (position == i || position == j)
            return;
        transfer(position, i, j);
    }

    void splice(iterator position, list &x, iterator first, iterator last) {
        if (first != last)
            transfer(position, first, last);
    }

    void merge(list &x) {
        iterator first1 = begin();
        iterator last1 = end();
        iterator first2 = x.begin();
        iterator last2 = x.end();

        while (first1 != last1 && first2 != last2) {
            if (*first2 < *first1) {
                iterator next = first2;
                transfer(first1, first2, ++next);
                first2 = next;
            } else {
                ++first1;
            }
        }
        if (first2 != last2)
            transfer(last1, first2, last2);
    }

    void reverse() {
        if (empty() || _node->_next->_next == _node)
            return;
        iterator first = begin();
        ++first;
        while (first != end()) {
            iterator old = first;
            ++first;
            transfer(begin(), old, first);
        }
    }

    void sort() {
        if (empty() || _node->_next->_next == _node)
            return;

        list carry;
        list counter[64];

        int fill = 0;

        while (!empty()) {
            carry.splice(carry.begin(), *this, begin());
            int i = 0;
            while (i < fill && !counter[i].empty()) {
                counter[i].merge(carry);
                carry.swap(counter[i++]);
            }
            carry.swap(counter[i]);
            if (i == fill)
                ++fill;
        }

        for (int i = 1; i < fill; ++i)
            counter[i].merge(counter[i - 1]);
        swap(counter[fill - 1]);
    }

    void swap(list &x) {
        if (allocator == x.allocator) {
            ListNodeBase::swap(*_node, *x._node);
        } else {
            const list temp(*this);
            *this = x;
            x = temp;
        }
    }

    allocator_type get_allocator() const {
        return allocator;
    }

protected:
    ListNodeBase *_node;
    NodeAllocator nodeAllocator;
    allocator_type allocator;
};

template<class T, class Alloc>
bool operator==(const list<T, Alloc> &lhs, const list<T, Alloc> &rhs) {
    auto end1 = lhs.end();
    auto end2 = rhs.end();

    auto i1 = lhs.begin();
    auto i2 = rhs.begin();
    while (i1 != end1 && i2 != end2 && *i1 == *i2) {
        ++i1;
        ++i2;
    }
    return i1 == end1 && i2 == end2;
}

template<class T, class Alloc>
bool operator!=(const list<T, Alloc> &lhs, const list<T, Alloc> &rhs) {
    return !(lhs == rhs);
}

template<class T, class Alloc>
bool operator<(const list<T, Alloc> &lhs, const list<T, Alloc> &rhs) {
    return lexicographical_compare(
            lhs.begin(), lhs.end(),
            rhs.begin(), rhs.end());
}

template<class T, class Alloc>
bool operator<=(const list<T, Alloc> &lhs, const list<T, Alloc> &rhs) {
    return !(lhs > rhs);
}

template<class T, class Alloc>
bool operator>(const list<T, Alloc> &lhs, const list<T, Alloc> &rhs) {
    return rhs < lhs;
}

template<class T, class Alloc>
bool operator>=(const list<T, Alloc> &lhs, const list<T, Alloc> &rhs) {
    return !(lhs < rhs);
}

template<class T, class Alloc>
void swap(list<T, Alloc> &x, list<T, Alloc> &y) {
    x.swap(y);
}
}

#endif //PGSTL_LIST_H
