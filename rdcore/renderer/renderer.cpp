#include "renderer.h"
#include "surface.h"
#include "../document/document.h"
#include "../plugin/assembler.h"
#include "../rdil/ilfunction.h"
#include "../rdil/rdil.h"
#include "../support/utils.h"
#include "../disassembler.h"
#include "../context.h"
#include <rdapi/renderer/surface.h>
#include <cstring>

#define INDENT_COMMENT      10
#define STRING_THRESHOLD    48
#define SEPARATOR_LENGTH    50
#define UNKNOWN_STRING      "???"
#define COMMENT_SEPARATOR   " | "

Renderer::Renderer(Context* ctx, rd_flag flags): Object(ctx), m_flags(flags) { }

bool Renderer::render(const RDDocumentItem* item)
{
    if(!item) return false;

    switch(item->type)
    {
        case DocumentItemType_Segment:     this->renderSegment(item);     break;
        case DocumentItemType_Function:    this->renderFunction(item);    break;
        case DocumentItemType_Instruction: this->renderInstruction(item); break;
        case DocumentItemType_Symbol:      this->renderSymbol(item);      break;
        case DocumentItemType_Unknown:     this->renderUnknown(item);     break;
        case DocumentItemType_Separator:   this->renderSeparator(item);   break;
        case DocumentItemType_Type:        this->renderType(item);        break;
        case DocumentItemType_Empty:       return true;
        default:                           return false;
    }

    return true;
}

void Renderer::renderHexDump(const RDBufferView* view, size_t size)
{
    if(!view) return;
    this->chunk(Utils::hexStringEndian(this->context(), view, size), Theme_Constant);
    this->renderIndent(1);
}

void Renderer::renderAssemblerInstruction(rd_address address)
{
    RDRendererParams srp;
    this->compileParams(address, &srp);

    if(!this->assembler()->renderInstruction(&srp))
        this->chunk(UNKNOWN_STRING);
}

void Renderer::renderRDILInstruction(rd_address address)
{
    RDRendererParams srp;
    this->compileParams(address, &srp);

    ILFunction il(this->context());
    this->context()->assembler()->lift(srp.address, &srp.view, &il);

    for(size_t i = 0; i < il.size(); i++)
    {
        if(i) this->chunk("; "); // Attach more statements, if needed
        RDIL::render(il.expression(i), this, address);
    }
}

void Renderer::renderSigned(s64 value) { this->chunk(Utils::hex(value), Theme_Constant); }
void Renderer::renderUnsigned(u64 value) { this->chunk(Utils::hex(value), Theme_Constant); }

void Renderer::renderReference(rd_location loc)
{
    const char* name = this->document()->name(loc);
    if(name) this->chunk(name, Theme_Symbol);
    else this->chunk(Utils::hex(loc), Theme_Constant);
}

void Renderer::renderMnemonic(const std::string& s, rd_type theme)
{
    if(s.empty()) return;

    this->chunk(s, theme);

    auto& ss = this->context()->surfaceState();
    ss.mnemonicendcol = std::max(ss.mnemonicendcol, s.size());

    size_t diff = ss.mnemonicendcol - s.size();
    if(diff) this->chunk(std::string(diff, ' '));
}

void Renderer::renderMnemonicWord(const std::string& s, rd_type theme)
{
    this->renderMnemonic(s, theme);
    this->chunk(" ");
}

void Renderer::renderRegister(const std::string& s) { this->chunk(s, Theme_Reg); }
void Renderer::renderConstant(const std::string& s) { this->chunk(s, Theme_Constant); }
void Renderer::renderText(const std::string& s, rd_type theme) { this->chunk(s, theme); }
void Renderer::renderUnknown() { this->chunk(UNKNOWN_STRING); }

const std::string& Renderer::text() const { return m_text; }
const Renderer::Chunks& Renderer::chunks() const { return m_tokens; }

void Renderer::renderSegment(const RDDocumentItem* item)
{
    this->renderPrologue(item->address);

    StyleScope s(this, Theme_Segment);
    RDSegment segment;

    if(this->context()->document()->segment(item->address, &segment))
    {
        this->chunk(segment.name);
        this->renderInstrIndent(segment.name);

        this->chunk("segment").chunk(" ")
             .chunk("(")
             .chunk("START").chunk(" ").chunk(Utils::hex(segment.address, this->assembler()->bits())).chunk(", ")
             .chunk("END").chunk(" ").chunk(Utils::hex(segment.endaddress, this->assembler()->bits()))
             .chunk(")");
    }
    else
    {
        std::string name = Utils::hex(item->address);
        this->chunk(name);
        this->renderInstrIndent(name);
        this->chunk(" ").chunk("segment");
    }
}

