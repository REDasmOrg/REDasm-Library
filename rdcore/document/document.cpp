#include "document.h"
#include <algorithm>
#include <cstring>
#include <cassert>
#include "backend/segmentcontainer.h"
#include "backend/functioncontainer.h"
#include "backend/instructioncache.h"
#include "backend/blockcontainer.h"
#include "backend/itemcontainer.h"
#include "backend/symboltable.h"
#include "../support/utils.h"
#include "../eventdispatcher.h"
#include "../disassembler.h"
#include "../context.h"

#define REDASM_ENTRY_FUNCTION "__redasm_entry__"

Document::Document()
{
    m_instructions = std::make_unique<InstructionCache>();
    m_segments     = std::make_unique<SegmentContainer>();
    m_functions    = std::make_unique<FunctionContainer>();
    m_blocks       = std::make_unique<BlockContainer>();
    m_items        = std::make_unique<ItemContainer>();
    m_symbols      = std::make_unique<SymbolTable>();

    m_events.insert(EventDispatcher::subscribe(Event_DocumentBlockInserted,
                                               &Document::onBlockInserted, this));

    m_events.insert(EventDispatcher::subscribe(Event_DocumentBlockRemoved,
                                               &Document::onBlockRemoved, this));
}

Document::~Document() { std::for_each(m_events.begin(), m_events.end(), &EventDispatcher::unsubscribe); }

bool Document::isInstructionCached(address_t address) const { return m_instructions->contains(address); }
const SymbolTable* Document::symbols() const { return m_symbols.get(); }
const BlockContainer* Document::blocks() const { return m_blocks.get(); }
const RDSymbol* Document::entry() const { return &m_entry; }

void Document::segment(const std::string& name, offset_t offset, address_t address, u64 psize, u64 vsize, flag_t flags)
{
    if((!(flags & SegmentFlags_Bss) && !psize) || !vsize)
    {
        rd_ctx->log("Segment '" + name + "' is empty, skipping");
        return;
    }

    size_t len = std::min<size_t>(name.size(), DEFAULT_NAME_SIZE);

    RDSegment segment{ };
    segment.offset = offset;
    segment.endoffset = offset + psize;
    segment.address = address;
    segment.endaddress = address + vsize;
    segment.flags = flags;
    std::copy_n(name.c_str(), len, reinterpret_cast<char*>(&segment.name));

    m_segments->insert(segment);
    m_blocks->unexploredSize(address, vsize);
    this->insert(address, DocumentItemType_Segment);
}

void Document::imported(address_t address, size_t size, const std::string& name) { this->block(address, size, name, SymbolType_Import, SymbolFlags_None); }
void Document::exported(address_t address, const std::string& name) { this->symbol(address, name, SymbolType_Data, SymbolFlags_Export); }
void Document::exportedFunction(address_t address, const std::string& name) { this->symbol(address, name, SymbolType_Function, SymbolFlags_Export); }

void Document::instruction(const RDInstruction* instruction)
{
    m_instructions->cache(instruction);
    m_blocks->codeSize(instruction->address, instruction->size);
}

void Document::asciiString(address_t address, size_t size) { this->block(address, size, std::string(), SymbolType_String, SymbolFlags_AsciiString); }
void Document::wideString(address_t address, size_t size) { this->block(address, size, std::string(), SymbolType_String, SymbolFlags_WideString); }
void Document::data(address_t address, size_t size, const std::string& name) { this->block(address, size, name, SymbolType_Data, SymbolFlags_None); }

void Document::table(address_t address, size_t count)
{
    this->tableItem(address, address, 0);
    this->type(address, "Table with " + Utils::number(count) + " case(s)");
}

void Document::tableItem(address_t address, address_t startaddress, u64 idx)
{
    this->block(address, rd_disasm->addressWidth(),
                SymbolTable::name(startaddress, SymbolType_Data, SymbolFlags_TableItem) + "_" + Utils::number(idx),
                SymbolType_Data, SymbolFlags_TableItem | SymbolFlags_Pointer);
}

