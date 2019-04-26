#pragma once

#include <unordered_map>
#include <functional>
#include <algorithm>
#include <fstream>
#include "../types/buffer/memorybuffer.h"

namespace REDasm {
namespace Serializer {

class Serializable
{
    public:
        virtual void serializeTo(std::fstream& fs) = 0;
        virtual void deserializeFrom(std::fstream& fs) = 0;
};

template<typename T> void serializeScalar(std::fstream& fs, T scalar, u64 size = sizeof(T)) { fs.write(reinterpret_cast<const char*>(&scalar), size); }
template<typename T> void deserializeScalar(std::fstream& fs, T* scalar, u64 size = sizeof(T)) { fs.read(reinterpret_cast<char*>(scalar), size); }

template<template<typename, typename> class V, typename T> void serializeArray(std::fstream& fs, const V< T, std::allocator<T> >& v, const std::function<void(const T&)>& cb) {
    Serializer::serializeScalar(fs, v.size(), sizeof(u32));
    std::for_each(v.begin(), v.end(), cb);
}

template<template<typename, typename, typename> class V, typename T> void serializeArray(std::fstream& fs, const V< T, std::less<T>, std::allocator<T> >& v, const std::function<void(const T&)>& cb) {
    Serializer::serializeScalar(fs, v.size(), sizeof(u32));
    std::for_each(v.begin(), v.end(), cb);
}

template<typename K, typename V> void serializeMap(std::fstream& fs, const std::unordered_map<K, V>& v, const std::function<void(K, const V&)>& cb) {
    Serializer::serializeScalar(fs, v.size(), sizeof(u32));

    for(const auto& item : v)
        cb(item.first, item.second);
}

template<template<typename, typename> class V, typename T> void deserializeArray(std::fstream& fs, V< T, std::allocator<T> >& v, const std::function<void(T&)>& cb) {
    u32 size = 0;
    Serializer::deserializeScalar(fs, &size, sizeof(u32));

    for(u32 i = 0; i < size; i++) {
        T t;
        cb(t);
        v.push_back(t);
    }
}

template<typename K, typename V> void deserializeMap(std::fstream& fs, std::unordered_map<K, V>& v, const std::function<void(K&, V&)>& cb) {
    u32 size = 0;
    Serializer::deserializeScalar(fs, &size, sizeof(u32));

    for(u32 i = 0; i < size; i++) {
        K key; V value;
        cb(key, value);
        v.emplace(key, std::move(value));
    }
}

template<template<typename, typename, typename> class V, typename T> void deserializeArray(std::fstream& fs, V< T, std::less<T>, std::allocator<T> >& v, const std::function<void(T&)>& cb) {

    u32 size = 0;
    Serializer::deserializeScalar(fs, &size, sizeof(u32));

    for(u32 i = 0; i < size; i++) {
        T t;
        cb(t);
        v.insert(t);
    }
}

void obfuscateString(std::fstream& fs, std::string s);
void deobfuscateString(std::fstream& fs, std::string& s);
bool compressBuffer(std::fstream& fs, const AbstractBuffer *buffer);
bool decompressBuffer(std::fstream& fs, AbstractBuffer* buffer);

void serializeBuffer(std::fstream& fs, const AbstractBuffer* buffer);
void deserializeBuffer(std::fstream& fs, AbstractBuffer *buffer);
void serializeString(std::fstream& fs, const std::string& s);
void deserializeString(std::fstream& fs, std::string& s);

bool checkSignature(std::fstream& fs, const std::string &signature);

} // namespace Serializer
} // namespace REDasm
