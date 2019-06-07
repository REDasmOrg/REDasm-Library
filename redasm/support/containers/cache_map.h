#pragma once

#include <iostream>
#include <fstream>
#include <unordered_map>
#include <unordered_set>
#include "../../types/base_types.h"

namespace REDasm {

template<typename Key, typename Value> class cache_map // Use STL's coding style for this type
{
    private:
        typedef cache_map<Key, Value> type;
        typedef std::unordered_map<Key, std::streamoff> offset_map;
        typedef typename offset_map::iterator offset_iterator;

    public:
        class iterator: public std::iterator<std::random_access_iterator_tag, Value> {
            public:
                explicit iterator(type& container, const offset_iterator& offit): m_container(container), m_offit(offit) { update(); }
                iterator& operator++() { m_offit++; update(); return *this; }
                iterator& operator--() { m_offit--; update(); return *this; }
                iterator operator++(int) { iterator copy = *this; m_offit++; update(); return copy; }
                iterator operator--(int) { iterator copy = *this; m_offit--; update(); return copy; }
                bool operator==(const iterator& rhs) const { return m_offit == rhs.m_offit; }
                bool operator!=(const iterator& rhs) const { return m_offit != rhs.m_offit; }
                iterator& operator=(const iterator& rhs) { m_offit = rhs.m_offit; update(); return *this; }
                Value operator *() { return m_container[key]; }

            private:
                void update() { if(m_offit != m_container.m_offsets.end()) key = m_offit->first; }

            private:
                type& m_container;
                offset_iterator m_offit;

            public:
                Key key;
        };

    public:
        cache_map();
        virtual ~cache_map();
        iterator begin() { return iterator(*this, m_offsets.begin()); }
        iterator end() { return iterator(*this, m_offsets.end()); }
        iterator find(const Key& key) { auto it = m_offsets.find(key); return (it != m_offsets.end() ? iterator(*this, it) : this->end()); }
        size_t size() const;
        void commit(const Key& key, const Value& value);
        void erase(const iterator& it);
        Value value(const Key& key);
        Value operator[](const Key& key);

    private:
        static std::string generateFilePath();

    private:
        static std::unordered_set<std::string> m_activenames;
        std::string m_filepath;
        offset_map m_offsets;
        std::fstream m_file;
};

} // namespace REDasm

#include "cache_map.hpp"