void Document::pointer(address_t address, type_t type, const std::string& name) { this->block(address, rd_disasm->addressWidth(), name, type, SymbolFlags_Pointer); }
void Document::function(address_t address, const std::string& name) { this->symbol(address, name, SymbolType_Function, SymbolFlags_None); }

void Document::branch(address_t address, int direction)
{
    std::string name = Utils::hex(address);

    if(!direction) name = "infinite_loop_" + name;
    else name = "loc_" + name;

    this->symbol(address, name, SymbolType_Label, SymbolType_None);
}

void Document::type(address_t address, const std::string& s)
{
    if(m_itemdata[address].type == s) return;

    m_itemdata[address].type = s;
    this->replace(address, DocumentItemType_Type);
}

void Document::separator(address_t address)
{
    if(m_separators.find(address) != m_separators.end()) return;
    this->insert(address, DocumentItemType_Separator);
}

void Document::label(address_t address) { this->symbol(address, std::string(), SymbolType_Label, SymbolFlags_None); }

void Document::entry(address_t address)
{
    const char* name = m_symbols->getName(address);

    if(name) this->symbol(address, name, SymbolType_Function, SymbolFlags_Export | SymbolFlags_EntryPoint); // Don't override symbol name, if exists
    else this->symbol(address, REDASM_ENTRY_FUNCTION, SymbolType_Function, SymbolFlags_Export | SymbolFlags_EntryPoint);
    m_symbols->get(address, &m_entry);
}

void Document::empty(address_t address) { this->insert(address, DocumentItemType_Empty); }
size_t Document::blocksCount() const { return m_blocks->size(); }
size_t Document::itemsCount() const { return m_items->size(); }
size_t Document::segmentsCount() const { return m_segments->size(); }
size_t Document::functionsCount() const { return m_functions->size(); }
size_t Document::symbolsCount() const { return m_symbols->size(); }
bool Document::empty() const { return m_items->empty(); }

std::string Document::comment(address_t address, bool skipauto, const char* separator) const
{
    auto it = m_itemdata.find(address);
    if(it == m_itemdata.end()) return std::string();

    auto comments = it->second.comments;

    if(!skipauto)
        comments.insert(it->second.autocomments.begin(), it->second.autocomments.end());

    return Utils::join(comments, separator);
}

void Document::autoComment(address_t address, const std::string& s)
{
    if(s.empty()) return;

    auto it = m_itemdata[address].autocomments.insert(s);
    if(it.second) return;

    size_t idx = m_items->instructionIndex(address);
    if(idx == RD_NPOS) return;
    this->notify(m_items->functionIndex(address), DocumentAction_ItemChanged);
}

void Document::comment(address_t address, const std::string& s)
{
    m_itemdata[address].comments.clear();
    if(s.empty()) return;

    auto parts = Utils::split(s, '\n');
    for(const std::string& part : parts) m_itemdata[address].comments.insert(part);

    size_t idx = m_items->instructionIndex(address);
    if(idx == RD_NPOS) return;
    this->notify(m_items->functionIndex(address), DocumentAction_ItemChanged);
}

void Document::rename(address_t address, const std::string& newname)
{
    if(!m_symbols->rename(address, newname)) return;
    this->notify(m_items->symbolIndex(address), DocumentAction_ItemChanged);
}

size_t Document::itemsAt(size_t startidx, size_t count, RDDocumentItem* item) const
{
    if(!item) return 0;

    size_t c = 0;
    RDDocumentItem* curritem = item;

    for(size_t i = startidx; (i < m_items->size()) && (i < count); i++, c++, curritem++)
    {
        if(m_items->get(i, curritem)) break;
    }

    return c;
}

bool Document::itemAt(size_t idx, RDDocumentItem* item) const { return m_items->get(idx, item); }
bool Document::blockAt(size_t idx, RDBlock* block) const { return m_blocks->get(idx, block); }
bool Document::segmentAt(size_t idx, RDSegment* segment) const { return m_segments->get(idx, segment); }

RDLocation Document::functionAt(size_t idx) const
{
    if(idx >= m_functions->size()) return { {0}, false };
    return { {m_functions->at(idx)}, true };
}

