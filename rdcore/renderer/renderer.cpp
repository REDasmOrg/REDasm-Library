#include "renderer.h"
#include "../document/backend/blockcontainer.h"
#include "../rdil/ilfunction.h"
#include "../rdil/rdil.h"
#include "../disassembler.h"
#include <rdcore/support/utils.h>
#include <rdcore/document/document.h>
#include <rdcore/document/cursor.h>
#include <rdcore/context.h>
#include <cmath>
#include <regex>

#define INDENT_WIDTH         2
#define INDENT_COMMENT      10
#define STRING_THRESHOLD    48
#define HEADER_SYMBOL_COUNT 10
#define SEPARATOR_LENGTH    50
#define WORD_REGEX          R"(\b[\w\d\.\$_]+\b)"
#define COMMENT_SEPARATOR   " | "

Renderer::Renderer(Disassembler* disassembler, const Cursor* cursor, rd_flag flags): m_disassembler(disassembler), m_cursor(cursor), m_flags(flags) { }

void Renderer::render(size_t index, size_t count, Callback_Render cbrender, void* userdata) const
{
    if(!cbrender) return;

    const auto& doc = m_disassembler->document();

    for(size_t docindex = index, i = 0; (docindex < doc->itemsCount()) && (i < count); docindex++, i++)
    {
        RendererItem ritem;
        if(!this->renderItem(docindex, &ritem)) continue;

        if(m_cursor && m_cursor->isLineSelected(docindex)) this->highlightSelection(&ritem);
        else if(m_cursor) this->highlightWords(&ritem);

        if(m_cursor && (m_cursor->currentLine() == docindex)) this->highlightCursor(&ritem);
        cbrender(CPTR(RDRendererItem, &ritem), i, userdata);
    }
}

void Renderer::renderSigned(RendererItem* ritem, s64 value) const
{
    if(value < 0) ritem->push("-");
    ritem->push(Utils::hex(value), Theme_Constant);
}

void Renderer::renderUnsigned(RendererItem* ritem, u64 value) const
{
    const char* name = m_disassembler->document()->name(static_cast<rd_address>(value));
    if(name) ritem->push(name, Theme_Symbol);
    else ritem->push(Utils::hex(value), Theme_Constant);
}

void Renderer::renderMnemonic(RendererItem* ritem, const std::string& s, rd_type theme) const { if(!s.empty()) ritem->push(s, theme); }
void Renderer::renderRegister(RendererItem* ritem, const std::string& s) const { ritem->push(s, Theme_Reg); }
void Renderer::renderConstant(RendererItem* ritem, const std::string& s) const { ritem->push(s, Theme_Constant); }
void Renderer::renderText(RendererItem* ritem, const std::string& s, rd_type theme) const { ritem->push(s, theme); }

void Renderer::renderIndent(RendererItem* ritem, size_t n, bool ignoreflags) const
{
    if(!ignoreflags && (m_flags & RendererFlags_NoIndent)) return;
    ritem->push(std::string(n * INDENT_WIDTH, ' '));
}

void Renderer::renderAssemblerInstruction(rd_address address, RendererItem* ritem) const
{
    RDRenderItemParams rip;
    this->compileParams(address, ritem, &rip);

    if(!m_disassembler->assembler()->renderInstruction(&rip))
        ritem->push("???");
}

void Renderer::renderRDILInstruction(rd_address address, RendererItem* ritem) const
{
    RDRenderItemParams rip;
    this->compileParams(address, ritem, &rip);

    ILFunction il(m_disassembler);
    m_disassembler->assembler()->lift(rip.address, &rip.view, &il);

    for(size_t i = 0; i < il.size(); i++)
    {
        if(i) ritem->push("; "); // Attach more statements, if needed
        RDIL::render(il.expression(i), this, ritem, address);
    }
}

bool Renderer::renderItem(size_t index, RendererItem* ritem) const
{
    ritem->setDocumentIndex(index);

    RDDocumentItem item;
    if(!m_disassembler->document()->itemAt(index, &item)) return false;

    switch(item.type)
    {
        case DocumentItemType_Segment:     this->renderSegment(item.address, ritem);     break;
        case DocumentItemType_Function:    this->renderFunction(item.address, ritem);    break;
        case DocumentItemType_Instruction: this->renderInstruction(item.address, ritem); break;
        case DocumentItemType_Symbol:      this->renderSymbol(item.address, ritem);      break;
        case DocumentItemType_Unexplored:  this->renderUnexplored(item.address, ritem);  break;
        case DocumentItemType_Separator:   this->renderSeparator(item.address, ritem);   break;
        case DocumentItemType_Empty:       return true;
        default:                           return false;
    }

    return true;
}

