/*
Copyright 2017-2019 Glen Joseph Fernandes
(glenjofe@gmail.com)

Distributed under the Boost Software License, Version 1.0.
(http://www.boost.org/LICENSE_1_0.txt)
*/
#ifndef BOOST_SMART_PTR_ALLOCATE_LOCAL_SHARED_ARRAY_HPP
#define BOOST_SMART_PTR_ALLOCATE_LOCAL_SHARED_ARRAY_HPP

#include <boost/smart_ptr/allocate_shared_array.hpp>
#include <boost/smart_ptr/local_shared_ptr.hpp>

namespace boost {
namespace detail {

class BOOST_SYMBOL_VISIBLE lsp_array_base
    : public local_counted_base {
public:
    void set(sp_counted_base* base) BOOST_SP_NOEXCEPT {
        count_ = shared_count(base);
    }

    virtual void local_cb_destroy() BOOST_SP_NOEXCEPT {
        shared_count().swap(count_);
    }

    virtual shared_count local_cb_get_shared_count() const BOOST_SP_NOEXCEPT {
        return count_;
    }

private:
    shared_count count_;
};

template<class A>
class lsp_array_state
    : public sp_array_state<A> {
public:
    template<class U>
    lsp_array_state(const U& other, std::size_t size) BOOST_SP_NOEXCEPT
        : sp_array_state<A>(other, size) { }

    lsp_array_base& base() BOOST_SP_NOEXCEPT {
        return base_;
    }

private:
    lsp_array_base base_;
};

template<class A, std::size_t N>
class lsp_size_array_state
    : public sp_size_array_state<A, N> {
public:
    template<class U>
    lsp_size_array_state(const U& other, std::size_t size) BOOST_SP_NOEXCEPT
        : sp_size_array_state<A, N>(other, size) { }

    lsp_array_base& base() BOOST_SP_NOEXCEPT {
        return base_;
    }

private:
    lsp_array_base base_;
};

} /* detail */

template<class T, class A>
inline typename enable_if_<is_unbounded_array<T>::value,
    local_shared_ptr<T> >::type
allocate_local_shared(const A& allocator, std::size_t count)
{
    typedef typename remove_extent<T>::type type;
    typedef typename detail::sp_array_scalar<T>::type scalar;
    typedef typename detail::sp_bind_allocator<A, scalar>::type other;
    typedef detail::lsp_array_state<other> state;
    typedef detail::sp_array_base<state> base;
    std::size_t size = count * detail::sp_array_count<type, scalar>::value;
    detail::sp_array_result<other, base> result(allocator, size);
    base* node = result.get();
    scalar* start = detail::sp_array_start<base, scalar>(node);
    ::new(static_cast<void*>(node)) base(allocator, size, start);
    detail::lsp_array_base& local = node->state().base();
    local.set(node);
    result.release();
    return local_shared_ptr<T>(detail::lsp_internal_constructor_tag(),
        reinterpret_cast<type*>(start), &local);
}

template<class T, class A>
inline typename enable_if_<is_bounded_array<T>::value,
    local_shared_ptr<T> >::type
allocate_local_shared(const A& allocator)
{
    typedef typename remove_extent<T>::type type;
    typedef typename detail::sp_array_scalar<T>::type scalar;
    typedef typename detail::sp_bind_allocator<A, scalar>::type other;
    enum {
        size = detail::sp_array_count<T, scalar>::value
    };
    typedef detail::lsp_size_array_state<other, size> state;
    typedef detail::sp_array_base<state> base;
    detail::sp_array_result<other, base> result(allocator, size);
    base* node = result.get();
    scalar* start = detail::sp_array_start<base, scalar>(node);
    ::new(static_cast<void*>(node)) base(allocator, size, start);
    detail::lsp_array_base& local = node->state().base();
    local.set(node);
    result.release();
    return local_shared_ptr<T>(detail::lsp_internal_constructor_tag(),
        reinterpret_cast<type*>(start), &local);
}

template<class T, class A>
inline typename enable_if_<is_unbounded_array<T>::value,
    local_shared_ptr<T> >::type
allocate_local_shared(const A& allocator, std::size_t count,
    const typename remove_extent<T>::type& value)
{
    typedef typename remove_extent<T>::type type;
    typedef typename detail::sp_array_scalar<T>::type scalar;
    typedef typename detail::sp_bind_allocator<A, scalar>::type other;
    typedef detail::lsp_array_state<other> state;
    typedef detail::sp_array_base<state> base;
    enum {
        total = detail::sp_array_count<type, scalar>::value
    };
    std::size_t size = count * total;
    detail::sp_array_result<other, base> result(allocator, size);
    base* node = result.get();
    scalar* start = detail::sp_array_start<base, scalar>(node);
    ::new(static_cast<void*>(node)) base(allocator, size,
        reinterpret_cast<const scalar*>(&value), total, start);
    detail::lsp_array_base& local = node->state().base();
    local.set(node);
    result.release();
    return local_shared_ptr<T>(detail::lsp_internal_constructor_tag(),
        reinterpret_cast<type*>(start), &local);
}

template<class T, class A>
inline typename enable_if_<is_bounded_array<T>::value,
    local_shared_ptr<T> >::type
allocate_local_shared(const A& allocator,
    const typename remove_extent<T>::type& value)
{
    typedef typename remove_extent<T>::type type;
    typedef typename detail::sp_array_scalar<T>::type scalar;
    typedef typename detail::sp_bind_allocator<A, scalar>::type other;
    enum {
        size = detail::sp_array_count<T, scalar>::value
    };
    typedef detail::lsp_size_array_state<other, size> state;
    typedef detail::sp_array_base<state> base;
    detail::sp_array_result<other, base> result(allocator, size);
    base* node = result.get();
    scalar* start = detail::sp_array_start<base, scalar>(node);
    ::new(static_cast<void*>(node)) base(allocator, size,
        reinterpret_cast<const scalar*>(&value),
        detail::sp_array_count<type, scalar>::value, start);
    detail::lsp_array_base& local = node->state().base();
    local.set(node);
    result.release();
    return local_shared_ptr<T>(detail::lsp_internal_constructor_tag(),
        reinterpret_cast<type*>(start), &local);
}

template<class T, class A>
inline typename enable_if_<is_unbounded_array<T>::value,
    local_shared_ptr<T> >::type
allocate_local_shared_noinit(const A& allocator, std::size_t count)
{
    typedef typename remove_extent<T>::type type;
    typedef typename detail::sp_array_scalar<T>::type scalar;
    typedef typename detail::sp_bind_allocator<A, scalar>::type other;
    typedef detail::lsp_array_state<other> state;
    typedef detail::sp_array_base<state, false> base;
    std::size_t size = count * detail::sp_array_count<type, scalar>::value;
    detail::sp_array_result<other, base> result(allocator, size);
    base* node = result.get();
    scalar* start = detail::sp_array_start<base, scalar>(node);
    ::new(static_cast<void*>(node)) base(detail::sp_default(), allocator,
        size, start);
    detail::lsp_array_base& local = node->state().base();
    local.set(node);
    result.release();
    return local_shared_ptr<T>(detail::lsp_internal_constructor_tag(),
        reinterpret_cast<type*>(start), &local);
}

template<class T, class A>
inline typename enable_if_<is_bounded_array<T>::value,
    local_shared_ptr<T> >::type
allocate_local_shared_noinit(const A& allocator)
{
    typedef typename remove_extent<T>::type type;
    typedef typename detail::sp_array_scalar<T>::type scalar;
    typedef typename detail::sp_bind_allocator<A, scalar>::type other;
    enum {
        size = detail::sp_array_count<T, scalar>::value
    };
    typedef detail::lsp_size_array_state<other, size> state;
    typedef detail::sp_array_base<state, false> base;
    detail::sp_array_result<other, base> result(allocator, size);
    base* node = result.get();
    scalar* start = detail::sp_array_start<base, scalar>(node);
    ::new(static_cast<void*>(node)) base(detail::sp_default(), allocator,
        size, start);
    detail::lsp_array_base& local = node->state().base();
    local.set(node);
    result.release();
    return local_shared_ptr<T>(detail::lsp_internal_constructor_tag(),
        reinterpret_cast<type*>(start), &local);
}

} /* boost */

#endif