RDLocation Document::entryPoint() const
{
    if(!IS_TYPE(&m_entry, SymbolType_Function)) return {{0}, false};
    return { {m_entry.address}, true };
}

bool Document::unlockInstruction(const RDInstruction* instruction) const { return m_instructions->unlock(instruction); }
bool Document::lockInstruction(address_t address, RDInstruction** instruction) const { return m_instructions->lock(address, instruction); }

bool Document::prevInstruction(const RDInstruction* instruction, RDInstruction** previnstruction) const
{
    RDBlock block;
    if(!m_blocks->find(instruction->address, &block)) return false;

    size_t idx = m_blocks->indexOf(&block);
    if(!m_blocks->get(--idx, &block)) return false;
    return this->lockInstruction(block.address, previnstruction);
}

bool Document::symbol(const char* name, RDSymbol* symbol) const { return m_symbols->get(name, symbol); }
bool Document::symbol(address_t address, RDSymbol* symbol) const { return m_symbols->get(address, symbol); }
bool Document::block(address_t address, RDBlock* block) const { return m_blocks->find(address, block); }
bool Document::segment(address_t address, RDSegment* segment) const { return m_segments->find(address, segment); }
bool Document::segmentOffset(offset_t offset, RDSegment* segment) const { return m_segments->findOffset(offset, segment); }
const char* Document::name(address_t address) const { return m_symbols->getName(address); }

size_t Document::itemIndex(const RDDocumentItem* item) const
{
    if(!item) return RD_NPOS;
    return m_items->indexOf(*item);
}

size_t Document::functionIndex(address_t address) const { return m_items->functionIndex(address); }
size_t Document::instructionIndex(address_t address) const { return m_items->instructionIndex(address); }
size_t Document::symbolIndex(address_t address) const { return m_items->symbolIndex(address); }

bool Document::functionItem(address_t address, RDDocumentItem* item) const
{
    size_t idx = this->functionIndex(address);
    if(idx == RD_NPOS) return false;
    return m_items->get(idx, item);
}

bool Document::instructionItem(address_t address, RDDocumentItem* item) const
{
    size_t idx = this->instructionIndex(address);
    if(idx == RD_NPOS) return false;
    return m_items->get(idx, item);
}

bool Document::symbolItem(address_t address, RDDocumentItem* item) const
{
    size_t idx = this->symbolIndex(address);
    if(idx == RD_NPOS) return false;
    return m_items->get(idx, item);
}

void Document::invalidateGraphs()
{
    while(!m_separators.empty()) this->remove(*m_separators.begin(), DocumentItemType_Separator);
    m_functions->clearGraphs();
}

void Document::graph(FunctionGraph* g) { m_functions->graph(g); }
FunctionGraph* Document::graph(address_t address) const { return m_functions->findGraph(address); }
RDLocation Document::functionStart(address_t address) const { return m_functions->findFunction(address); }

void Document::block(address_t address, size_t size, const std::string& name, type_t type, flag_t flags)
{
    if(!this->canSymbolizeAddress(address, flags)) return;

    if(!size)
    {
        rd_ctx->problem("Invalid block size @ " + Utils::hex(address));
        return;
    }

    m_blocks->dataSize(address, size);
    this->symbol(address, name, type, flags);
}

void Document::symbol(address_t address, const std::string& name, type_t type, flag_t flags)
{
    if(!this->canSymbolizeAddress(address, flags)) return;
    if(rd_disasm->needsWeak()) flags |= SymbolFlags_Weak;

    RDSymbol symbol;

    if(m_symbols->get(address, &symbol))
    {
        if(symbol.type == SymbolType_Function)
        {
            if(type == SymbolType_Function) // Overwrite symbol only
            {
                m_symbols->create(address, name, type, flags);
                this->notify(m_items->functionIndex(address), DocumentAction_ItemChanged);
                return;
            }

            this->remove(address, DocumentItemType_Function);
        }
        else
            this->remove(address, DocumentItemType_Symbol);
    }

    m_symbols->create(address, name, type, flags);
    this->insert(address, (type == SymbolType_Function) ? DocumentItemType_Function : DocumentItemType_Symbol);
}