bool Renderer::selectedSymbol(RDSymbol* symbol) const
{
    const std::string& word = this->getCurrentWord();
    if(word.empty()) return false;

    return m_disassembler->document()->symbol(word.c_str(), symbol);
}

const std::string& Renderer::getInstructionText(rd_address address) const
{
    m_instructionstr.clear();
    size_t idx = m_disassembler->document()->instructionIndex(address);
    if(idx == RD_NPOS) return m_instructionstr;

    RendererItem ritem;
    if(this->renderItem(idx, &ritem)) m_instructionstr = ritem.text();

    return m_instructionstr;
}

const std::string& Renderer::getAssemblerInstruction(rd_address address) const
{
    m_asmstr.clear();

    RendererItem ritem;
    this->renderAssemblerInstruction(address, &ritem);
    m_asmstr = ritem.text();
    return m_asmstr;
}

const std::string& Renderer::getRDILInstruction(rd_address address) const
{
    m_rdilstr.clear();

    RendererItem ritem;
    this->renderRDILInstruction(address, &ritem);
    m_rdilstr = ritem.text();
    return m_rdilstr;
}

const std::string& Renderer::getSelectedText() const
{
    m_selectedtext.clear();
    if(!m_cursor->hasSelection()) return m_selectedtext;

    const RDCursorPos* startpos = m_cursor->startSelection();
    const RDCursorPos* endpos = m_cursor->endSelection();

    if(startpos->line != endpos->line)
    {
        size_t line = startpos->line;

        while(line <= endpos->line)
        {
            RendererItem ritem;
            this->renderItem(line, &ritem);

            if(line == startpos->line) m_selectedtext += ritem.text().substr(startpos->column);
            else if(line == endpos->line) m_selectedtext += ritem.text().substr(0, endpos->column + 1);
            else m_selectedtext += ritem.text();

            m_selectedtext += "\n";
            line++;
        }
    }
    else
    {
        RendererItem ritem;
        this->renderItem(startpos->line, &ritem);
        if(startpos->column >= ritem.text().size()) m_selectedtext.clear();
        else m_selectedtext = ritem.text().substr(startpos->column, endpos->column - startpos->column + 1);
    }

    return m_selectedtext;
}

const std::string& Renderer::getCurrentWord() const
{
    if(m_cursor->hasSelection()) return this->getSelectedText();
    return this->getWordFromPosition(m_cursor->position(), nullptr);
}

size_t Renderer::getLastColumn(size_t index) const
{
    RendererItem ritem;
    if(!this->renderItem(index, &ritem)) return 0;
    if(ritem.text().empty()) return 0;
    return ritem.text().size() - 1;
}

rd_flag Renderer::flags() const { return m_flags; }

const std::string& Renderer::getWordFromPosition(const RDCursorPos* pos, RDCursorRange* range) const
{
    m_lastword.clear();
    if(!m_cursor || !pos) return m_lastword;

    RendererItem ritem;
    if(!this->renderItem(pos->line, &ritem)) return m_lastword;

    if(!m_cursor->hasSelection())
    {
        for(size_t i = 0; i < ritem.size(); i++)
        {
            const RDRendererFormat& rf = ritem.format(i);
            if(!RendererItem::formatContains(&rf, pos->column)) continue;

            m_lastword = ritem.formatText(&rf);
            RDSymbol symbol;

            if(m_disassembler->document()->symbol(m_lastword.c_str(), &symbol))
            {
                if(range) *range = { rf.start, rf.end };
                return m_lastword;
            }
        }
    }

    // Word Matching
    m_lastword.clear();
    static std::regex rgx(WORD_REGEX);

    const std::string& s = ritem.text();
    auto wbegin = std::sregex_iterator(s.begin(), s.end(), rgx);
    auto wend = std::sregex_iterator();

    for(auto it = wbegin; it != wend; it++)
    {
        auto match = *it;
        s32 start = static_cast<s32>(match.position());
        s32 end = static_cast<s32>(match.position() + match.str().size());

        if((pos->column < start) || (pos->column > end))
            continue;

        if(range) *range = { start, end };
        m_lastword = match.str();
        return m_lastword;
    }

    if(range) *range = { 1, 0 };
    return m_lastword;
}

void Renderer::highlightSelection(RendererItem* ritem) const
{
    if((m_flags & RendererFlags_NoCursor) || ritem->text().empty()) return;

    const RDCursorPos* startsel = m_cursor->startSelection();
    const RDCursorPos* endsel = m_cursor->endSelection();

    if(startsel->line != endsel->line)
    {
        size_t start = (ritem->documentIndex() == startsel->line) ? startsel->column : 0;
        size_t end = (ritem->documentIndex() == endsel->line) ? endsel->column : (ritem->text().size() - 1);
        ritem->format(start, end, Theme_SelectionFg, Theme_SelectionBg);
    }
    else
        ritem->format(startsel->column, endsel->column, Theme_SelectionFg, Theme_SelectionBg);
}

