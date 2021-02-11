#pragma once

/*
 * 1 - https://www.codeproject.com/Articles/1183379/We-make-any-object-thread-safe
 * 2 - https://www.codeproject.com/Articles/1183423/We-make-a-std-shared-mutex-times-faster
 * 3 - https://www.codeproject.com/Articles/1183446/Thread-safe-std-map-with-the-speed-of-lock-free-ma
 */

#include <memory>
#include <mutex>

template<typename T, typename mutex_t = std::recursive_mutex, typename s_lock_t = std::scoped_lock<mutex_t>, typename x_lock_t = std::scoped_lock<mutex_t>>
class safe_ptr
{
    public:
        typedef mutex_t mutex_type;
        typedef s_lock_t s_lock_type;
        typedef x_lock_t x_lock_type;
        typedef T object_type;

    private:
        template<typename lock_t> struct auto_lock {
            T* t;
            lock_t m;

            auto_lock(T* t, mutex_t& m): t(t), m(m) { }
            auto_lock(auto_lock&& al): t(std::move(al.t)), m(std::move(al.m)) { }
            ~auto_lock() { }
            const T* operator->() const { return t; }
            T* operator->() { return t; }
        };

        template<typename lock_t> struct auto_lock_obj {
            T* t;
            lock_t m;

            auto_lock_obj(T* t, mutex_t& m): t(t), m(m) { }
            auto_lock_obj(auto_lock_obj&& alo): t(std::move(alo.t)), m(std::move(alo.m)) { }
            ~auto_lock_obj() { }
            template<typename ARG> auto operator[](ARG arg) -> decltype((*t)[arg]) { return (*t)[arg]; }
        };

        struct nolock_t {
            nolock_t(nolock_t&&) { }
            template<typename Unused> nolock_t(Unused&) { }
        };

        using auto_nolock_obj = auto_lock_obj<nolock_t>;

    public:
        safe_ptr() = default;
        template< typename deleter_t = std::default_delete<T> > safe_ptr(T* ptr, const deleter_t& d = deleter_t()) { m_ptr = std::shared_ptr<T>(ptr, d); m_mutex = std::make_shared<mutex_t>(); }
        mutex_t* mget() const { return m_mutex.get(); }
        T* get() const { return m_ptr.get(); }
        const auto_lock_obj<s_lock_t> operator*() const { return auto_lock_obj<s_lock_t>(m_ptr.get(), *m_mutex.get()); }
        const auto_lock<s_lock_t> operator->() const { return auto_lock<s_lock_t>(m_ptr.get(), *m_mutex.get()); }
        auto_lock_obj<x_lock_t> operator*() { return auto_lock_obj<x_lock_t>(m_ptr.get(), *m_mutex.get()); }
        auto_lock<x_lock_t> operator->() { return auto_lock<x_lock_t>(m_ptr.get(), *m_mutex.get()); }

    private:
        std::shared_ptr<T> m_ptr;
        std::shared_ptr<mutex_t> m_mutex;

    template<typename Unknown> friend struct x_locked_safe_ptr;
    template<typename Unknown> friend struct s_locked_safe_ptr;
};

template<typename T> struct x_locked_safe_ptr {
    T& t;
    typename T::x_lock_type xlock;

    x_locked_safe_ptr(T& t): t(t), xlock(*t.mget()) { }
    typename T::object_type* operator->() { return t.get(); }
    typename T::auto_nolock_obj operator*() { return T::auto_nolock_obj(t.get(), *t.mget()); }
};

template<typename T> struct s_locked_safe_ptr {
    T& t;
    typename T::s_lock_type slock;

    s_locked_safe_ptr(T& t): t(t), slock(*t.mget()) { }
    typename T::object_type* operator->() const { return t.get(); }
    const typename T::auto_nolock_obj operator*() const { return T::auto_nolock_obj(t.get(), *t.mget()); }
};

template<typename T> x_locked_safe_ptr<T> x_lock_safe_ptr(T& t) { return x_locked_safe_ptr<T>(t); }
template<typename T> s_locked_safe_ptr<T> s_lock_safe_ptr(T& t) { return s_locked_safe_ptr<T>(t); }
