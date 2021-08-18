#pragma once

#include "sortedcontainer.h"
#include <unordered_map>
#include <map>

typedef SortedContainer<rd_address, std::equal_to<rd_address>, std::less<rd_address>, true> SortedAddresses;

template<typename T, bool Sorted = false>
class AddressContainer
{
    public:
        typedef AddressContainer<T> ClassType;
        typedef std::conditional_t<Sorted, std::map<rd_address, T>, std::unordered_map<rd_address, T>> Values;
        typedef SortedAddresses::const_iterator const_iterator;
        typedef SortedAddresses::iterator iterator;

    public:
        bool empty() const { return m_addresses.empty(); }
        bool contains(rd_address address) const { return this->find(address) != nullptr; }
        size_t size() const { return m_addresses.size(); }
        size_t data(const rd_address** addresses) const { return m_addresses.data(addresses); }
        size_t indexOf(rd_address address) const { return m_addresses.indexOf(address); }
        void clear() { m_addresses.clear(); m_values.clear(); }
        void clearValues() { m_values.clear(); }
        void insert(rd_address address) { m_addresses.insert(address); }
        template<typename V> void bind(rd_address address, const V& value) { m_values.emplace(address, value); }
        void remove(rd_address address) { m_values.erase(address); m_addresses.remove(address); }
        rd_address operator[](size_t index) const { return (index < m_addresses.size()) ? m_addresses.at(index) : RD_NVAL; }
        rd_address front() const { return !m_addresses.empty() ? m_addresses.front() : RD_NVAL; }
        rd_address back() const { return !m_addresses.empty() ? m_addresses.back() : RD_NVAL; }

        template<typename V> void insert(rd_address address, const V& value) {
            m_addresses.insert(address);
            m_values[address] = value;
        }

        T* find(rd_address address) const {
            auto it = m_values.find(address);
            if(it == m_values.end()) return nullptr;

            if constexpr(std::is_pointer_v<T>) return const_cast<T*>(it->second);
            else return const_cast<T*>(std::addressof(it->second));
        }

        bool find(rd_address address, T* t) const {
            auto it = m_values.find(address);
            if(it == m_values.end()) return false;
            if(t) *t = it->second;
            return true;
        }

    public:
        inline const_iterator begin() const { return m_addresses.begin(); }
        inline const_iterator end() const { return m_addresses.end(); }
        inline iterator begin() { return m_addresses.begin(); }
        inline iterator end() { return m_addresses.end(); }

    protected:
        SortedAddresses m_addresses;
        Values m_values;
};
