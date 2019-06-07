#pragma once

// Sequence Containers
#include <vector>
#include <deque>
#include <list>

// Associative Containers
#include <unordered_map>
#include <map>

// Set Containers
#include <unordered_set>
#include <set>

// REDasm Containers
#include "containers/sorted_container.h"

#include <redasm/libs/visit_struct/visit_struct.hpp>
#include <algorithm>
#include <fstream>
#include <memory>
#include "../types/api_types.h"
#include "../buffer/abstractbuffer.h"

VISITABLE_STRUCT(REDasm::RegisterOperand, r, tag);
VISITABLE_STRUCT(REDasm::DisplacementOperand, base, index, scale, displacement);
VISITABLE_STRUCT(REDasm::Operand, type, tag, size, index, loc_index, reg, disp, u_value);
VISITABLE_STRUCT(REDasm::Instruction, mnemonic, operands, address, type, size, id);
VISITABLE_STRUCT(REDasm::Segment, name, offset, endoffset, address, endaddress, type);

namespace REDasm {

namespace Detail {

template<typename T> struct is_seq_container { static const bool value = false; };
template <typename... Args> struct is_seq_container< std::vector<Args...> > { static bool const value = true; };
template <typename... Args> struct is_seq_container< std::deque<Args...> > { static bool const value = true; };
template <typename... Args> struct is_seq_container< std::list<Args...> > { static bool const value = true; };

template<typename T> struct is_assoc_container { static const bool value = false; };
template<typename... Args> struct is_assoc_container< std::unordered_map<Args...> > { static const bool value = true; };
template<typename... Args> struct is_assoc_container< std::map<Args...> > { static const bool value = true; };

template<typename T> struct is_set_container { static const bool value = false; };
template <typename... Args> struct is_set_container< std::unordered_set<Args...> > { static bool const value = true; };
template <typename... Args> struct is_set_container< std::set<Args...> > { static bool const value = true; };
template <typename... Args> struct is_set_container< sorted_container<Args...> > { static bool const value = true; };

} // namespace Detail

template<typename T, typename = void> struct Serializer {
    //static void write(std::fstream& fs, const T& t);
    //static void read(std::fstream& fs, T& t);
    //static void read(std::fstream& fs, const std::function<void(Item)>& cb);
};

template<typename T> struct Serializer<T, typename std::enable_if<std::is_integral<T>::value || std::is_enum<T>::value>::type> {
    static void write(std::fstream& fs, const T& t) { fs.write(reinterpret_cast<const char*>(&t), sizeof(T));  }
    static void read(std::fstream& fs, T& t) {  fs.read(reinterpret_cast<char*>(&t), sizeof(T)); }
};

template<> struct Serializer<std::string> {
    static void write(std::fstream& fs, const std::string& s) { fs.write(s.c_str(), s.size() + 1); }
    static void read(std::fstream& fs, std::string& s) { std::getline(fs, s, '\0'); }
};

// template<typename T> struct Serializer<T, typename std::enable_if<std::is_enum<T>::value>::type> {
//     static void write(std::fstream& fs, const T& t) { Serializer<typename std::underlying_type<T>::type>::write(fs, static_cast<typename std::underlying_type<T>::type>(t)); }
//     static void read(std::fstream& fs, T& t) { Serializer<typename std::underlying_type<T>::type>::read(fs, static_cast<typename std::underlying_type<T>::type>(t)); }
// };

template<typename T> struct Serializer<T, typename std::enable_if<Detail::is_seq_container<T>::value>::type> {
    static void write(std::fstream& fs, const T& c) {
        Serializer<typename T::size_type>::write(fs, c.size());
        std::for_each(c.begin(), c.end(), [&](const typename T::value_type& v) { Serializer<typename T::value_type>::write(fs, v); });
    }

    static void read(std::fstream& fs, T& c) {
        typename T::size_type sz;
        Serializer<typename T::size_type>::read(fs, sz);

        for(typename T::size_type i = 0; i < sz; i++) {
            typename T::value_type v;
            Serializer<typename T::value_type>::read(fs, v);
            c.push_back(v);
        }
    }

    static void read(std::fstream& fs, const std::function<void(typename T::value_type)>& cb) {
        typename T::size_type sz;
        Serializer<typename T::size_type>::read(fs, sz);

        for(typename T::size_type i = 0; i < sz; i++) {
            typename T::value_type v;
            Serializer<typename T::value_type>::read(fs, v);
            cb(v);
        }
    }
};

template<typename T> struct Serializer<T, typename std::enable_if<Detail::is_assoc_container<T>::value>::type> {
    static void write(std::fstream& fs, const T& c) {
        Serializer<typename T::size_type>::write(fs, c.size());

        for(const typename T::value_type& item : c) {
            Serializer<typename T::key_type>::write(fs, item.first);
            Serializer<typename T::mapped_type>::write(fs, item.second);
        }
    }

    static void read(std::fstream& fs, T& c) {
        typename T::size_type sz;
        Serializer<typename T::size_type>::read(fs, sz);

        for(typename T::size_type i = 0; i < sz; i++) {
            typename T::key_type k;
            typename T::mapped_type v;

            Serializer<typename T::key_type>::read(fs, k);
            Serializer<typename T::mapped_type>::read(fs, v);
            c.emplace(k, std::move(v));
        }
    }

    static void read(std::fstream& fs, const std::function<void(typename T::key_type, typename T::mapped_type)>& cb) {
        typename T::size_type sz;
        Serializer<typename T::size_type>::read(fs, sz);

        for(typename T::size_type i = 0; i < sz; i++) {
            typename T::key_type k;
            typename T::mapped_type v;

            Serializer<typename T::key_type>::read(fs, k);
            Serializer<typename T::mapped_type>::read(fs, v);
            cb(k, std::move(v));
        }
    }
};

template<typename T> struct Serializer<T, typename std::enable_if<Detail::is_set_container<T>::value>::type> {
    static void write(std::fstream& fs, const T& s) {
        Serializer<typename T::size_type>::write(fs, s.size());
        std::for_each(s.begin(), s.end(), [&](const typename T::value_type& v) { Serializer<typename T::value_type>::write(fs, v); });
    }

    static void read(std::fstream& fs, T& s) {
        typename T::size_type sz;
        Serializer<typename T::size_type>::read(fs, sz);

        for(typename T::size_type i = 0; i < sz; i++) {
            typename T::value_type v;
            Serializer<typename T::value_type>::read(fs, v);
            s.insert(v);
        }
    }

    static void read(std::fstream& fs, const std::function<void(typename T::value_type)>& cb) {
        typename T::size_type sz;
        Serializer<typename T::size_type>::read(fs, sz);

        for(typename T::size_type i = 0; i < sz; i++) {
            typename T::value_type v;
            Serializer<typename T::value_type>::read(fs, v);
            cb(std::move(v));
        }
    }
};

template<> struct Serializer<AbstractBuffer*> {
    static void write(std::fstream& fs, const AbstractBuffer* b) {
        Serializer<decltype(b->size())>::write(fs, b->size());
        fs.write(reinterpret_cast<const char*>(b->data()), b->size());
    }

    static void read(std::fstream& fs, AbstractBuffer* b) {
        decltype(b->size()) sz = 0;
        Serializer<decltype(b->size())>::read(fs, sz);

        b->resize(sz);
        fs.read(reinterpret_cast<char*>(b->data()), b->size());
    }
};

namespace Detail {

struct StructSerializer {
    StructSerializer(std::fstream& fs): m_fs(fs) { }
    template<typename T> void operator()(const char*, const T& value) { Serializer<T>::write(m_fs, value); }
    template<typename T> void operator()(const char*, T& value) { Serializer<T>::read(m_fs, value); }

