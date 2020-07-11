#pragma once

#include <algorithm>
#include <cstddef>
#include <vector>
#include <map>

template<typename T, typename Position = size_t>
struct BlockT
{
    bool free{true};
    Position start; Position size;
    T data{ };
};

template<typename T, typename ValueType = size_t>
class BlockContainer
{
    private:
        typedef BlockT<T, ValueType> BlockType;
        typedef std::vector<ValueType> PositionsVector;
        typedef std::map<ValueType, BlockType> BlockMap;

    public:
        typedef ValueType Value;
        typedef BlockType Block;

    public:
        void alloc(ValueType start, ValueType size, const T& data) { this->mark(start, size, false, data); }
        void unused(ValueType start, ValueType size);
        size_t size() const { return m_blocks.size(); }
        bool empty() const { return m_blocks.empty(); }
        bool get(ValueType pos, BlockType* block) const;
        bool at(size_t idx, BlockType* block) const;

    private:
        bool contains(const BlockType& b, ValueType pos) const { return (pos >= b.start) && (pos < (b.start + b.size)); }
        typename BlockContainer<T, ValueType>::PositionsVector::iterator mark(ValueType start, ValueType size, bool free, const T& data);
        void mergeFree(typename PositionsVector::iterator mergeit);
        void remove(ValueType start, ValueType size);

    private:
        PositionsVector m_positions;
        BlockMap m_blocks;
};

template<typename T, typename ValueType>
void BlockContainer<T, ValueType>::unused(ValueType start, ValueType size) {
    auto it = this->mark(start, size, true, { });
    this->mergeFree(it);
}

template<typename T, typename ValueType>
bool BlockContainer<T, ValueType>::get(ValueType pos, BlockContainer::BlockType* block) const {
    auto it = m_blocks.lower_bound(pos);
    if(it == m_blocks.end()) return false;

    if(!this->contains(it->second, pos)) // It may be contained by previous block
    {
        if(it == m_blocks.begin()) return false;
        it--;
        if(!this->contains(it->second, pos)) return false;
    }

    *block = it->second;
    return true;
}

template<typename T, typename ValueType>
bool BlockContainer<T, ValueType>::at(size_t idx, BlockContainer::BlockType* block) const {
    if(idx >= m_positions.size()) return false;
    return this->get(m_positions[idx], block);
}

template<typename T, typename ValueType>
typename BlockContainer<T, ValueType>::PositionsVector::iterator BlockContainer<T, ValueType>::mark(ValueType start, ValueType size, bool free, const T& data) {
    this->remove(start, size);
    m_blocks[start] = { free, start, size, data };

    auto it = std::lower_bound(m_positions.begin(), m_positions.end(), start);
    return m_positions.insert(it, start);
}

template<typename T, typename ValueType>
void BlockContainer<T, ValueType>::mergeFree(typename BlockContainer<T, ValueType>::PositionsVector::iterator mergeit) {
    const BlockType& mergeb = m_blocks.at(*mergeit);
    ValueType start = mergeb.start, size = 0;

    for(auto it = mergeit; ; it--) {
        const BlockType& b = m_blocks.at(*it);
        if(!b.free || (it == m_positions.begin())) break;
        start = b.start;
        size += b.size;
    }

    for(auto it = mergeit; it != m_positions.end(); it++) {
        const BlockType& b = m_blocks.at(*it);
        if(!b.free) break;
        size += b.size;
    }

    if(start != mergeb.start) this->mark(start, size, true, { });
}

template<typename T, typename ValueType>
void BlockContainer<T, ValueType>::remove(ValueType start, ValueType size) {
    ValueType end = start + size;
    auto firstblock = m_blocks.lower_bound(start);
    auto lastblock = m_blocks.lower_bound(end);

    if(firstblock != m_blocks.end()) m_blocks.erase(firstblock, lastblock);
    else if(lastblock != m_blocks.end()) m_blocks.erase(lastblock);

    auto firstpos = std::lower_bound(m_positions.begin(), m_positions.end(), start);
    auto lastpos = std::lower_bound(m_positions.begin(), m_positions.end(), end);

    if(firstpos != m_positions.end()) m_positions.erase(firstpos, lastpos);
    else if(lastpos != m_positions.end()) m_positions.erase(lastpos);
}
