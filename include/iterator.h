#ifndef PGSTL_ITERATOR_H
#define PGSTL_ITERATOR_H

namespace pgstl {

struct input_iterator_tag {
};

struct output_iterator_tag {
};

struct forward_iterator_tag :
        public input_iterator_tag {
};

struct bidirectional_iterator_tag : public forward_iterator_tag {
};

struct random_access_iterator_tag : public bidirectional_iterator_tag {
};

template<class Category,
        class T,
        class Distance = ptrdiff_t,
        class Pointer = T *,
        class Reference = T &>
struct iterator {
    using iterator_category = Category;
    using value_type = T;
    using difference_type = Distance;
    using pointer = Pointer;
    using reference = Reference;
};

template<class T>
struct iterator_traits {
    using iterator_category = typename T::iterator_category;
    using value_type = typename T::value_type;
    using difference_type = typename T::difference_type;
    using reference = typename T::reference;
    using pointer = typename T::pointer;
};

// 对该类型进行偏特化
template<typename T>
struct iterator_traits<T *> {
    typedef random_access_iterator_tag iterator_category;
    typedef T value_type;
    typedef ptrdiff_t difference_type;
    typedef T *pointer;
    typedef T &reference;
};

template<typename T>
struct iterator_traits<const T *> {
    typedef random_access_iterator_tag iterator_category;
    typedef T value_type;
    typedef ptrdiff_t difference_type;
    typedef const T *pointer;
    typedef const T &reference;
};

template<class Iterator>
class reverse_iterator : public iterator<
        typename iterator_traits<Iterator>::iterator_category,
        typename iterator_traits<Iterator>::value_type,
        typename iterator_traits<Iterator>::difference_type,
        typename iterator_traits<Iterator>::pointer,
        typename iterator_traits<Iterator>::reference
> {
protected:
    Iterator _cur;

    using TraitsType = iterator_traits<Iterator>;

public:
    using iterator_type = Iterator;
    using iterator_category = typename TraitsType::iterator_category;
    using value_type = typename TraitsType::value_type;
    using difference_type = typename TraitsType::difference_type;
    using pointer = typename TraitsType::pointer;
    using reference = typename TraitsType::reference;

    reverse_iterator() : _cur() {}
    explicit reverse_iterator(iterator_type it) : _cur(it) {}
    reverse_iterator(const reverse_iterator &rev_it) : _cur(rev_it._cur) {}

    iterator_type base() const { return _cur; }
    reference operator*() const {
        Iterator tmp = _cur;
        return *--tmp;
    }
    pointer operator->() const {
        return &(operator*());
    }

    reverse_iterator &operator++() {
        --_cur;
        return *this;
    }
    reverse_iterator operator++(int) {
        reverse_iterator tmp = *this;
        --_cur;
        return tmp;
    }

    reverse_iterator &operator--() {
        ++_cur;
        return *this;
    }
    reverse_iterator operator--(int) {
        reverse_iterator tmp = *this;
        ++_cur;
        return tmp;
    }

    reverse_iterator operator+(difference_type n) const {
        return reverse_iterator(_cur - n);
    }
    reverse_iterator operator-(difference_type n) const {
        return reverse_iterator(_cur + n);
    }
    reverse_iterator &operator+=(difference_type n) {
        _cur -= n;
        return *this;
    }
    reverse_iterator &operator-=(difference_type n) {
        _cur += n;
        return *this;
    }

    reference operator[](difference_type n) const {
        return *(*this + n);
    }

};

template<class Iterator>
bool operator==(const reverse_iterator<Iterator> &lhs,
                const reverse_iterator<Iterator> &rhs) {
    return lhs.base() == rhs.base();
}

template<class Iterator>
bool operator!=(const reverse_iterator<Iterator> &lhs,
                const reverse_iterator<Iterator> &rhs) {
    return lhs.base() != rhs.base();
}

template<class Iterator>
bool operator<(const reverse_iterator<Iterator> &lhs,
               const reverse_iterator<Iterator> &rhs) {
    return lhs.base() > rhs.base();
}

template<class Iterator>
bool operator<=(const reverse_iterator<Iterator> &lhs,
                const reverse_iterator<Iterator> &rhs) {
    return lhs.base() >= rhs.base();
}

template<class Iterator>
bool operator>(const reverse_iterator<Iterator> &lhs,
               const reverse_iterator<Iterator> &rhs) {
    return lhs.base() < rhs.base();
}

template<class Iterator>
bool operator>=(const reverse_iterator<Iterator> &lhs,
                const reverse_iterator<Iterator> &rhs) {
    return lhs.base() <= lhs.base();
}

template<class Iterator>
reverse_iterator<Iterator> operator+(
        typename reverse_iterator<Iterator>::difference_type n,
        const reverse_iterator<Iterator> &rev_it) {
    return reverse_iterator<Iterator>(rev_it.base() - n);
}

template<class Iterator>
typename reverse_iterator<Iterator>::difference_type operator-(
        const reverse_iterator<Iterator> &lhs,
        const reverse_iterator<Iterator> &rhs) {
    return rhs.base() - lhs.base();
}

template<typename InputIterator>
inline typename iterator_traits<InputIterator>::difference_type
distance(InputIterator first, InputIterator last) {
    return distance(first, last,
                    typename InputIterator::iterator_category());
}

template<typename InputIterator>
inline typename iterator_traits<InputIterator>::difference_type
distance(InputIterator first, InputIterator last,
         input_iterator_tag) {
    typename iterator_traits<InputIterator>::difference_type n = 0;
    while (first != last) {
        ++first;
        ++n;
    }
    return n;
}

template<typename RandomAccessIterator>
inline typename iterator_traits<RandomAccessIterator>::difference_type
distance(RandomAccessIterator first, RandomAccessIterator last,
         random_access_iterator_tag) {
    return last - first;
}

template<class InputIterator1, class InputIterator2>
inline bool lexicographical_compare(
        InputIterator1 first1, InputIterator1 last1,
        InputIterator2 first2, InputIterator2 last2) {
    for (; first1 != last1 && first2 != last2; ++first1, ++first2)
        if (*first1 < *first2)
            return (true);
        else if (*first2 < *first1)
            return (false);
    return (first1 == last1 && first2 != last2);
}

}

#endif //PGSTL_ITERATOR_H
