#ifndef PGSTL_ALLOCATOR_H
#define PGSTL_ALLOCATOR_H

#include <cstddef>
#include <climits>

namespace pgstl {

template<class T>
class allocator {
public:
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using pointer = T *;
    using const_pointer = const T *;
    using value_type = T;
    using reference = T &;
    using const_reference = const T &;

    /**
     * 利用当前的分配器产生另外一个类型的分配器
     * @tparam U 另一个类型
     */
    template<class U>
    struct rebind {
        typedef allocator<U> other;
    };

public:
    allocator() noexcept = default;
    allocator(const allocator &a) noexcept = default;

    template<class U>
    explicit allocator(const allocator<U> &) noexcept {}

    ~allocator() noexcept = default;

    pointer address(reference x) { return &x; }
    const_pointer address(const_reference x) { return &x; }

    /**
     * 申请大小为 n 的内存
     * @param n : 申请空间的大小
     * @param hint : 在使用时传一个空指针（将其转换成对应类型）来指明申请什么类型的指针（函数内未使用）
     * @return 返回申请空间的首地址
     */
    T* allocate(size_type n, const void * = nullptr) {
        // TODO: 这个地方需要抛出bad_alloc异常
        if (n > max_size() || n < 0)
            n = 0;
        return static_cast<T *>(::operator new((size_t) (n * sizeof(T))));
    }

    /**
     * 收回分配的空间
     * @param p 分配空间的首地址
     * @param n 表明分配空间的大小，(在函数内并未使用)
     */
    void deallocate(pointer p, size_type) { ::operator delete(p); }

    size_type max_size() const noexcept { return size_type(-1) / sizeof(T); }

    void construct(pointer p, const T &value) {
        ::new(static_cast<void *>(p)) T(value);
    }

    void destroy(pointer p) { p->~T(); }
};

// All allocators are considered equal, as they merely use global new/delete.
template<typename T1, typename T2>
inline bool
operator==(const allocator<T1> &, const allocator<T2> &) { return true; }

template<typename T>
inline bool
operator==(const allocator<T> &, const allocator<T> &) { return true; }

// All allocators are considered equal, as they merely use global new/delete.
template<typename T1, typename T2>
inline bool
operator!=(const allocator<T1> &, const allocator<T2> &) { return false; }

template<typename T>
inline bool
operator!=(const allocator<T> &, const allocator<T> &) { return false; }

template<>
class allocator<void> {
public:
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using pointer = void *;
    using const_pointer = const void *;
    using value_type = void;

    template<class U>
    struct rebind {
        using other = allocator<U>;
    };
};

template<class T, class Allocator>
struct alloc_traits {
    using origin_allocator_type = Allocator;
    using rebind_type = typename Allocator::template rebind<T>;
    using allocator_type = typename rebind_type::other;

    static allocator_type create_allocator(const origin_allocator_type& a) {
        return allocator_type(a);
    }
};




}

#endif //PGSTL_ALLOCATOR_H
