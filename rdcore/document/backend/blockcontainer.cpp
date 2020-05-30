#include "blockcontainer.h"
#include "../../support/error.h"
#include "../../support/utils.h"

void BlockContainer::unexplored(const RDBlock* blockitem) { this->unexplored(blockitem->start, blockitem->end); }

void BlockContainer::unexplored(address_t start)
{
    auto it = this->findOverlap(m_blocks.begin(), m_blocks.end(), start);
    if(it != m_blocks.end()) this->unexplored(it->start, it->end);
}

void BlockContainer::unexplored(address_t start, address_t end) { this->mark(start, end, BlockType_Unexplored); }
void BlockContainer::data(address_t start, address_t end) { this->mark(start, end, BlockType_Data); }
void BlockContainer::code(address_t start, address_t end) { this->mark(start, end, BlockType_Code); }
void BlockContainer::unexploredSize(address_t start, size_t size) { this->markSize(start, size, BlockType_Unexplored); }
void BlockContainer::dataSize(address_t start, size_t size) { this->markSize(start, size, BlockType_Data); }
void BlockContainer::codeSize(address_t start, size_t size) { this->markSize(start, size, BlockType_Code); }

bool BlockContainer::find(address_t address, RDBlock* block) const
{
    if(!block) return false;
    auto it = this->findOverlap(m_blocks.begin(), m_blocks.end(), address);
    if(it == m_blocks.end()) return false;
    *block = *it;
    return true;
}

bool BlockContainer::get(size_t idx, RDBlock* block) const
{
    if(!block || (idx >= m_blocks.size())) return false;
    *block = m_blocks[idx];
    return true;
}

const RDBlock& BlockContainer::at(size_t idx) const { return m_blocks[idx]; }
size_t BlockContainer::indexOf(const RDBlock* b) const
{
    auto it = this->findOverlap(m_blocks.begin(), m_blocks.end(), b->start);
    return (it != m_blocks.end()) ? std::distance(m_blocks.begin(), it) : RD_NPOS;
}

size_t BlockContainer::size() const { return m_blocks.size(); }

size_t BlockContainer::size(const RDBlock* b)
{
    if(b->start > b->end) return 0;
    return (b->end - b->start) + 1;
}

bool BlockContainer::contains(const RDBlock* b, address_t address) { return (address >= b->start) && (address <= b->end); }
bool BlockContainer::empty(const RDBlock* b) { return (b->start > b->end)|| (b->start == RD_NPOS) || (b->end == RD_NPOS); }

void BlockContainer::mark(address_t start, address_t end, type_t type)
{
    if(end < start) REDasmError("Trying to insert an empty block [" + Utils::hex(start) + ", " + Utils::hex(end) + "]");
    this->insert(start, end, type);
}

void BlockContainer::markSize(address_t start, size_t size, type_t type) { this->mark(start, start + size - 1, type); }

void BlockContainer::remove(address_t start, address_t end)
{
    auto begit = this->findOverlap(m_blocks.begin(), m_blocks.end(), start);
    auto endit = this->findOverlap(m_blocks.begin(), m_blocks.end(), end);

    if((begit == m_blocks.end()) && (endit == m_blocks.end()))
        return;

    RDBlock begbl, endbl;

    if(begit != m_blocks.end())
    {
        begbl = *begit;
        begbl.end = start - 1; // Shrink first part
    }

    if(endit != m_blocks.end())
    {
        endbl = *endit;
        endbl.start = end + 1; // Shrink last part
    }

    auto it = m_blocks.end();

    if((begit != m_blocks.end()) && (endit != m_blocks.end())) it = this->eraseRange(begit, endit + 1);
    else if(begit != m_blocks.end()) it = this->eraseBlock(begit);
    else if(endit != m_blocks.end()) it = this->eraseBlock(endit);
    else REDasmError("BlockContainer: remove failed");

    if(!BlockContainer::empty(&begbl))
    {
        begbl.type = BlockType_Unexplored; // Demote to "Unexplored"
        it = this->insertBlock(it, begbl);
        it++;
    }

    if(!BlockContainer::empty(&endbl))
    {
        endbl.type = BlockType_Unexplored; // Demote to "Unexplored"
        this->insertBlock(it, endbl);
    }
}

void BlockContainer::insert(address_t start, address_t end, type_t type)
{
    this->remove(start, end);
    auto it = this->insertionPoint(m_blocks.begin(), m_blocks.end(), start);
    this->insertBlock(it, { start, end, type });
}
