#ifndef NUMERIC_TYPE_H
#define NUMERIC_TYPE_H

#include <type_traits>
#include <cstddef>
#include "endianness/endianness_base.h"

namespace REDasm {

template<typename T, endianness_t endianness> struct numeric_type
{
    typedef numeric_type<T, Endianness::current> platform_numeric_type;

    numeric_type() = default;
    numeric_type(const T& val): m_val(val) { }
    numeric_type(const numeric_type<T, endianness>&) = default;
    T* operator &() { return &m_val; }
    constexpr bool operator !() const { return !m_val; }
    template<typename U> constexpr bool operator ==(U rhs) const { return converted() == rhs; }
    template<typename U> constexpr bool operator !=(U rhs) const { return converted() != rhs; }
    template<typename U> constexpr bool operator <=(U rhs) const { return converted() <= rhs; }
    template<typename U> constexpr bool operator >=(U rhs) const { return converted() >= rhs; }
    template<typename U> constexpr bool operator <(U rhs) const { return converted() < rhs; }
    template<typename U> constexpr bool operator >(U rhs) const { return converted() > rhs; }
    constexpr numeric_type<T, endianness> operator ~() const { return numeric_type<T, endianness>(~m_val); }
    numeric_type<T, endianness>& operator ++() { m_val++; return *this; }
    numeric_type<T, endianness> operator ++(int) { numeric_type<T, endianness> copy(m_val); m_val++; return copy; }
    numeric_type<T, endianness>& operator --() { m_val--; return *this; }
    numeric_type<T, endianness> operator --(int) { numeric_type<T, endianness> copy(m_val); m_val--; return copy; }
    template<typename U> constexpr platform_numeric_type operator +(U rhs) const { return converted() + rhs; }
    template<typename U> constexpr platform_numeric_type operator -(U rhs) const { return converted() - rhs; }
    template<typename U> constexpr platform_numeric_type operator *(U rhs) const { return converted() * rhs; }
    template<typename U> constexpr platform_numeric_type operator /(U rhs) const { return converted() / rhs; }
    template<typename U> constexpr platform_numeric_type operator %(U rhs) const { return converted() % rhs; }
    template<typename U> constexpr platform_numeric_type operator &(U rhs) const { return converted() & rhs; }
    template<typename U> constexpr platform_numeric_type operator |(U rhs) const { return converted() | rhs; }
    template<typename U> constexpr platform_numeric_type operator ^(U rhs) const { return converted() ^ rhs; }
    template<typename U> constexpr platform_numeric_type operator >>(U rhs) const { return converted() >> rhs; }
    template<typename U> constexpr platform_numeric_type operator <<(U rhs) const { return converted() << rhs; }
    template<typename U> numeric_type<T, endianness>& operator +=(U rhs) { m_val += rhs; return *this; }
    template<typename U> numeric_type<T, endianness>& operator -=(U rhs) { m_val -= rhs; return *this; }
    template<typename U> numeric_type<T, endianness>& operator *=(U rhs) { m_val *= rhs; return *this; }
    template<typename U> numeric_type<T, endianness>& operator /=(U rhs) { m_val /= rhs; return *this; }
    template<typename U> numeric_type<T, endianness>& operator &=(U rhs) { m_val &= rhs; return *this; }
    template<typename U> numeric_type<T, endianness>& operator |=(U rhs) { m_val |= rhs; return *this; }
    template<typename U> numeric_type<T, endianness>& operator ^=(U rhs) { m_val ^= rhs; return *this; }
    template<typename U> numeric_type<T, endianness>& operator >>=(U rhs) { m_val >>= rhs; return *this; }
    template<typename U> numeric_type<T, endianness>& operator <<=(U rhs) { m_val <<= rhs; return *this; }
    constexpr operator T() const { return converted(); }
    constexpr T value() const { return m_val; }
    constexpr T converted() const { return (Endianness::current != endianness) ? Endianness::swap<T>(m_val) : m_val; }

    protected:
        T m_val;
};

static_assert(std::is_trivial< numeric_type<size_t, Endianness::LittleEndian> >::value, "numeric_type<T> must be trivial");

} // namespace REDasm

#endif // NUMERIC_TYPE_H