void Renderer::renderFunction(const RDDocumentItem* item)
{
    this->renderPrologue(item->address);

    StyleScope s(this, Theme_Function);
    const char* fname = this->document()->name(item->address);

    std::string name = fname ? fname : Utils::hex(item->address);
    this->chunk(name).chunk(" ");
    this->renderInstrIndent(name + " ");
    this->chunk("function");
}

void Renderer::renderInstruction(const RDDocumentItem* item)
{
    this->renderPrologue(item->address);
    this->renderInstrIndent(std::string());

    if(this->context()->flags() & ContextFlags_ShowRDIL)
        this->renderRDILInstruction(item->address);
    else
        this->renderAssemblerInstruction(item->address);

    this->renderComments(item->address);
}

void Renderer::renderSymbol(const RDDocumentItem* item)
{
    this->renderPrologue(item->address);

    const char* name = this->document()->name(item->address);
    RDSymbol symbol;

    if(!name || !this->document()->symbol(item->address, &symbol))
    {
        this->chunk(Utils::hex(item->address, this->assembler()->bits()), Theme_Constant);
        return;
    }

    if(HAS_FLAG(&symbol, SymbolFlags_Field))
    {
        auto* itemdata = this->document()->itemData(item);
        this->renderIndent(itemdata ? itemdata->level : 0, true);
    }

    if(!HAS_FLAG(&symbol, SymbolFlags_Pointer))
    {
        switch(symbol.type)
        {
            case SymbolType_Data: this->chunk(name, Theme_Data); break;
            case SymbolType_String: this->chunk(name, Theme_String); break;
            case SymbolType_Import: this->chunk(name, Theme_Import); break;
            case SymbolType_Label: this->chunk(name, Theme_Symbol).chunk(":"); return;
            case SymbolType_Function: this->chunk(name, Theme_Function); return;
            default: this->chunk(name); return;
        }
    }
    else
        this->chunk(name, Theme_Pointer);

    this->renderInstrIndent(name, true);
    this->renderSymbolValue(&symbol);
    this->renderComments(item->address);
}

void Renderer::renderUnknown(const RDDocumentItem* item)
{
    this->renderPrologue(item->address);
    this->renderInstrIndent(std::string());

    RDBlock block;
    if(!this->document()->block(item->address, &block)) REDasmError("Invalid Block", item->address);

    switch(BlockContainer::size(&block))
    {
        case 2:  this->chunk("word", Theme_Nop);  break;
        case 4:  this->chunk("dword", Theme_Nop); break;
        case 8:  this->chunk("qword", Theme_Nop); break;
        default: this->chunk("byte", Theme_Nop);  break;
    }

    this->chunk(" ");
    this->renderBlock(item->address);
}

void Renderer::renderSeparator(const RDDocumentItem* item)
{
    if(this->hasFlag(RendererFlags_NoSeparators)) return;
    if(!this->hasFlag(RendererFlags_NoSegmentAndAddress)) this->renderAddressIndent(item->address);
    this->chunk(std::string(SEPARATOR_LENGTH, '-'), Theme_Comment);
}

void Renderer::renderType(const RDDocumentItem* item)
{
    this->renderPrologue(item->address);
    auto type = this->document()->type(item);
    if(type) this->chunk(type->name(), Theme_Symbol).chunk(" ").chunk(type->typeName(), Theme_Type);
    else this->chunk("Type not Found");
}

void Renderer::renderInstrIndent(const std::string& diffstr, bool ignoreflags)
{
    auto& ss = this->context()->surfaceState();
    if(ss.instrstartcol <= diffstr.size()) return;
    this->renderIndent(ss.instrstartcol - diffstr.size(), ignoreflags);
}

void Renderer::renderIndent(size_t n, bool ignoreflags)
{
    if(!ignoreflags && this->hasFlag(RendererFlags_NoIndent)) return;
    this->chunk(std::string(n, ' '));
}

void Renderer::renderPrologue(rd_address address)
{
    if(!this->hasFlag(RendererFlags_NoSegment))
    {
        RDSegment s;
        auto& doc = this->context()->document();

        if(doc->segment(address, &s)) this->chunk(s.name);
        else this->chunk(UNKNOWN_STRING);
        this->chunk(":");
    }

    if(!this->hasFlag(RendererFlags_NoAddress))
        this->chunk(Utils::hex(address, this->assembler()->bits()));

    this->chunk(" ");
}

