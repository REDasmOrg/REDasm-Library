#include "blockcontainer.h"
#include "../../support/error.h"
#include "../../support/utils.h"
#include <optional>

void BlockContainer::explored(rd_address start, rd_address end) { this->mark(start, end, BlockType_Unknown); }
void BlockContainer::unknown(rd_address start, rd_address end) { this->mark(start, end, BlockType_Unknown); }
void BlockContainer::data(rd_address start, rd_address end) { this->mark(start, end, BlockType_Data); }
void BlockContainer::code(rd_address start, rd_address end) { this->mark(start, end, BlockType_Code); }
void BlockContainer::string(rd_address start, rd_address end) { this->mark(start, end, BlockType_String); }
void BlockContainer::exploredSize(rd_address start, size_t size) { this->markSize(start, size, BlockType_Unknown); }
void BlockContainer::unknownSize(rd_address start, size_t size) { this->markSize(start, size, BlockType_Unknown); }
void BlockContainer::dataSize(rd_address start, size_t size) { this->markSize(start, size, BlockType_Data); }
void BlockContainer::codeSize(rd_address start, size_t size) { this->markSize(start, size, BlockType_Code); }
void BlockContainer::stringSize(rd_address start, size_t size) { this->markSize(start, size, BlockType_String); }
size_t BlockContainer::size() const { return m_container.size(); }
bool BlockContainer::empty() const { return m_container.empty(); }

size_t BlockContainer::size(const RDBlock* b)
{
    if(b->start >= b->end) return 0;
    return b->end - b->start;
}

bool BlockContainer::contains(const RDBlock* b, rd_address address) { return (address >= b->start) && (address < b->end); }
bool BlockContainer::empty(const RDBlock* b) { return b->start >= b->end; }

void BlockContainer::mark(rd_address start, rd_address end, rd_type type)
{
    if(start > end) REDasmError("Trying to insert an empty block [" + Utils::hex(start) + ", " + Utils::hex(end) + "]");

    auto begit = this->get(start);
    auto endit = this->get(end - 1);

    std::optional<RDBlock> begbl, endbl;

    if(begit != m_container.end())
    {
        begbl = *begit;
        begbl->type = BlockType_Unknown; // Demote to "Unknown"
        begbl->end = start;

        if(BlockContainer::empty(std::addressof(*begbl)))
            begbl = std::nullopt;
    }

    if(endit != m_container.end())
    {
        endbl = *endit;
        endbl->type = BlockType_Unknown; // Demote to "Unknown"
        endbl->start = end;

        if(BlockContainer::empty(std::addressof(*endbl)))
            endbl = std::nullopt;
    }

    if((begit != m_container.end()) && (endit != m_container.end())) this->doRemove(begit, std::next(endit));
    else if(begit != m_container.end()) this->doRemove(begit);
    else if(endit != m_container.end()) this->doRemove(endit);

    if(begbl)
    {
        if(this->canMerge(std::addressof(*begbl), type)) start = begbl->start;
        else this->doInsert(*begbl);
    }

    if(endbl)
    {
        if(this->canMerge(std::addressof(*endbl), type)) end = endbl->end;
        else this->doInsert(*endbl);
    }

    this->doInsert({{start}, end, type});
}

void BlockContainer::markSize(rd_address start, size_t size, rd_type type) { this->mark(start, start + size, type); }

void BlockContainer::doInsert(const RDBlock& b)
{
    auto it = m_container.find(b.end); // Check next block

    if((it != m_container.end()) && this->canMerge(std::addressof(*it), &b)) // Merge-Recurse
    {
        RDBlock mb = *it;
        mb.start = b.start;

        this->doRemove(it);
        this->doInsert(mb);
        return;
    }

    m_container.insert(b);
}

BlockContainer::ContainerType::const_iterator BlockContainer::get(rd_address address) const
{
    auto it = m_container.lower_bound(address);

    while(it != m_container.end()) {
        if(BlockContainer::contains(std::addressof(*it), address)) break;
        if(it == m_container.begin()) break;
        it--;
    }

    return it;
}

bool BlockContainer::canMerge(const RDBlock* block1, const RDBlock* block2) const { return this->canMerge(block1, block2->type); }
bool BlockContainer::canMerge(const RDBlock* block, rd_type type) const { return (block->type == type) && (type == BlockType_Unknown); }
