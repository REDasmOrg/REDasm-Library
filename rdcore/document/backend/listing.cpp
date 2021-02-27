#include "listing.h"
#include "../../support/utils.h"
#include "../../buffer/buffer.h"
#include "../../context.h"
#include "../../config.h"

Listing::Listing(Context* ctx): Object(ctx)
{
    m_symbols = std::make_unique<SymbolTable>(ctx);
    m_segments.whenInsert(std::bind(&Listing::onBlockInserted, this, std::placeholders::_1));
    m_segments.whenRemove(std::bind(&Listing::onBlockRemoved, this, std::placeholders::_1));
}

size_t Listing::size() const { return m_items.size(); }
const ItemContainer* Listing::items() const { return &m_items; }
const FunctionContainer* Listing::functions() const { return &m_functions; }
const SegmentContainer* Listing::segments() const { return &m_segments; }
const SymbolTable* Listing::symbols() const { return m_symbols.get(); }
const BlockContainer* Listing::blocks(rd_address address) const { return m_segments.findBlocks(address); }
const RDSymbol* Listing::entry() const { return &m_entry; }
bool Listing::pointer(rd_address address, rd_type type, const std::string& name) { return this->block(address, this->context()->addressWidth(), name, type, SymbolFlags_Pointer); }

bool Listing::segment(const std::string& name, rd_offset offset, rd_address address, u64 psize, u64 vsize, rd_flag flags)
{
    if((!(flags & SegmentFlags_Bss) && !psize) || !vsize)
    {
        this->log("Segment " + Utils::quoted(name) + " is empty, skipping");
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
        auto endoffset = offset + psize;
        auto bsize = this->context()->buffer()->size();

        if(endoffset > bsize)
        {
            this->log("Segment " + Utils::quoted(name) +
                      " is bigger than loaded file (" + Utils::hex(endoffset) + " vs " + Utils::hex(bsize) + ")");

            endoffset = bsize;
        }

        segment.offset = offset;
        segment.endoffset = endoffset;
    }

    segment.address = address;
    segment.endaddress = address + vsize;
    segment.flags = flags;
    std::copy_n(name.c_str(), len, reinterpret_cast<char*>(&segment.name));

    if(!m_segments.insert(segment)) return false;
    this->insert(address, DocumentItemType_Segment);
    return true;
}

bool Listing::imported(rd_address address, size_t size, const std::string& name) { return this->block(address, size, name, SymbolType_Import, SymbolFlags_None); }
bool Listing::exported(rd_address address, size_t size, const std::string& name) { return this->block(address, size, name, SymbolType_Data, SymbolFlags_Export); }
bool Listing::exportedFunction(rd_address address, const std::string& name) { return this->symbol(address, name, SymbolType_Function, SymbolFlags_Export); }
bool Listing::instruction(rd_address address, size_t size) { return m_segments.markCode(address, size); }
bool Listing::asciiString(rd_address address, size_t size, const std::string& name, rd_flag flags) { return this->block(address, size, name, SymbolType_String, SymbolFlags_AsciiString | flags); }
bool Listing::wideString(rd_address address, size_t size, const std::string& name, rd_flag flags) { return this->block(address, size, name, SymbolType_String, SymbolFlags_WideString | flags); }

bool Listing::data(rd_address address, size_t size, std::string name, rd_flag flags)
{
    if(name.empty()) name = SymbolTable::dataName(address, size);
    return this->block(address, size, name, SymbolType_Data, SymbolFlags_None | flags);
}

bool Listing::function(rd_address address, const std::string& name) { return this->symbol(address, name, SymbolType_Function, SymbolFlags_None); }

bool Listing::branch(rd_address address, int direction)
{
    std::string name = Utils::hex(address);

    if(!direction) name = "infinite_loop_" + name;
    else name = "loc_" + name;

    return this->symbol(address, name, SymbolType_Label, SymbolType_None);
}

void Listing::separator(rd_address address)
{
    if(m_separators.find(address) != m_separators.end()) return;
    this->insert(address, DocumentItemType_Separator);
}

