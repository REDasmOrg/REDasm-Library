#pragma once

#include "sortedcontainer.h"
#include <unordered_map>

typedef SortedContainer<rd_address, std::equal_to<rd_address>, std::less<rd_address>, true> SortedAddresses;

template<typename T>
class AddressContainer
{
    public:
        typedef std::unordered_map<rd_address, T> Values;
        typedef typename Values::const_iterator const_iterator;
        typedef typename Values::iterator iterator;

    public:
        bool empty() const { return m_addresses.empty(); }
        size_t size() const { return m_addresses.size(); }
        size_t data(const rd_address** addresses) const { return m_addresses.data(addresses); }
        size_t indexOf(rd_address address) const { return m_addresses.indexOf(address); }
        void clear() { m_addresses.clear(); m_values.clear(); }
        template<typename V> void insert(rd_address address, const V& value) { m_values.emplace(address, value); m_addresses.insert(address); }
        void remove(rd_address address) { m_values.erase(address); m_addresses.remove(address); }
        rd_address operator[](size_t index) const { return (index < m_addresses.size()) ? m_addresses.at(index) : RD_NVAL; }
        rd_address front() const { return !m_addresses.empty() ? m_addresses.front() : RD_NVAL; }
        rd_address back() const { return !m_addresses.empty() ? m_addresses.back() : RD_NVAL; }

        bool find(rd_address address, T* t) const {
            auto it = m_values.find(address);
            if(it == m_values.end()) return false;
            if(t) *t = it->second;
            return true;
        }

    public:
        inline const_iterator begin() const { return m_values.begin(); }
        inline const_iterator end() const { return m_values.end(); }
        inline iterator begin() { return m_values.begin(); }
        inline iterator end() { return m_values.end(); }

    private:
        SortedAddresses m_addresses;
        Values m_values;
};
