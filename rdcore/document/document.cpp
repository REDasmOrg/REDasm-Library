#include "document.h"
#include <algorithm>
#include <cstring>
#include "backend/segmentcontainer.h"
#include "backend/functioncontainer.h"
#include "backend/blockcontainer.h"
#include "backend/itemcontainer.h"
#include "backend/symboltable.h"
#include "../disassembler.h"
#include "../support/utils.h"
#include "../disassembler.h"
#include "../context.h"
#include "../config.h"

Document::Document(Context* ctx): Object(ctx)
{
    m_symbols = std::make_unique<SymbolTable>(ctx);
    m_segments.whenInsert(std::bind(&Document::onBlockInserted, this, std::placeholders::_1));
    m_segments.whenRemove(std::bind(&Document::onBlockRemoved, this, std::placeholders::_1));
}

size_t Document::size() const { return m_items.size(); }
const ItemContainer* Document::items() const { return &m_items; }
const FunctionContainer* Document::functions() const { return &m_functions; }
const SegmentContainer* Document::segments() const { return &m_segments; }
const SymbolTable* Document::symbols() const { return m_symbols.get(); }
const BlockContainer* Document::blocks(rd_address address) const { return m_segments.findBlocks(address); }
const RDSymbol* Document::entry() const { return &m_entry; }

bool Document::segment(const std::string& name, rd_offset offset, rd_address address, u64 psize, u64 vsize, rd_flag flags)
{
    if((!(flags & SegmentFlags_Bss) && !psize) || !vsize)
    {
        rd_cfg->log("Segment '" + name + "' is empty, skipping");
        return false;
    }

    size_t len = std::min<size_t>(name.size(), DEFAULT_NAME_SIZE);

    RDSegment segment{ };

    if(flags & SegmentFlags_Bss)
    {
        segment.offset = 0;
        segment.endoffset = 0;
    }
    else
    {
        segment.offset = offset;
        segment.endoffset = offset + psize;
    }

    segment.address = address;
    segment.endaddress = address + vsize;
    segment.flags = flags;
    std::copy_n(name.c_str(), len, reinterpret_cast<char*>(&segment.name));

    if(!m_segments.insert(segment)) return false;
    //if(idx) this->insert(address, DocumentItemType_Empty);
    this->insert(address, DocumentItemType_Segment);
    return true;
}

bool Document::imported(rd_address address, size_t size, const std::string& name) { return this->block(address, size, name, SymbolType_Import, SymbolFlags_None); }
bool Document::exported(rd_address address, size_t size, const std::string& name) { return this->block(address, size, name, SymbolType_Data, SymbolFlags_Export); }
bool Document::exportedFunction(rd_address address, const std::string& name) { return this->symbol(address, name, SymbolType_Function, SymbolFlags_Export); }
bool Document::instruction(rd_address address, size_t size) { return m_segments.markCode(address, size); }
bool Document::asciiString(rd_address address, size_t size, const std::string& name) { return this->block(address, size, name, SymbolType_String, SymbolFlags_AsciiString); }
bool Document::wideString(rd_address address, size_t size, const std::string& name) { return this->block(address, size, name, SymbolType_String, SymbolFlags_WideString); }
bool Document::data(rd_address address, size_t size, const std::string& name) { return this->block(address, size, name, SymbolType_Data, SymbolFlags_None); }

void Document::table(rd_address address, size_t count)
{
    this->tableItem(address, address, 0);
    //FIXME: this->type(address, "Table with " + Utils::number(count) + " case(s)");
}

void Document::tableItem(rd_address address, rd_address startaddress, size_t idx)
{
    this->block(address, this->context()->addressWidth(),
                SymbolTable::name(startaddress, SymbolType_Data, SymbolFlags_TableItem) + "_" + Utils::number(idx),
                SymbolType_Data, SymbolFlags_TableItem | SymbolFlags_Pointer);
}

bool Document::pointer(rd_address address, rd_type type, const std::string& name) { return this->block(address, this->context()->addressWidth(), name, type, SymbolFlags_Pointer); }

bool Document::function(rd_address address, const std::string& name)
{
    return this->symbol(address, name, SymbolType_Function, SymbolFlags_None);
}

bool Document::branch(rd_address address, int direction)
{
    std::string name = Utils::hex(address);

    if(!direction) name = "infinite_loop_" + name;
    else name = "loc_" + name;

    return this->symbol(address, name, SymbolType_Label, SymbolType_None);
}

bool Document::typeName(rd_address address, const std::string& q)
{
    RDDatabaseValue v;
    if(!this->context()->database()->query(q, &v)) return false;
    return this->type(address, CPTR(const Type, v.t));
}

