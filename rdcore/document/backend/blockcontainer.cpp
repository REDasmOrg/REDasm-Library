#include "blockcontainer.h"
#include "../../support/error.h"
#include "../../support/utils.h"

BlockContainer::BlockContainer()
{
    m_oninsert = [](const RDBlock&) { };
    m_onremove = [](const RDBlock&) { };
}

void BlockContainer::whenInsert(const BlockContainer::Callback& cb) { m_oninsert = cb; }
void BlockContainer::whenRemove(const BlockContainer::Callback& cb) { m_onremove = cb; }
void BlockContainer::unexplored(const RDBlock* blockitem) { this->unexplored(blockitem->start, blockitem->end); }

void BlockContainer::unexplored(rd_address start)
{
    auto it = this->find(m_blocks.begin(), m_blocks.end(), start);
    if(it != m_blocks.end()) this->unexplored(it->start, it->end);
}

void BlockContainer::unexplored(rd_address start, rd_address end) { this->mark(start, end, BlockType_Unexplored); }
void BlockContainer::data(rd_address start, rd_address end) { this->mark(start, end, BlockType_Data); }
void BlockContainer::code(rd_address start, rd_address end) { this->mark(start, end, BlockType_Code); }
void BlockContainer::unexploredSize(rd_address start, size_t size) { this->markSize(start, size, BlockType_Unexplored); }
void BlockContainer::dataSize(rd_address start, size_t size) { this->markSize(start, size, BlockType_Data); }
void BlockContainer::codeSize(rd_address start, size_t size) { this->markSize(start, size, BlockType_Code); }

bool BlockContainer::contains(rd_address address) const
{
    if(m_blocks.empty()) return false;

    const auto& f = m_blocks.front();
    const auto& b = m_blocks.back();
    return (address >= f.start) >= (address <= b.end);
}

bool BlockContainer::find(rd_address address, RDBlock* block) const
{
    if(!block) return false;
    auto it = this->find(m_blocks.begin(), m_blocks.end(), address);
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

const RDBlock& BlockContainer::front() const { return m_blocks.front(); }
const RDBlock& BlockContainer::back() const { return m_blocks.back(); }
const RDBlock& BlockContainer::at(size_t idx) const { return m_blocks[idx]; }

size_t BlockContainer::indexOf(const RDBlock* b) const
{
    auto it = this->find(m_blocks.begin(), m_blocks.end(), b->start);
    return (it != m_blocks.end()) ? std::distance(m_blocks.begin(), it) : RD_NPOS;
}

size_t BlockContainer::size() const { return m_blocks.size(); }

size_t BlockContainer::size(const RDBlock* b)
{
    if(b->start > b->end) return 0;
    return (b->end - b->start) + 1;
}

bool BlockContainer::contains(const RDBlock* b, rd_address address) { return (address >= b->start) && (address <= b->end); }
bool BlockContainer::empty(const RDBlock* b) { return (b->start > b->end)|| (b->start == RD_NPOS) || (b->end == RD_NPOS); }

void BlockContainer::mark(rd_address start, rd_address end, rd_type type)
{
    if(end < start) REDasmError("Trying to insert an empty block [" + Utils::hex(start) + ", " + Utils::hex(end) + "]");
    this->insert(start, end, type);
}

void BlockContainer::markSize(rd_address start, size_t size, rd_type type) { this->mark(start, start + size - 1, type); }

void BlockContainer::remove(rd_address start, rd_address end)
{
    auto begit = this->find(m_blocks.begin(), m_blocks.end(), start);
    auto endit = this->find(m_blocks.begin(), m_blocks.end(), end);

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

    if((begit != m_blocks.end()) && (endit != m_blocks.end())) it = this->doRemove(begit, endit + 1);
    else if(begit != m_blocks.end()) it = this->doRemove(begit);
    else if(endit != m_blocks.end()) it = this->doRemove(endit);
    else REDasmError("BlockContainer: remove failed");

    if(!BlockContainer::empty(&begbl))
    {
        begbl.type = BlockType_Unexplored; // Demote to "Unexplored"
        it = this->doInsert(it, begbl);
        it++;
    }

    if(!BlockContainer::empty(&endbl))
    {
        endbl.type = BlockType_Unexplored; // Demote to "Unexplored"
        this->doInsert(it, endbl);
    }
}

void BlockContainer::insert(rd_address start, rd_address end, rd_type type)
{
    this->remove(start, end);
    auto it = this->find<FindMode::LowerBound>(m_blocks.begin(), m_blocks.end(), start);
    this->doInsert(it, { {start}, end, type });
}