const RDDocumentItem& Document::insert(address_t address, type_t type, u16 index)
{
    switch(type)
    {
        case DocumentItemType_Function: m_functions->insert(address); this->insert(address, DocumentItemType_Empty); break;
        case DocumentItemType_Type: this->insert(address, DocumentItemType_Empty); break;
        case DocumentItemType_Separator: m_separators.insert(address); break;
        default: break;
    }

    size_t idx = m_items->insert({ address, type, index });
    this->notify(idx, DocumentAction_ItemInserted);
    return m_items->at(idx);
}

void Document::notify(size_t idx, type_t action)
{
    if(idx >= m_items->size()) return;
    EventDispatcher::dispatch<RDDocumentEventArgs>(Event_DocumentChanged, this, action, idx, m_items->at(idx));
}

void Document::replace(address_t address, type_t type)
{
    this->remove(address, type);
    this->insert(address, type);
}

void Document::remove(address_t address, type_t type)
{
    size_t idx = m_items->indexOf({ address, type, 0 });
    if(idx == RD_NPOS) return;
    this->removeAt(idx);
}

void Document::removeAt(size_t idx)
{
    RDDocumentItem item = m_items->at(idx);
    EventDispatcher::dispatch<RDDocumentEventArgs>(Event_DocumentChanged, this, DocumentAction_ItemRemoved, idx, item);
    m_items->removeAt(idx);

    switch(item.type)
    {
        case DocumentItemType_Instruction: m_instructions->erase(item.address); break;
        case DocumentItemType_Segment: m_segments->removeAt(item.address); break;
        case DocumentItemType_Separator: m_separators.erase(item.address); break;

        case DocumentItemType_Symbol:
            if(!m_functions->contains(item.address)) m_symbols->remove(item.address); // Don't delete functions
            break;

        case DocumentItemType_Function:
            m_functions->remove(item.address);
            m_symbols->remove(item.address);
            this->remove(item.address, DocumentItemType_Empty);
            break;

        case DocumentItemType_Type:
            this->remove(item.address, DocumentItemType_Empty);
            break;

        default: break;
    }
}

bool Document::canSymbolizeAddress(address_t address, flag_t flags) const
{
    if(!m_segments->find(address, nullptr)) return false; // Ignore out of segment addresses
    if(rd_disasm->needsWeak()) flags |= SymbolFlags_Weak;

    RDBlock block;
    assert(m_blocks->find(address, &block));

    RDSymbol symbol;
    if(!m_symbols->get(block.start, &symbol)) return true;

    if(!HAS_FLAG(&symbol, SymbolFlags_Weak) && (flags & SymbolFlags_Weak)) return false;
    return true;
}

void Document::onBlockInserted(const RDEventArgs* e, void* userdata)
{
    const RDDocumentBlockEventArgs* be = reinterpret_cast<const RDDocumentBlockEventArgs*>(e);
    Document* thethis = reinterpret_cast<Document*>(userdata);

    switch(be->block.type)
    {
        case BlockType_Unexplored: thethis->insert(be->block.start, DocumentItemType_Unexplored); break;
        //case BlockType_Data: thethis->insert(be->block.start, DocumentItemType_Symbol); break;
        case BlockType_Code: thethis->insert(be->block.start, DocumentItemType_Instruction); break;
        default: break;
    }
}

void Document::onBlockRemoved(const RDEventArgs* e, void* userdata)
{
    const RDDocumentBlockEventArgs* be = reinterpret_cast<const RDDocumentBlockEventArgs*>(e);
    Document* thethis = reinterpret_cast<Document*>(userdata);

    switch(be->block.type)
    {
        case BlockType_Unexplored: thethis->remove(be->block.start, DocumentItemType_Unexplored); break;
        case BlockType_Data: thethis->remove(be->block.start, DocumentItemType_Symbol); break;
        case BlockType_Code: thethis->remove(be->block.start, DocumentItemType_Instruction); break;
        default: break;
    }
}