bool Document::type(rd_address address, const Type* type)
{
    if(!type) return false;

    //if(m_itemdata[address].type == q) return false;
    m_itemdata[address].type.reset(type->clone());

    if(!m_segments.markData(address, type->size())) return false;
    this->replace(address, DocumentItemType_Type);
    return true;
}

void Document::separator(rd_address address)
{
    if(m_separators.find(address) != m_separators.end()) return;
    this->insert(address, DocumentItemType_Separator);
}

bool Document::label(rd_address address) { return this->symbol(address, std::string(), SymbolType_Label, SymbolFlags_None); }

bool Document::entry(rd_address address)
{
    const char* name = m_symbols->getName(address);
    bool res = false;

    if(name) res = this->symbol(address, name, SymbolType_Function, SymbolFlags_Export | SymbolFlags_EntryPoint); // Don't override symbol name, if exists
    else res = this->symbol(address, RD_ENTRY_NAME, SymbolType_Function, SymbolFlags_Export | SymbolFlags_EntryPoint);
    m_symbols->get(address, &m_entry);
    return res;
}

void Document::empty(rd_address address) { this->insert(address, DocumentItemType_Empty); }
bool Document::empty() const { return m_items.empty(); }
bool Document::contains(const RDDocumentItem* item) const { return item && (m_items.find(*item) != m_items.end()); }

bool Document::getAny(rd_address address, const rd_type* types, RDDocumentItem* item) const
{
    if(!types) return false;

    RDDocumentItem citem = { address, *types, 0 };

    while(citem.type != DocumentItemType_None)
    {
        if(this->contains(&citem))
        {
            if(item) *item = citem;
            return true;
        }

        citem.type = *(++types);
    }

    return false;
}

std::string Document::comment(rd_address address, bool skipauto, const char* separator) const
{
    auto it = m_itemdata.find(address);
    if(it == m_itemdata.end()) return std::string();

    auto comments = it->second.comments;

    if(!skipauto)
        comments.insert(it->second.autocomments.begin(), it->second.autocomments.end());

    return Utils::join(comments, separator);
}

void Document::autoComment(rd_address address, const std::string& s)
{
    if(s.empty()) return;

    auto it = m_itemdata[address].autocomments.insert(s);
    if(!it.second) return;

    if(m_items.containsInstruction(address)) this->notifyEvent({ address, DocumentItemType_Instruction, 0 }, DocumentAction_ItemChanged);
    if(m_items.containsSymbol(address)) this->notifyEvent({ address, DocumentItemType_Symbol, 0 }, DocumentAction_ItemChanged);
}

void Document::comment(rd_address address, const std::string& s)
{
    m_itemdata[address].comments.clear();
    if(s.empty()) return;

    auto parts = Utils::split(s, '\n');
    for(const std::string& part : parts) m_itemdata[address].comments.insert(part);

    if(m_items.containsInstruction(address)) this->notifyEvent({ address, DocumentItemType_Instruction, 0 }, DocumentAction_ItemChanged);
}

bool Document::rename(rd_address address, const std::string& newname)
{
    if(!m_symbols->rename(address, newname)) return false;
    if(!m_items.containsSymbol(address)) return false;

    this->notifyEvent({ address, DocumentItemType_Symbol, 0 }, DocumentAction_ItemChanged);
    return true;
}

RDLocation Document::entryPoint() const
{
    if(!IS_TYPE(&m_entry, SymbolType_Function)) return {{0}, false};
    return { {m_entry.address}, true };
}

const Type* Document::type(rd_address address) const
{
    auto it = m_itemdata.find(address);
    return (it != m_itemdata.end()) ? it->second.type.get() : nullptr;
}

bool Document::symbol(const char* name, RDSymbol* symbol) const { return m_symbols->get(name, symbol); }
bool Document::symbol(rd_address address, RDSymbol* symbol) const { return m_symbols->get(address, symbol); }
bool Document::block(rd_address address, RDBlock* block) const { return m_segments.findBlock(address, block); }
bool Document::segment(rd_address address, RDSegment* segment) const { return m_segments.find(address, segment); }
bool Document::segmentOffset(rd_offset offset, RDSegment* segment) const { return m_segments.findOffset(offset, segment); }
const char* Document::name(rd_address address) const { return m_symbols->getName(address); }

void Document::invalidateGraphs()
{
    while(!m_separators.empty()) this->remove(*m_separators.begin(), DocumentItemType_Separator);
    m_functions.clearGraphs();
}