    private:
        std::fstream& m_fs;
};

} // namespace Detail

template<typename T> struct Serializer< T, typename std::enable_if<::visit_struct::traits::is_visitable<T>::value>::type > {
    static void write(std::fstream& fs, const T& t) {
        Detail::StructSerializer s(fs);
        ::visit_struct::for_each(t, s);
    }

    static void read(std::fstream& fs, T& t) {
        Detail::StructSerializer s(fs);
        ::visit_struct::for_each(t, s);
    }
};

template<typename T> struct Serializer< typename std::shared_ptr<T> > {
    static void write(std::fstream& fs, const std::shared_ptr<T>& t) { Serializer<typename std::shared_ptr<T>::element_type>::write(fs, *t); }

    static void read(std::fstream& fs, std::shared_ptr<T>& t) {
        t = std::make_shared<T>();
        Serializer<typename std::shared_ptr<T>::element_type>::read(fs, *t);
    }
};

template<typename T> struct Serializer< typename std::unique_ptr<T> > {
    static void write(std::fstream& fs, const std::unique_ptr<T>& t) { Serializer<typename std::unique_ptr<T>::element_type>::write(fs, *t); }

    static void read(std::fstream& fs, std::unique_ptr<T>& t) {
        t = std::make_unique<T>();
        Serializer<typename std::unique_ptr<T>::element_type>::read(fs, *t);
    }
};

namespace SerializerHelper {

bool signatureIs(std::fstream& fs, const std::string &signatureIs);
void obfuscated(std::fstream& fs, std::string s);
void deobfuscated(std::fstream& fs, std::string& s);
bool compressed(std::fstream& fs, const AbstractBuffer *buffer);
bool decompressed(std::fstream& fs, AbstractBuffer* buffer);

} // namespace SerializerHelper

} // namespace REDasm
