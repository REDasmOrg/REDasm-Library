#pragma once

#include <cstddef>
#include <vector>

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
        enum class FindMode { Contains, LowerBound };

    private:
        typedef BlockT<T, ValueType> BlockType;
        typedef std::vector<BlockType> BlockVector;
        typedef typename BlockVector::iterator Iterator;

    public:
        typedef ValueType Value;
        typedef BlockType Block;

    public:
        void alloc(ValueType start, ValueType size, const T& data) { if(size) this->mark(start, size, false, data); }
        void free(ValueType start, ValueType size);
        size_t size() const { return m_blocks.size(); }
        bool empty() const { return m_blocks.empty(); }
        bool get(ValueType pos, BlockType* block) const;
        bool at(size_t idx, BlockType* block) const;

    private:
        size_t mark(ValueType start, ValueType size, bool free, const T& data);
        void mergeFree(size_t idx);

    private:
        bool contains(const BlockType& b, ValueType pos) const { return (pos >= b.start) && (pos < (b.start + b.size)); }

        Iterator remove(ValueType start, ValueType size) {
            ValueType end = start + size;
            auto firstblock = this->find<FindMode::LowerBound>(m_blocks.begin(), m_blocks.end(), start);
            auto lastblock = this->find<FindMode::LowerBound>(m_blocks.begin(), m_blocks.end(), end - 1);

            BlockType firstb{ }, lastb{ };

            if(firstblock != m_blocks.end()) { // Invalidate Block
                firstb.start = firstblock->start;
                firstb.size = (start >= firstblock->start) ? (start - firstblock->start) : 0;
            }

            if(lastblock != m_blocks.end()) { // Invalidate block
                lastb.size = (lastblock->start + lastblock->size) - end;
                lastb.start = end;
            }

            auto it = m_blocks.end();
            if((firstblock != m_blocks.end()) && (lastblock != m_blocks.end())) it = m_blocks.erase(firstblock, lastblock + 1);
            else if(firstblock != m_blocks.end()) it = m_blocks.erase(firstblock, lastblock);
            else if(lastblock != m_blocks.end()) it = m_blocks.erase(lastblock);

            if(firstb.size) {
                it = m_blocks.insert(it, firstb);
                it++;
            }

            if(lastb.size) it = m_blocks.insert(it, lastb);
            return it;
        }

        template<FindMode mode = FindMode::Contains, typename It> It find(It first, It last, ValueType pos) const {
            size_t count = m_blocks.size(), step;
            It it;

            while(count > 0) {
                it = first;
                step = count / 2;
                std::advance(it, step);

                if(this->contains(*it, pos)) return it;

                if((it->start < pos)) {
                   first = ++it;
                   count -= step + 1;
                }
                else
                   count = step;
            }

            if constexpr(mode == FindMode::Contains) return last;
            else return first;

        }

    private:
        BlockVector m_blocks;
};

template<typename T, typename ValueType>
void BlockContainer<T, ValueType>::free(ValueType start, ValueType size) {
    if(!size) return;
    auto idx = this->mark(start, size, true, { });
    this->mergeFree(idx);
}

template<typename T, typename ValueType>
bool BlockContainer<T, ValueType>::get(ValueType pos, BlockContainer::BlockType* block) const {
    auto it = this->find(m_blocks.begin(), m_blocks.end(), pos);
    if(it == m_blocks.end()) return false;
    *block = *it;
    return true;
}

template<typename T, typename ValueType>
bool BlockContainer<T, ValueType>::at(size_t idx, BlockContainer::BlockType* block) const {
    if(idx >= m_blocks.size()) return false;
    *block = m_blocks[idx];
    return true;
}

template<typename T, typename ValueType>
size_t BlockContainer<T, ValueType>::mark(ValueType start, ValueType size, bool free, const T& data) {
    auto it = this->remove(start, size);
    it = m_blocks.insert(it, { free, start, size, data });
    return std::distance(m_blocks.begin(), it);
}

template<typename T, typename ValueType>
void BlockContainer<T, ValueType>::mergeFree(size_t idx) {
    const BlockType& mergeb = m_blocks.at(idx);
    ValueType start = mergeb.start, size = mergeb.size;

    for(size_t i = idx; i-- > 0; ) {
        if(i == idx) continue;
        const BlockType& b = m_blocks.at(i);
        if(!b.free || (b.start + b.size) != start) break; // Check sibling
        start = b.start;
        size += b.size;
    }

    for(size_t i = idx; i < m_blocks.size(); i++) {
        if(i == idx) continue;
        const BlockType& b = m_blocks.at(i);
        if(!b.free || ((start + size) != b.start)) break; // Check sibling
        size += b.size;
    }

    if((start != mergeb.start) || (size != mergeb.size)) this->mark(start, size, true, { });
}