void Document::graph(FunctionGraph* g) { m_functions.graph(g); }
FunctionGraph* Document::graph(rd_address address) const { return m_functions.findGraph(address); }
RDLocation Document::functionStart(rd_address address) const { return m_functions.findFunction(address); }
bool Document::setSegmentUserData(rd_address address, uintptr_t userdata) { return m_segments.setUserData(address, userdata); }

bool Document::block(rd_address address, size_t size, const std::string& name, rd_type type, rd_flag flags)
{
    if(!this->canSymbolizeAddress(address, flags)) return false;

    if(!size)
    {
        this->context()->problem("Invalid block size @ " + Utils::hex(address));
        return false;
    }

    if(m_segments.markData(address, size)) return this->symbol(address, name, type, flags);
    return false;
}

bool Document::symbol(rd_address address, const std::string& name, rd_type type, rd_flag flags)
{
    if(!this->canSymbolizeAddress(address, flags)) return false;
    if(this->context()->needsWeak()) flags |= SymbolFlags_Weak;

    RDSymbol symbol;

    if(m_symbols->get(address, &symbol))
    {
        if(symbol.type == SymbolType_Function)
        {
            if(type == SymbolType_Function) // Overwrite symbol only
            {
                const char* n = m_symbols->getName(address); // Try to preserve old name, if any
                m_symbols->create(address, (n && name.empty()) ? n : name, type, flags);
                this->notifyEvent({ address, DocumentItemType_Function, 0 }, DocumentAction_ItemChanged);
                return true;
            }

            this->remove(address, DocumentItemType_Function);
        }
        else
            this->remove(address, DocumentItemType_Symbol);
    }

    m_symbols->create(address, name, type, flags);
    this->insert(address, (type == SymbolType_Function) ? DocumentItemType_Function : DocumentItemType_Symbol);
    return true;
}

const RDDocumentItem& Document::insert(rd_address address, rd_type type, u16 index)
{
    switch(type)
    {
        case DocumentItemType_Function: m_functions.insert(address); break;
        case DocumentItemType_Type: this->empty(address); break;
        case DocumentItemType_Separator: m_separators.insert(address); break;
        default: break;
    }

    const RDDocumentItem* item = m_items.insert({ address, type, index });
    this->notifyEvent(*item, DocumentAction_ItemInserted);
    return *item;
}

void Document::notifyEvent(const RDDocumentItem& item, const rd_type action) { this->context()->notify<RDDocumentEventArgs>(Event_DocumentChanged, this, action, item); }

void Document::replace(rd_address address, rd_type type)
{
    this->remove(address, type);
    this->insert(address, type);
}

void Document::remove(rd_address address, rd_type type)
{
    RDDocumentItem item = { address, type, 0 };
    this->context()->notify<RDDocumentEventArgs>(Event_DocumentChanged, this, DocumentAction_ItemRemoved, item);
    m_items.remove(item);

    switch(item.type)
    {
        case DocumentItemType_Segment: m_segments.removeAt(item.address); break;
        case DocumentItemType_Separator: m_separators.erase(item.address); break;

        case DocumentItemType_Symbol:
            if(!m_functions.contains(item.address)) m_symbols->remove(item.address); // Don't delete functions
            break;

        case DocumentItemType_Function:
            m_functions.remove(item.address);
            m_symbols->remove(item.address);
            break;

        case DocumentItemType_Type:
            this->remove(item.address, DocumentItemType_Empty);
            break;

        default: break;
    }
}

bool Document::canSymbolizeAddress(rd_address address, rd_flag flags) const
{
    if(!m_segments.find(address, nullptr)) return false; // Ignore out of segment addresses
    if(this->context()->needsWeak()) flags |= SymbolFlags_Weak;

    RDBlock block;
    if(!m_segments.findBlock(address, &block)) return false;

    RDSymbol symbol;
    if(!m_symbols->get(block.start, &symbol)) return true;

    if(!HAS_FLAG(&symbol, SymbolFlags_Weak) && (flags & SymbolFlags_Weak)) return false;
    return true;
}

void Document::onBlockInserted(const RDBlock& b)
{
    switch(b.type)
    {
        case BlockType_Unexplored: this->insert(b.start, DocumentItemType_Unexplored); break;
        //case BlockType_Data: thethis->insert(b.start, DocumentItemType_Symbol); break;
        case BlockType_Code: this->insert(b.start, DocumentItemType_Instruction); break;
        default: break;
    }
}

void Document::onBlockRemoved(const RDBlock& b)
{
    switch(b.type)
    {
        case BlockType_Unexplored: this->remove(b.start, DocumentItemType_Unexplored); break;
        case BlockType_Data: this->remove(b.start, DocumentItemType_Symbol); break;
        case BlockType_Code: this->remove(b.start, DocumentItemType_Instruction); break;
        default: break;
    }
}