bool Listing::label(rd_address address) { return this->symbol(address, std::string(), SymbolType_Label, SymbolFlags_None); }

bool Listing::entry(rd_address address)
{
    const char* name = m_symbols->getName(address);
    bool res = false;

    if(name) res = this->symbol(address, name, SymbolType_Function, SymbolFlags_Export | SymbolFlags_EntryPoint); // Don't override symbol name, if exists
    else res = this->symbol(address, RD_ENTRY_NAME, SymbolType_Function, SymbolFlags_Export | SymbolFlags_EntryPoint);
    m_symbols->get(address, &m_entry);
    return res;
}

void Listing::empty(rd_address address)
{
    if(m_empties.count(address)) return; // Avoid duplicate empty items
    m_empties.insert(address);
    this->insert(address, DocumentItemType_Empty);
}

bool Listing::empty() const { return m_items.empty(); }
bool Listing::contains(const RDDocumentItem* item) const { return item && (m_items.find(*item) != m_items.end()); }

bool Listing::getAny(rd_address address, const rd_type* types, RDDocumentItem* item) const
{
    if(!types) return false;

    RDDocumentItem citem = { address, *types };

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

bool Listing::unknown(rd_address address, size_t size) { return m_segments.markUnknown(address, size); }
bool Listing::explored(rd_address address, size_t size) { return m_segments.markExplored(address, size); }

std::string Listing::comment(rd_address address, bool skipauto, const char* separator) const
{
    auto it = m_itemdata.find(RD_DOCITEM(address, DocumentItemType_Instruction));
    if(it == m_itemdata.end()) return std::string();

    auto comments = it->second.comments;

    if(!skipauto)
        comments.insert(it->second.autocomments.begin(), it->second.autocomments.end());

    return Utils::join(comments, separator);
}

void Listing::autoComment(rd_address address, const std::string& s)
{
    if(s.empty()) return;

    RDDocumentItem item = RD_DOCITEM(address, DocumentItemType_Instruction);
    auto it = m_itemdata[item].autocomments.insert(s);
    if(!it.second) return;

    if(m_items.containsInstruction(address)) this->notifyEvent(item, DocumentAction_ItemChanged);
    if(m_items.containsSymbol(address)) this->notifyEvent(item, DocumentAction_ItemChanged);
}

void Listing::comment(rd_address address, const std::string& s)
{
    RDDocumentItem item = RD_DOCITEM(address, DocumentItemType_Instruction);

    m_itemdata[item].comments.clear();
    if(s.empty()) return;

    auto parts = Utils::split(s, '\n');
    for(const std::string& part : parts) m_itemdata[item].comments.insert(part);

    if(m_items.containsInstruction(address)) this->notifyEvent(item, DocumentAction_ItemChanged);
}

bool Listing::rename(rd_address address, const std::string& newname)
{
    if(!m_symbols->rename(address, newname)) return false;
    if(!m_items.containsSymbol(address)) return false;

    this->notifyEvent(RD_DOCITEM(address, DocumentItemType_Symbol), DocumentAction_ItemChanged);
    return true;
}

RDLocation Listing::entryPoint() const
{
    if(!IS_TYPE(&m_entry, SymbolType_Function)) return {{0}, false};
    return { {m_entry.address}, true };
}

bool Listing::getEntryItem(RDDocumentItem* item) const
{
    auto loc = this->entryPoint();
    if(!loc.valid) return false;
    if(item) *item = RD_DOCITEM(loc.address, DocumentItemType_Instruction);
    return true;
}

const Listing::ItemData* Listing::itemData(const RDDocumentItem* item) const
{
    if(!item) return nullptr;
    auto it = m_itemdata.find(*item);
    return (it != m_itemdata.end()) ? std::addressof(it->second) : 0;
}

bool Listing::symbol(const char* name, RDSymbol* symbol) const { return m_symbols->get(name, symbol); }
bool Listing::symbol(rd_address address, RDSymbol* symbol) const { return m_symbols->get(address, symbol); }
bool Listing::block(rd_address address, RDBlock* block) const { return m_segments.findBlock(address, block); }
bool Listing::segment(rd_address address, RDSegment* segment) const { return m_segments.find(address, segment); }
bool Listing::segmentOffset(rd_offset offset, RDSegment* segment) const { return m_segments.findOffset(offset, segment); }
const char* Listing::name(rd_address address) const { return m_symbols->getName(address); }

void Listing::invalidateGraphs()
{
    while(!m_separators.empty()) this->remove(*m_separators.begin(), DocumentItemType_Separator);
    m_functions.clearGraphs();
}

void Listing::graph(FunctionGraph* g) { m_functions.graph(g); }
FunctionGraph* Listing::graph(rd_address address) const { return m_functions.findGraph(address); }
RDLocation Listing::functionStart(rd_address address) const { return m_functions.findFunction(address); }

bool Listing::block(rd_address address, size_t size, const std::string& name, rd_type type, rd_flag flags)
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

bool Listing::symbol(rd_address address, const std::string& name, rd_type type, rd_flag flags)
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
                this->notifyEvent({ address, DocumentItemType_Function }, DocumentAction_ItemChanged);
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

const RDDocumentItem& Listing::insert(rd_address address, rd_type type)
{
    switch(type)
    {
        case DocumentItemType_Function: m_functions.insert(address); break;
        case DocumentItemType_Type: this->empty(address); break;
        case DocumentItemType_Separator: m_separators.insert(address); break;
        default: break;
    }

    const RDDocumentItem* item = m_items.insert({ address, type });
    this->notifyEvent(*item, DocumentAction_ItemInserted);
    return *item;
}

void Listing::notifyEvent(const RDDocumentItem& item, RDDocumentAction action) { this->context()->notify<RDDocumentEventArgs>(Event_DocumentChanged, this, action, item); }

const RDDocumentItem& Listing::replace(rd_address address, rd_type type)
{
    this->remove(address, type);
    return this->insert(address, type);
}

void Listing::remove(rd_address address, rd_type type)
{
    RDDocumentItem item = { address, type };
    this->context()->notify<RDDocumentEventArgs>(Event_DocumentChanged, this, DocumentAction_ItemRemoved, item);
    m_items.remove(item);

    switch(item.type)
    {
        case DocumentItemType_Segment: m_segments.removeKey(item.address); break;
        case DocumentItemType_Separator: m_separators.erase(item.address); break;

        case DocumentItemType_Symbol:
            if(!m_functions.contains(item.address)) m_symbols->remove(item.address); // Don't delete functions
            break;

        case DocumentItemType_Function:
            m_functions.remove(item.address);
            m_symbols->remove(item.address);
            break;

        default: break;
    }
}

bool Listing::canSymbolizeAddress(rd_address address, rd_flag flags) const
{
    if(!m_segments.find(address, nullptr)) return false; // Ignore out of segment addresses
    if(this->context()->needsWeak()) flags |= SymbolFlags_Weak;

    RDBlock block;
    if(!m_segments.findBlock(address, &block)) return false;
    if(IS_TYPE(&block, BlockType_Unknown)) return true;

    RDSymbol symbol;
    if(!m_symbols->get(block.start, &symbol)) return true;
    if(!HAS_FLAG(&symbol, SymbolFlags_Weak) && (flags & SymbolFlags_Weak)) return false;
    return true;
}

void Listing::onBlockInserted(const RDBlock& b)
{
    switch(b.type)
    {
        case BlockType_Unknown: this->insert(b.start, DocumentItemType_Unknown); break;
        //case BlockType_Data: thethis->insert(b.start, DocumentItemType_Symbol); break;
        case BlockType_Code: this->insert(b.start, DocumentItemType_Instruction); break;
        default: break;
    }
}

void Listing::onBlockRemoved(const RDBlock& b)
{
    switch(b.type)
    {
        case BlockType_Unknown: this->remove(b.start, DocumentItemType_Unknown); break;
        case BlockType_Data: this->remove(b.start, DocumentItemType_Symbol); break;
        case BlockType_Code: this->remove(b.start, DocumentItemType_Instruction); break;
        default: break;
    }
}