void Renderer::renderBlock(rd_address address)
{
    RDBlock block;
    if(!this->document()->block(address, &block)) REDasmError("Invalid Block", address);

    size_t blocksize = BlockContainer::size(&block);
    RDBufferView view;

    if((blocksize <= sizeof(rd_address)) && this->disassembler()->view(block.address, blocksize, &view))
        this->renderHexDump(&view, RD_NVAL);
    else
        this->chunk("(").chunk(Utils::hex(blocksize), Theme_Constant).chunk(")");}

void Renderer::renderAddressIndent(rd_address address)
{
    if(this->hasFlag(RendererFlags_NoAddress) || this->hasFlag(RendererFlags_NoIndent)) return;

    size_t c = this->assembler()->bits() / 4;
    const auto& doc = this->context()->document();

    RDSegment s;
    if(doc->segment(address, &s)) c += std::strlen(s.name);
    this->chunk(std::string(c, ' '));
}

void Renderer::renderComments(rd_address address)
{
    if(this->hasFlag(RendererFlags_NoComments)) return;

    std::string comment = this->document()->comment(address, false, COMMENT_SEPARATOR);
    if(comment.empty()) return;

    this->chunk(std::string(INDENT_COMMENT, ' '));
    this->chunk("# " + Utils::simplified(comment), Theme_Comment);
}

void Renderer::renderSymbolValue(const RDSymbol* symbol)
{
    RDSegment segment;

    if(this->document()->segment(symbol->address, &segment) && HAS_FLAG(&segment, SegmentFlags_Bss))
    {
        if(IS_TYPE(symbol, SymbolType_Label)) this->chunk("<").chunk("dynamic branch", Theme_Symbol).chunk(">");
        else this->chunk(UNKNOWN_STRING, Theme_Data);
        return;
    }

    if(HAS_FLAG(symbol, SymbolFlags_Pointer) && this->renderSymbolPointer(symbol)) return;

    switch(symbol->type)
    {
        case SymbolType_Import: this->chunk("<").chunk("import", Theme_Symbol).chunk(">"); return;

        case SymbolType_String:
             if(HAS_FLAG(symbol, SymbolFlags_WideString)) this->chunk(Utils::quoted(Utils::simplified(this->document()->readWString(symbol->address, STRING_THRESHOLD))), Theme_String);
             else this->chunk(Utils::quoted(Utils::simplified(this->document()->readString(symbol->address, STRING_THRESHOLD))), Theme_String);
             return;

        default: break;
    }

    RDLocation loc = this->document()->dereference(symbol->address);

    if(loc.valid)
    {
        const char* symbolname = this->document()->name(loc.address);

        if(symbolname)
        {
            this->chunk(symbolname, Theme_Symbol);
            return;
        }
    }

    this->renderBlock(symbol->address);
}

bool Renderer::renderSymbolPointer(const RDSymbol* symbol)
{
    auto loc = this->document()->dereference(symbol->address);
    if(!loc.valid) return false;

    RDSymbol ptrsymbol;
    if(!this->document()->symbol(loc.address, &ptrsymbol)) return false;

    this->chunk(this->document()->name(ptrsymbol.address), Theme_Symbol);
    return true;
}

void Renderer::compileParams(rd_address address, RDRendererParams* srp)
{
    *srp = { address,
             { },
             CPTR(const RDContext, this->context()),
             CPTR(RDRenderer, this) };

    this->context()->document()->view(address, RD_NVAL, &srp->view);
}

bool Renderer::hasFlag(rd_flag f) const { return m_flags & f; }
Disassembler* Renderer::disassembler() const { return this->context()->disassembler(); }
SafeDocument& Renderer::document() const { return this->context()->document(); }
Assembler* Renderer::assembler() const { return this->context()->assembler(); }

Renderer& Renderer::chunk(const std::string& s, u8 fg, u8 bg)
{
    if(fg == Theme_Default) fg = m_currentfg;
    if(bg == Theme_Default) bg = m_currentbg;

    m_text += s;
    m_tokens.push_back({ bg, fg, s });
    return *this;
}

std::string Renderer::getInstruction(Context* ctx, rd_address address)
{
    if(ctx->flags() & ContextFlags_ShowRDIL) return Renderer::getRDILInstruction(ctx, address);
    return Renderer::getAssemblerInstruction(ctx, address);
}

std::string Renderer::getAssemblerInstruction(Context* ctx, rd_address address)
{
    Renderer r(ctx, RendererFlags_Simplified);
    r.renderAssemblerInstruction(address);
    return r.text();
}

std::string Renderer::getRDILInstruction(Context* ctx, rd_address address)
{
    Renderer r(ctx, RendererFlags_Simplified);
    r.renderRDILInstruction(address);
    return r.text();
}
