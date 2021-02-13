#include "document.h"
#include "../plugin/assembler.h"
#include "../plugin/loader.h"
#include "../support/utils.h"
#include "../disassembler.h"
#include "../context.h"

Document::Document(const MemoryBufferPtr& buffer, Context* ctx): DocumentData(buffer, ctx) { m_net = std::make_unique<DocumentNet>(ctx); }
DocumentNet* Document::net() const { return m_net.get(); }

const Type* Document::type(const RDDocumentItem* item) const
{
    auto* itemdata = this->itemData(item);
    return itemdata ? itemdata->type.get() : nullptr;
}

bool Document::typeName(rd_address address, const std::string& q)
{
    RDDatabaseValue v;
    if(!this->context()->database()->query(q, &v)) return false;
    return this->type(address, CPTR(const Type, v.t));
}

bool Document::type(rd_address address, const Type* type) { return this->type(address, type, 0); }

void Document::checkLocation(rd_address fromaddress, rd_address address, size_t size)
{
    if(fromaddress == address) return; // Ignore self references

    RDSegment segment;
    if(!this->segment(address, &segment)) return;

    RDSymbol symbol;

    if(this->symbol(address, &symbol))
    {
        m_net->addRef(fromaddress, address);

        if(HAS_FLAG(&symbol, SymbolFlags_Pointer))
        {
            auto loc = this->dereferenceAddress(address);
            if(loc.valid) m_net->addRef(fromaddress, loc.address, ReferenceFlags_Indirect);
        }

        this->updateComments(fromaddress, address);
        return;
    }

    if(size == RD_NVAL) size = this->context()->addressWidth();

    rd_address ptraddress = 0;

    if(this->checkPointer(fromaddress, address, size, &ptraddress)) this->updateComments(fromaddress, address); // Is Pointer
    else if(this->markString(address, nullptr)) this->updateComments(fromaddress, address); // Is String
    else if(Utils::isPureCode(&segment)) // Code Reference
    {
        if(!this->label(address)) return;
        this->context()->disassembler()->enqueue(address); // Enqueue for analysis
    }
    else // Data
    {
        this->data(address, size, std::string());
        this->updateComments(fromaddress, address);
    }

    m_net->addRef(fromaddress, address);
}

bool Document::type(rd_address address, const Type* type, int level)
{
    if(!type) return false;

    //if(m_itemdata[address].type == q) return false;

    if(auto* st = dynamic_cast<const StructureType*>(type))
    {
        RDDocumentItem item = RD_DOCITEM(address, DocumentItemType_Type);
        m_itemdata[item].type.reset(type->clone(this->context()));
        m_itemdata[item].level = level;
        rd_address fieldaddress = address;

        for(const auto& [n, f] : st->fields())
        {
            this->type(fieldaddress, f, level + 1);

            auto loc = this->dereference(fieldaddress);
            if(loc.valid) this->checkLocation(fieldaddress, loc.address); // Check the pointed location...
            else this->checkLocation(address, fieldaddress);              // ...or ust add a reference to the field
            fieldaddress += f->size();
        }

        this->replace(address, DocumentItemType_Type);
    }
    else if(auto* at = dynamic_cast<const ArrayType*>(type))
    {
        RDDocumentItem item = RD_DOCITEM(address, DocumentItemType_Type);
        m_itemdata[item].type.reset(type->clone(this->context()));
        m_itemdata[item].level = level;
        rd_address itemaddress = address;

        for(size_t i = 0; i < at->itemsCount(); i++)
        {
            this->type(itemaddress, at->type());
            itemaddress += at->type()->size();
        }
    }
    else if(auto* st = dynamic_cast<const StringType*>(type))
    {
        TypePtr cst(st->clone(this->context()));
        static_cast<StringType*>(cst.get())->calculateSize(address);

        switch(cst->type())
        {
            case Type_AsciiString:
                if(!this->asciiString(address, cst->size(), cst->name(), SymbolFlags_Field)) return false;
                break;

            case Type_WideString:
                if(!this->wideString(address, cst->size(), cst->name(), SymbolFlags_Field)) return false;
                break;

            default: return false;
        }

        RDDocumentItem item = RD_DOCITEM(address, DocumentItemType_Symbol);
        m_itemdata[item].type = std::move(cst);
        m_itemdata[item].level = level;
    }
    else if(auto* nt = dynamic_cast<const NumericType*>(type))
    {
        if(!this->data(address, nt->size(), SymbolTable::name(address, nt->name()), SymbolFlags_Field))
            return false;

        RDDocumentItem item = RD_DOCITEM(address, DocumentItemType_Symbol);
        m_itemdata[item].type.reset(type->clone(this->context()));
        m_itemdata[item].level = level;
    }
    else
        return false;

    return true;
}

bool Document::checkPointer(rd_address fromaddress, rd_address address, size_t size, rd_address* firstaddress)
{
    if(size != this->context()->addressWidth()) return false;

    auto loc = this->dereferenceAddress(address);
    size_t i = 0;

    for( ; loc.valid; i++, address += size, loc = this->dereferenceAddress(address))
    {
        this->pointer(address, SymbolType_Data, std::string());

        if(!i)
        {
            m_net->addRef(fromaddress, loc.address, ReferenceFlags_Indirect);
            *firstaddress = loc.address;
        }

        this->checkLocation(address, loc.address, size);
    }

    return i;
}

void Document::updateComments(rd_address address, rd_address symboladdress, const std::string& prefix)
{
    RDSymbol symbol;
    if(!this->symbol(symboladdress, &symbol)) return;

    const char* symbolname = nullptr;

    if(HAS_FLAG(&symbol, SymbolFlags_Pointer))
    {
        u64 ptraddress = 0;
        if(!this->readAddress(symboladdress, &ptraddress) || (ptraddress == symboladdress)) return; // Avoid infinite recursion
        symbolname = this->name(ptraddress);
        if(!symbolname) return;

        this->updateComments(address, static_cast<rd_address>(ptraddress),
                             (prefix.empty() ? prefix : (prefix + " ")) + symbolname + " => ");

        return;
    }

    symbolname = this->name(symboladdress);

    if(HAS_FLAG(&symbol, SymbolFlags_WideString))
        this->autoComment(address, prefix + "WIDE STRING: " + Utils::quoted(this->readWString(symboladdress)));
    else if(HAS_FLAG(&symbol, SymbolFlags_AsciiString))
        this->autoComment(address, prefix + "STRING: " + Utils::quoted(this->readString(symboladdress)));
    else if(HAS_FLAG(&symbol, SymbolFlags_Export) && symbolname)
        this->autoComment(address, prefix + "EXPORT: " + symbolname);
    else if(IS_TYPE(&symbol, SymbolType_Import) && symbolname)
        this->autoComment(address, prefix + "IMPORT: " + symbolname);
}

size_t Document::markString(rd_address address, rd_flag* resflags)
{
    RDBufferView view;
    if(!this->view(address, &view)) return false;

    size_t totalsize = 0;
    rd_flag flags = StringFinder::categorize(this->context(), view, &totalsize);
    if(resflags) *resflags = flags;

    if(StringFinder::checkAndMark(this->context(), address, flags, totalsize)) return totalsize;
    return 0;
}