void Renderer::highlightCursor(RendererItem* ritem) const
{
    if((m_flags & RendererFlags_NoCursor) ||  !m_cursor || !m_cursor->active()) return;
    ritem->format(m_cursor->currentColumn(), m_cursor->currentColumn(), Theme_CursorFg, Theme_CursorBg);
}

void Renderer::highlightWords(RendererItem* ritem) const
{
    if(m_flags & RendererFlags_NoHighlightWords) return;

    const std::string& word = this->getCurrentWord();
    if(word.empty()) return;

    size_t pos = ritem->text().find(word, 0);
    std::list<size_t> locations;

    while(pos != std::string::npos)
    {
        locations.push_back(pos);
        pos = ritem->text().find(word, pos + word.size());
    }

    for(size_t loc : locations)
        ritem->format(loc, loc + word.size() - 1, Theme_HighlightFg, Theme_HighlightBg);
}

void Renderer::renderHexDump(RendererItem* ritem, const RDBufferView* view, size_t size) const
{
    if(!view) return;

    ritem->push(Utils::hexString(view, size), Theme_Constant);
    this->renderIndent(ritem, 1);
}

bool Renderer::renderSymbolPointer(const RDSymbol* symbol, RendererItem* ritem) const
{
    auto loc = m_disassembler->dereference(symbol->address);
    if(!loc.valid) return false;

    RDSymbol ptrsymbol;
    if(!m_disassembler->document()->symbol(loc.address, &ptrsymbol)) return false;

    ritem->push(m_disassembler->document()->name(ptrsymbol.address), Theme_Symbol);
    return true;
}

void Renderer::renderSegment(rd_address address, RendererItem* ritem) const
{
    static std::string eq = std::string(HEADER_SYMBOL_COUNT * 2, '-');

    std::string seg = "SEGMENT ";
    RDSegment segment;

    if(m_disassembler->document()->segment(address, &segment))
    {
        seg += Utils::quoted(segment.name);
        seg += " START: " + Utils::hex(segment.address, m_disassembler->assembler()->bits());
        seg += " END: " + Utils::hex(segment.endaddress, m_disassembler->assembler()->bits());
    }
    else
    {
        seg += "???";
        seg += " START: " + Utils::hex(address, m_disassembler->assembler()->bits());
        seg += " END: ???" ;
    }

    ritem->push(eq + "< " + seg + " >" + eq, Theme_Segment);
}

void Renderer::renderFunction(rd_address address, RendererItem* ritem) const
{
    if(!(m_flags & RendererFlags_NoSegmentAndAddress)) this->renderAddressIndent(address, ritem);

    const char* name = m_disassembler->document()->name(address);
    if(name) ritem->push("function " + std::string(name) + "()", Theme_Function);
    else ritem->push("function \?\?\?()", Theme_Function);
}

void Renderer::renderInstruction(rd_address address, RendererItem* ritem) const
{
    this->renderPrologue(address, ritem);

    if(rd_ctx->flags() & ContextFlags_ShowRDIL) this->renderRDILInstruction(address, ritem);
    else this->renderAssemblerInstruction(address, ritem);

    this->renderComments(address, ritem);
}

void Renderer::renderSymbol(rd_address address, RendererItem* ritem) const
{
    this->renderPrologue(address, ritem);

    const char* name = m_disassembler->document()->name(address);
    RDSymbol symbol;

    if(!name || !m_disassembler->document()->symbol(address, &symbol))
    {
        ritem->push(Utils::hex(address, m_disassembler->assembler()->bits()), Theme_Constant);
        return;
    }

    if(HAS_FLAG(&symbol, SymbolFlags_Pointer))
        ritem->push(name, Theme_Pointer);
    else
    {
        switch(symbol.type)
        {
            case SymbolType_Data: ritem->push(name, Theme_Data); break;
            case SymbolType_String: ritem->push(name, Theme_String); break;
            case SymbolType_Import: ritem->push(name, Theme_Imported); break;
            case SymbolType_Label: ritem->push(name, Theme_Symbol).push(":"); return;
            case SymbolType_Function: ritem->push(name, Theme_Function); return;
            default: ritem->push(name); return;
        }
    }

    this->renderIndent(ritem, 1);
    this->renderSymbolValue(&symbol, ritem);
    this->renderComments(address, ritem);
}

