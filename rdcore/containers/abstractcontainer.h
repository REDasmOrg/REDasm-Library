#pragma once

#include <cstddef>
#include <functional>
#include <rdapi/types.h>

template<typename T, typename Container>
class AbstractContainer
{
    public:
        typedef std::function<bool(const T&)> TypeCallback;
        typedef Container ContainerType;
        typedef typename ContainerType::iterator Iterator;
        typedef typename ContainerType::const_iterator ConstIterator;
        typedef T Type;

    public:
        virtual ~AbstractContainer() = default;
        size_t size() const { return m_container.size(); }
        bool empty() const { return m_container.empty(); }
        void clear() { m_container.clear(); }
        virtual const T* insert(const T& t) = 0;
        virtual bool remove(const T& t) = 0;

    public:
        void each(const TypeCallback& cb) const {
            for(auto it = this->begin(); it != this->end(); it++) {
                if(!cb(*it)) break;
            }
        }

    public: // Ranged-For support
        typename ContainerType::iterator begin() { return m_container.begin(); }
        typename ContainerType::iterator end() { return m_container.end(); }
        typename ContainerType::const_iterator begin() const { return m_container.begin(); }
        typename ContainerType::const_iterator end() const { return m_container.end(); }

    protected:
        Container m_container;
};

template<typename T, typename Container>
class RandomAccessContainer: public AbstractContainer<T, Container>
{
    public:
        typedef Container ContainerType;

    public:
        virtual ~RandomAccessContainer() = default;
        virtual T& at(size_t idx) = 0;
        virtual T& front() = 0;
        virtual T& back() = 0;
        virtual size_t indexOf(const T& t) const = 0;
        virtual bool removeAt(size_t idx) = 0;
        virtual size_t capacity() const = 0;

    public:
        const T& front() const { return const_cast<RandomAccessContainer*>(this)->front(); }
        const T& back() const { return const_cast<RandomAccessContainer*>(this)->back(); }
        const T& at(size_t idx) const { return const_cast<RandomAccessContainer*>(this)->at(idx); }
        bool contains(const T& t) const { return this->indexOf(t) != RD_NVAL; }

        bool get(size_t idx, T* t) const {
            if(idx >= this->size()) return false;
            if(t) *t = this->at(idx);
            return true;
        }
};
