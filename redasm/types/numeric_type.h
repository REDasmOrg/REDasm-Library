#ifndef NUMERIC_TYPE_H
#define NUMERIC_TYPE_H

#include <type_traits>
#include <cstddef>

namespace REDasm {

template<typename T> class numeric_type
{
    public:
        numeric_type() = default;
        numeric_type(const T& val): m_val(val) { }
        numeric_type(const numeric_type<T>&) = default;

    public:
        T* operator &() { return &m_val; }
        constexpr bool operator !() const { return !m_val; }
        template<typename U> constexpr bool operator ==(U rhs) const { return m_val == rhs; }
        template<typename U> constexpr bool operator !=(U rhs) const { return m_val != rhs; }
        template<typename U> constexpr bool operator <=(U rhs) const { return m_val <= rhs; }
        template<typename U> constexpr bool operator >=(U rhs) const { return m_val >= rhs; }
        template<typename U> constexpr bool operator <(U rhs) const { return m_val < rhs; }
        template<typename U> constexpr bool operator >(U rhs) const { return m_val > rhs; }
        constexpr numeric_type<T> operator ~() const { return numeric_type<T>(~m_val); }
        numeric_type<T>& operator ++() { m_val++; return *this; }
        numeric_type<T> operator ++(int) { numeric_type<T> copy(m_val); m_val++; return copy; }
        numeric_type<T>& operator --() { m_val--; return *this; }
        numeric_type<T> operator --(int) { numeric_type<T> copy(m_val); m_val--; return copy; }
        template<typename U> constexpr numeric_type<T> operator +(U rhs) const { return numeric_type<T>(m_val + rhs); }
        template<typename U> constexpr numeric_type<T> operator -(U rhs) const { return numeric_type<T>(m_val - rhs); }
        template<typename U> constexpr numeric_type<T> operator *(U rhs) const { return numeric_type<T>(m_val * rhs); }
        template<typename U> constexpr numeric_type<T> operator /(U rhs) const { return numeric_type<T>(m_val / rhs); }
        template<typename U> constexpr numeric_type<T> operator %(U rhs) const { return numeric_type<T>(m_val % rhs); }
        template<typename U> constexpr numeric_type<T> operator &(U rhs) const { return numeric_type<T>(m_val & rhs); }
        template<typename U> constexpr numeric_type<T> operator |(U rhs) const { return numeric_type<T>(m_val | rhs); }
        template<typename U> constexpr numeric_type<T> operator ^(U rhs) const { return numeric_type<T>(m_val ^ rhs); }
        template<typename U> constexpr numeric_type<T> operator >>(U rhs) const { return numeric_type<T>(m_val >> rhs); }
        template<typename U> constexpr numeric_type<T> operator <<(U rhs) const { return numeric_type<T>(m_val << rhs); }
        template<typename U> numeric_type<T>& operator +=(U rhs) { m_val += rhs; return *this; }
        template<typename U> numeric_type<T>& operator -=(U rhs) { m_val -= rhs; return *this; }
        template<typename U> numeric_type<T>& operator *=(U rhs) { m_val *= rhs; return *this; }
        template<typename U> numeric_type<T>& operator /=(U rhs) { m_val /= rhs; return *this; }
        template<typename U> numeric_type<T>& operator &=(U rhs) { m_val &= rhs; return *this; }
        template<typename U> numeric_type<T>& operator |=(U rhs) { m_val |= rhs; return *this; }
        template<typename U> numeric_type<T>& operator ^=(U rhs) { m_val ^= rhs; return *this; }
        template<typename U> numeric_type<T>& operator >>=(U rhs) { m_val >>= rhs; return *this; }
        template<typename U> numeric_type<T>& operator <<=(U rhs) { m_val <<= rhs; return *this; }
        template<typename U> constexpr operator U() const { return static_cast<U>(m_val); }

    protected:
        T m_val;
};

static_assert(std::is_trivial< numeric_type<size_t> >::value, "numeric_type<T> must be trivial");

} // namespace REDasm

#endif // NUMERIC_TYPE_H
