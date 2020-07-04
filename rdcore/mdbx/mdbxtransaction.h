#pragma once

#include <memory>
#include <vector>
#include <string>
#include <rdapi/types.h>
#include "../libs/mdbx/mdbx.h"

typedef std::vector<u8> MDBXBuffer;

class MDBXTransaction
{
    private:
        MDBXTransaction(MDBX_env* env, unsigned int dbflags);

    public:
        ~MDBXTransaction();

    public: // Disable copy and assignment
        MDBXTransaction(const MDBXTransaction&) = delete;
        MDBXTransaction& operator=(const MDBXTransaction&) = delete;
        void commit();
        void abort();

    public: // Put
        void puts(const std::string& key, const std::string& val);
        template<typename T> void put(const std::string& key, const T& val);
        template<typename K> void putb(const K& key, const MDBXBuffer& buffer);
        template<typename K, typename V> void putr(const K& key, const V* val);
        template<typename K, typename V> void put(const K& key, const V& val);

    public: // Get
        std::string gets(const std::string& key) const;
        template<typename B, typename K> MDBXBuffer get(const K& key) const;
        template<typename T, typename K> const T* get(const K& key) const;
        template<typename T> const T* get(const std::string& k) const;
        template<typename T, typename K> void get(const K& key, T* t) const;
        template<typename T> void get(const std::string& key, T* t) const;

    private:
        void doGet(void* keydata, size_t keysize, void** valuedata, size_t* valuesize) const;
        void doPut(void* keydata, size_t keysize, void* valuedata, size_t valuesize);

    private:
        MDBX_env* m_env{nullptr};
        MDBX_txn* m_txn{nullptr};
        MDBX_dbi m_dbi;

        friend class MDBX;
};

template<typename K, typename V>
void MDBXTransaction::putr(const K& key, const V* val)
{
    this->doPut(const_cast<K*>(&key), sizeof(K),
                reinterpret_cast<void*>(const_cast<V*>(val)), sizeof(V));
}

template<typename B, typename K>
MDBXBuffer MDBXTransaction::get(const K& key) const
{
    MDBXBuffer b;
    u8* data = nullptr;
    size_t size = 0;

    this->doGet(reinterpret_cast<void*>(const_cast<K*>(&key)), key.size(),
                &data, &size);

    b.resize(size);
    std::copy(data, data + size, b.data());
    return b;
}

template<typename K>
void MDBXTransaction::putb(const K& key, const MDBXBuffer& buffer)
{
    this->doPut(reinterpret_cast<void*>(const_cast<K*>(&key)), key.size(),
                reinterpret_cast<void*>(const_cast<u8*>(buffer.data())), buffer.size());
}

template<typename T>
void MDBXTransaction::get(const std::string& key, T* t) const
{
    T* res = nullptr;
    this->doGet(const_cast<char*>(key.c_str()), key.size(), reinterpret_cast<void**>(&res), nullptr);
    *t = *res;
}

template<typename T, typename K>
void MDBXTransaction::get(const K& key, T* t) const
{
    T* res = nullptr;
    this->doGet(reinterpret_cast<void*>(const_cast<K*>(&key)), sizeof(K),
                reinterpret_cast<void**>(&res), nullptr);
    *t = *res;
}

template<typename K, typename V>
void MDBXTransaction::put(const K& key, const V& val)
{
    this->doPut(reinterpret_cast<void*>(const_cast<K*>(&key)), sizeof(K),
                reinterpret_cast<void*>(const_cast<V*>(&val)), sizeof(V));
}

template<typename T>
void MDBXTransaction::put(const std::string& key, const T& val)
{
    this->doPut(const_cast<char*>(key.c_str()), key.size(),
                reinterpret_cast<void*>(const_cast<T*>(&val)), sizeof(val));
}

template<typename T>
const T* MDBXTransaction::get(const std::string& k) const
{
    T* t = nullptr;
    this->doGet(const_cast<char*>(k.c_str()), k.size(), reinterpret_cast<void**>(&t), nullptr);
    return t;
}

template<typename T, typename K>
const T* MDBXTransaction::get(const K& key) const
{
    T* t = nullptr;
    this->doGet(reinterpret_cast<void*>(const_cast<K*>(&key)), sizeof(K),
                reinterpret_cast<void**>(&t), nullptr);
    return t;
}

typedef std::unique_ptr<MDBXTransaction> MDBXTransactionPtr;