void Renderer::renderSymbolValue(const RDSymbol* symbol, RendererItem* ritem) const
{
    RDSegment segment;

    if(m_disassembler->document()->segment(symbol->address, &segment) && HAS_FLAG(&segment, SegmentFlags_Bss))
    {
        if(IS_TYPE(symbol, SymbolType_Label)) ritem->push("<").push("dynamic branch", Theme_Symbol).push(">");
        else ritem->push("???", Theme_Data);
        return;
    }

    if(HAS_FLAG(symbol, SymbolFlags_Pointer) && this->renderSymbolPointer(symbol, ritem)) return;

    switch(symbol->type)
    {
        case SymbolType_Import: ritem->push("<").push("import", Theme_Symbol).push(">"); return;

        case SymbolType_String:
             if(HAS_FLAG(symbol, SymbolFlags_WideString)) ritem->push(Utils::quoted(Utils::simplified(m_disassembler->readWString(symbol->address, STRING_THRESHOLD))), Theme_String);
             else ritem->push(Utils::quoted(Utils::simplified(m_disassembler->readString(symbol->address, STRING_THRESHOLD))), Theme_String);
             return;

        default: break;
    }

    RDLocation loc = m_disassembler->dereference(symbol->address);

    if(loc.valid)
    {
        const char* symbolname = m_disassembler->document()->name(loc.address);

        if(symbolname)
        {
            ritem->push(symbolname, Theme_Symbol);
            return;
        }
    }

    this->renderBlock(symbol->address, ritem);
}

void Renderer::compileParams(rd_address address, RendererItem* ritem, RDRenderItemParams* rip) const
{
    *rip = { address,
             { },
             CPTR(const RDRenderer, this),
             CPTR(RDDisassembler, m_disassembler),
             CPTR(RDRendererItem, ritem) };

    m_disassembler->view(address, RD_NPOS, &rip->view);
}

void Renderer::renderComments(rd_address address, RendererItem* ritem) const
{
    if(m_flags & RendererFlags_NoComments) return;

    m_commentcolumn = std::max<size_t>(m_commentcolumn, ritem->text().size()); // Recalculate comment column

    std::string comment = m_disassembler->document()->comment(address, false, COMMENT_SEPARATOR);
    if(comment.empty()) return;

    ritem->push(std::string((m_commentcolumn - ritem->text().size()) + INDENT_COMMENT, ' '));
    ritem->push("# " + Utils::simplified(comment), Theme_Comment);
}

void Renderer::renderBlock(rd_address address, RendererItem* ritem) const
{
    RDBlock block;
    if(!m_disassembler->document()->block(address, &block)) REDasmError("Invalid Block", address);

    size_t blocksize = BlockContainer::size(&block);
    RDBufferView view;

    if((blocksize <= sizeof(rd_address)) && m_disassembler->view(block.address, blocksize, &view)) this->renderHexDump(ritem, &view, RD_NPOS);
    else ritem->push("(").push(Utils::hex(blocksize), Theme_Constant).push(")");
}

void Renderer::renderSeparator(rd_address address, RendererItem* ritem) const
{
    if(m_flags & RendererFlags_NoSeparators) return;
    if(!(m_flags & RendererFlags_NoSegmentAndAddress)) this->renderAddressIndent(address, ritem);

    ritem->push(std::string(SEPARATOR_LENGTH, '-'), Theme_Comment);
}

void Renderer::renderUnexplored(rd_address address, RendererItem* ritem) const
{
    this->renderPrologue(address, ritem);
    ritem->push("db ");
    this->renderBlock(address, ritem);
}

void Renderer::renderAddressIndent(rd_address address, RendererItem* ritem) const
{
    if((m_flags & RendererFlags_NoAddress) || (m_flags & RendererFlags_NoIndent)) return;

    size_t c = m_disassembler->assembler()->bits() / 4;
    const auto& doc = m_disassembler->document();

    RDSegment s;
    if(doc->segment(address, &s)) c += std::strlen(s.name);
    ritem->push(std::string(c + INDENT_WIDTH, ' '));
}

void Renderer::renderPrologue(rd_address address, RendererItem* ritem) const
{
    if(!(m_flags & RendererFlags_NoSegmentAndAddress))
    {
        if(!(m_flags & RendererFlags_NoSegment))
        {
            RDSegment s;
            auto& doc = m_disassembler->document();

            if(doc->segment(address, &s)) ritem->push(s.name, Theme_Address);
            else ritem->push("???", Theme_Address);
            ritem->push(":", Theme_Address);
        }

        if(!(m_flags & RendererFlags_NoAddress))
            ritem->push(Utils::hex(address, m_disassembler->assembler()->bits()), Theme_Address);
    }

    this->renderIndent(ritem, 1);
}
