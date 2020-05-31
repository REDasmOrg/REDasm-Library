#include "renderer.h"
#include "../document/backend/blockcontainer.h"
#include "../support/sugar.h"
#include "../disassembler.h"
#include <rdcore/support/utils.h>
#include <rdcore/document/document.h>
#include <rdcore/document/cursor.h>
#include <rdcore/context.h>
#include <algorithm>
#include <cstring>
#include <climits>
#include <cmath>
#include <regex>
#include <list>

#define INDENT_WIDTH         2
#define INDENT_COMMENT      10
#define STRING_THRESHOLD    48
#define HEADER_SYMBOL_COUNT 10
#define SEPARATOR_LENGTH    50
#define WORD_REGEX          R"(\b[\w\d\.\$_]+\b)"
#define COMMENT_SEPARATOR   " | "

std::array<Callback_AssemblerRender, DocumentItemType_Length> Renderer::m_slots{ };

Renderer::Renderer(Disassembler* disassembler, const Cursor* cursor, flag_t flags): m_disassembler(disassembler), m_cursor(cursor), m_flags(flags)
{
    m_slots[RendererItemType_Segment]     = &Renderer::renderSegment;
    m_slots[RendererItemType_Function]    = &Renderer::renderFunction;
    m_slots[RendererItemType_Instruction] = &Renderer::renderInstruction;
    m_slots[RendererItemType_Symbol]      = &Renderer::renderSymbol;
    m_slots[RendererItemType_Operand]     = &Renderer::renderOperand;
}

void Renderer::render(size_t index, size_t count, Callback_Render cbrender, void* userdata) const
{
    if(!cbrender) return;

    const auto& doc = m_disassembler->document();

    for(size_t docindex = index, i = 0; (docindex < doc->itemsCount()) && (i < count); docindex++, i++)
    {
        RendererItem ritem;
        if(!this->renderItem(docindex, CPTR(RDRendererItem, &ritem))) continue;

        if(m_cursor && m_cursor->isLineSelected(docindex)) this->highlightSelection(&ritem);
        else if(m_cursor) this->highlightWords(&ritem);

        if(m_cursor && (m_cursor->currentLine() == docindex)) this->highlightCursor(&ritem);
        cbrender(CPTR(RDRendererItem, &ritem), i, userdata);
    }
}

void Renderer::renderAddress(RDRenderItemParams* rip)
{
    const Renderer* renderer = CPTR(const Renderer, rip->renderer);
    RendererItem* ri = CPTR(RendererItem, rip->rendereritem);
    renderer->renderAddress(rip->documentitem, ri);
}

void Renderer::renderAddressIndent(RDRenderItemParams* rip)
{
    const Renderer* renderer = CPTR(const Renderer, rip->renderer);
    RendererItem* ri = CPTR(RendererItem, rip->rendereritem);
    renderer->renderAddressIndent(rip->documentitem, ri);
}

void Renderer::renderIndent(RDRenderItemParams* rip, size_t n, bool ignoreflags)
{
    const Renderer* renderer = CPTR(const Renderer, rip->renderer);
    RendererItem* ri = CPTR(RendererItem, rip->rendereritem);
    renderer->renderIndent(ri, n, ignoreflags);
}

bool Renderer::renderConstant(RDRenderItemParams* rip)
{
    RendererItem* ri = CPTR(RendererItem, rip->rendereritem);
    ri->push(Utils::hex(rip->operand->u_value), "immediate_fg");
    return true;
}

bool Renderer::renderImmediate(RDRenderItemParams* rip)
{
    const Disassembler* d = CPTR(const Disassembler, rip->disassembler);
    RendererItem* ri = CPTR(RendererItem, rip->rendereritem);

    std::string value;
    const char* name = d->document()->name(rip->operand->address);

    if(name) value = name;
    else value = Utils::hex(rip->operand->u_value);

    if(IS_TYPE(rip->operand, OperandType_Memory)) ri->push("[").push(value, "memory_fg").push("]");
    else ri->push(value, "immediate_fg");
    return true;
}

bool Renderer::renderMemory(RDRenderItemParams* rip) { return Renderer::renderImmediate(rip); }

bool Renderer::renderDisplacement(const RDAssemblerPlugin* plugin, RDRenderItemParams* rip)
{
    if(!rip->instruction) return false;
    RendererItem* ri = CPTR(RendererItem, rip->rendereritem);
    size_t prevsize = ri->size();
    ri->push("[");

    if(Sugar::isBaseValid(rip->operand)) Renderer::renderRegister(rip, rip->operand->base);

    if(Sugar::isIndexValid(rip->operand))
    {
        if(prevsize != ri->size()) ri->push("+");
        prevsize++;

        Renderer::renderRegister(rip, rip->operand->index);
        if(rip->operand->scale > 1) ri->push("*").push(Utils::hex(rip->operand->scale), "immediate_fg");
    }

    if(rip->operand->displacement > 0)
    {
        if(prevsize != ri->size()) ri->push("+");
        Renderer::renderSymbol(plugin, rip, rip->operand->address);
    }
    else if(rip->operand->displacement < 0)
        ri->push("-").push(Utils::hex(std::abs(rip->operand->displacement)), "immediate_fg");

    ri->push("]");
    return true;
}

bool Renderer::renderRegister(RDRenderItemParams* rip)
{
    if(!rip->instruction) return false;
    Renderer::renderRegister(rip, rip->operand->reg);
    return true;
}

bool Renderer::renderMnemonic(RDRenderItemParams* rip)
{
    if(!rip->instruction || !std::strlen(rip->instruction->mnemonic)) return false;
    RendererItem* ri = CPTR(RendererItem, rip->rendereritem);

    switch(rip->instruction->type)
    {
        case InstructionType_Invalid: ri->push(rip->instruction->mnemonic, "instruction_invalid"); break;
        case InstructionType_Ret:     ri->push(rip->instruction->mnemonic, "instruction_ret");    break;
        case InstructionType_Nop:     ri->push(rip->instruction->mnemonic, "instruction_nop");     break;
        case InstructionType_Call:    ri->push(rip->instruction->mnemonic, "instruction_call");    break;
        case InstructionType_Compare: ri->push(rip->instruction->mnemonic, "instruction_compare"); break;

        case InstructionType_Jump:
            ri->push(rip->instruction->mnemonic, HAS_FLAG(rip->instruction, InstructionFlags_Conditional) ? "instruction_jmp_c" : "instruction_jmp");
            break;

        default: ri->push(rip->instruction->mnemonic); break;
    }

    if(rip->instruction->operandscount) ri->push(" ");
    return true;
}

bool Renderer::renderItem(size_t index, RDRendererItem* ritem) const
{
    reinterpret_cast<RendererItem*>(ritem)->setDocumentIndex(index);

    RDDocumentItem item;
    if(!m_disassembler->document()->itemAt(index, &item) || (item.type >= m_slots.size())) return false;

    type_t slottype;
    RDSymbol symbol{ };
    RDInstruction* instruction = nullptr;

    switch(item.type)
    {
        case DocumentItemType_Segment:  slottype = RendererItemType_Segment;  break;
        case DocumentItemType_Function: slottype = RendererItemType_Function; break;

        case DocumentItemType_Instruction:
            slottype = RendererItemType_Instruction;
            m_disassembler->document()->lockInstruction(item.address, &instruction);
            break;

        case DocumentItemType_Symbol:
            slottype = RendererItemType_Symbol;
            m_disassembler->document()->symbol(item.address, &symbol);
            break;

        case DocumentItemType_Unexplored: return this->renderUnexplored(&item, ritem);
        case DocumentItemType_Separator:  return this->renderSeparator(&item, ritem);
        case DocumentItemType_Empty:      return true;
        default:                          return false;
    }

    RDRenderItemParams rip = { slottype,
                               CPTR(const RDRenderer, this), CPTR(RDDisassembler, m_disassembler),
                               &item, ritem, &symbol, instruction, nullptr };

    bool res = this->renderParams(&rip);

    // Post rendering code
    switch(item.type)
    {
        case DocumentItemType_Instruction: Renderer::renderComments(&rip); break;
        default: break;
    }

    if(instruction) m_disassembler->document()->unlockInstruction(instruction);
    return res;
}

bool Renderer::selectedSymbol(RDSymbol* symbol) const
{
    const std::string& word = this->getCurrentWord();
    if(word.empty()) return false;

    return m_disassembler->document()->symbol(word.c_str(), symbol);
}

const std::string& Renderer::getInstruction(address_t address) const
{
    m_instructionstr.clear();
    size_t idx = m_disassembler->document()->instructionIndex(address);
    if(idx == RD_NPOS) return m_instructionstr;

    RDDocumentItem item;
    if(!m_disassembler->document()->itemAt(idx, &item)) return m_instructionstr;

    RDInstruction* instruction = nullptr;
    if(!m_disassembler->document()->lockInstruction(address, &instruction)) return m_instructionstr;

    RendererItem ritem;

    RDRenderItemParams rip = { RendererItemType_Instruction,
                               CPTR(const RDRenderer, this), CPTR(RDDisassembler, m_disassembler),
                               &item, CPTR(RDRendererItem, &ritem), nullptr, instruction, nullptr };

    if(this->renderParams(&rip))
        m_instructionstr = ritem.text();

    m_disassembler->document()->unlockInstruction(instruction);
    return m_instructionstr;
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
            this->renderItem(line, CPTR(RDRendererItem, &ritem));

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
        this->renderItem(startpos->line, CPTR(RDRendererItem, &ritem));
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
    if(!this->renderItem(index, CPTR(RDRendererItem, &ritem))) return 0;
    if(ritem.text().empty()) return 0;
    return ritem.text().size() - 1;
}

flag_t Renderer::flags() const { return m_flags; }

const std::string& Renderer::getWordFromPosition(const RDCursorPos* pos, RDCursorRange* range) const
{
    m_lastword.clear();
    if(!m_cursor || !pos) return m_lastword;

    RendererItem ritem{ };
    if(!this->renderItem(pos->line, CPTR(RDRendererItem, &ritem))) return m_lastword;

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
        ritem->format(start, end, "selection_fg", "selection_bg");
    }
    else
        ritem->format(startsel->column, endsel->column, "selection_fg", "selection_bg");
}

void Renderer::highlightCursor(RendererItem* ritem) const
{
    if((m_flags & RendererFlags_NoCursor) ||  !m_cursor || !m_cursor->active()) return;
    ritem->format(m_cursor->currentColumn(), m_cursor->currentColumn(), "cursor_fg", "cursor_bg");
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
        ritem->format(loc, loc + word.size() - 1, "highlight_fg", "highlight_bg");
}

void Renderer::renderPrologue(RDRenderItemParams* rip)
{
    const Renderer* r = CPTR(const Renderer, rip->renderer);

    if(!(r->m_flags & RendererFlags_NoSegmentAndAddress))
    {
        Renderer::renderAddress(rip);
        Renderer::renderIndent(rip, 3);
    }
    else
        Renderer::renderIndent(rip, 1);
}

void Renderer::renderSymbolPrologue(RDRenderItemParams* rip)
{
    const Disassembler* d = CPTR(const Disassembler, rip->disassembler);
    RendererItem* ri = CPTR(RendererItem, rip->rendereritem);

    Renderer::renderPrologue(rip);
    ri->push(d->document()->name(rip->documentitem->address), "label_fg");
    Renderer::renderIndent(rip, 1);
}

bool Renderer::renderSymbolPointer(RDRenderItemParams* rip)
{
    const Disassembler* d = CPTR(const Disassembler, rip->disassembler);
    RendererItem* ri = CPTR(RendererItem, rip->rendereritem);
    u64 value = 0;

    if(!d->readAddress(rip->documentitem->address, d->addressWidth(), &value)) return false;

    RDSymbol ptrsymbol;
    if(!d->document()->symbol(value, &ptrsymbol)) return false;

    ri->push(d->document()->name(ptrsymbol.address), "label_fg");
    return true;
}

bool Renderer::renderSegment(const RDAssemblerPlugin*, RDRenderItemParams* rip)
{
    static std::string eq = std::string(HEADER_SYMBOL_COUNT * 2, '=');
    const Disassembler* d = CPTR(const Disassembler, rip->disassembler);
    RendererItem* ri = CPTR(RendererItem, rip->rendereritem);

    std::string seg = "SEGMENT ";
    RDSegment segment;

    if(d->document()->segment(rip->documentitem->address, &segment))
    {
        seg += Utils::quoted(segment.name);
        seg += " START: " + Utils::hex(segment.address, d->assembler()->bits);
        seg += " END: " + Utils::hex(segment.endaddress, d->assembler()->bits);
    }
    else
    {
        seg += "???";
        seg += " START: " + Utils::hex(rip->documentitem->address, d->assembler()->bits);
        seg += " END: ???" ;
    }

    ri->push(eq + " " + seg + " " + eq, "segment_fg");
    return true;
}

bool Renderer::renderFunction(const RDAssemblerPlugin*, RDRenderItemParams* rip)
{
    static std::string eq = std::string(HEADER_SYMBOL_COUNT, '=');

    const Renderer* r = CPTR(const Renderer, rip->renderer);
    const Disassembler* d = CPTR(const Disassembler, rip->disassembler);
    RendererItem* ri = CPTR(RendererItem, rip->rendereritem);
    const char* name = d->document()->name(rip->documentitem->address);

    if(!(r->m_flags & RendererFlags_NoSegmentAndAddress))
        Renderer::renderAddressIndent(rip);

    if(name) ri->push(eq + " FUNCTION " + name + " " + eq, "function_fg");
    else ri->push(eq + " FUNCTION ??? " + eq, "function_fg");
    return true;
}

bool Renderer::renderInstruction(const RDAssemblerPlugin*, RDRenderItemParams* rip)
{
    const Renderer* r = CPTR(const Renderer, rip->renderer);
    RendererItem* ri = CPTR(RendererItem, rip->rendereritem);
    Renderer::renderPrologue(rip);

    if(rip->instruction)
    {
        Renderer::renderMnemonic(rip);
        if(!rip->instruction->operandscount) return true;
        rip->type = RendererItemType_Operand;

        for(size_t i = 0; i < rip->instruction->operandscount; i++)
        {
            rip->operand = &rip->instruction->operands[i];

            if(i) ri->push(", ");
            r->renderParams(rip);
        }
    }
    else
        ri->push("???", "instruction_invalid");

    return true;
}

bool Renderer::renderOperand(const RDAssemblerPlugin* plugin, RDRenderItemParams* rip)
{
    if(!rip->operand) return false;

    switch(rip->operand->type)
    {
        case OperandType_Constant:     return Renderer::renderConstant(rip);
        case OperandType_Immediate:    return Renderer::renderImmediate(rip);
        case OperandType_Memory:       return Renderer::renderMemory(rip);
        case OperandType_Displacement: return Renderer::renderDisplacement(plugin, rip);
        case OperandType_Register:     return Renderer::renderRegister(rip);
        default: return false;
    }
}

bool Renderer::renderSymbol(const RDAssemblerPlugin* , RDRenderItemParams* rip)
{
    const Disassembler* d = CPTR(const Disassembler, rip->disassembler);
    RendererItem* ri = CPTR(RendererItem, rip->rendereritem);

    RDSymbol symbol;

    if(!d->document()->symbol(rip->documentitem->address, &symbol))
    {
        Renderer::renderPrologue(rip);
        ri->push("Unknown Symbol");
        return true;
    }

    RDSegment segment;

    if(d->document()->segment(symbol.address, &segment) && HAS_FLAG(&segment, SegmentFlags_Bss))
    {
        Renderer::renderSymbolPrologue(rip);

        if(IS_TYPE(&symbol, SymbolType_Label))
        {
            ri->push(d->document()->name(rip->documentitem->address), "label_fg");
            ri->push(" <").push("dynamic branch", "label_fg").push(">");
        }
        else
            ri->push("???", "data_fg");

        return true;
    }

    bool prologuedone = false;

    if(HAS_FLAG(&symbol, SymbolFlags_Pointer))
    {
        Renderer::renderSymbolPrologue(rip);
        prologuedone = true;
        if(Renderer::renderSymbolPointer(rip)) return true;
    }

    switch(symbol.type)
    {
        case SymbolType_Import:
            if(!prologuedone) Renderer::renderSymbolPrologue(rip);
            ri->push("<").push("import", "import_fg").push(">");
            return true;

        case SymbolType_String:
            if(!prologuedone) Renderer::renderSymbolPrologue(rip);
            if(HAS_FLAG(&symbol, SymbolFlags_WideString)) ri->push(Utils::quoted(Utils::simplified(d->readWString(rip->documentitem->address, STRING_THRESHOLD))), "string_fg");
            else ri->push(Utils::quoted(Utils::simplified(d->readString(rip->documentitem->address, STRING_THRESHOLD))), "string_fg");
            return true;

        case SymbolType_Label:
            Renderer::renderAddressIndent(rip);
            ri->push(d->document()->name(rip->documentitem->address), "label_fg").push(":");
            return true;

        default: break;
    }

    if(!prologuedone) Renderer::renderSymbolPrologue(rip);

    RDLocation loc = d->dereference(rip->documentitem->address);

    if(loc.valid)
    {
        const char* symbolname = d->document()->name(loc.address);

        if(symbolname)
        {
            ri->push(symbolname, "label_fg");
            return true;
        }
    }

    ri->push(Utils::hex(loc.address, d->bits()), d->document()->segment(loc.address, nullptr) ? "pointer_fg" : "data_fg");
    return true;
}

void Renderer::renderAddress(const RDDocumentItem* item, RendererItem* ritem) const
{
    if(m_flags & RendererFlags_NoAddress) return;

    RDSegment s;
    auto& doc = m_disassembler->document();

    if(doc->segment(item->address, &s)) ritem->push(s.name, "address_fg");
    else ritem->push("???", "address_fg");

    ritem->push(":", "address_fg");
    ritem->push(Utils::hex(item->address, m_disassembler->bits()), "address_fg");
}

bool Renderer::renderParams(RDRenderItemParams* rip) const
{
    if(rip->type >= m_slots.size()) return false;

    RDAssemblerPlugin* aplugin = m_disassembler->assembler();
    if(aplugin->render && aplugin->render(aplugin, rip)) return true;

    auto slot = m_slots[rip->type];
    if(!slot) return false;
    return slot(aplugin, rip);
}

void Renderer::renderComments(RDRenderItemParams* rip)
{
    const Renderer* r = CPTR(const Renderer, rip->renderer);
    if(r->flags() & RendererFlags_NoComments) return;

    // Recalculate comment column
    RendererItem* ri = CPTR(RendererItem, rip->rendereritem);
    r->m_commentcolumn = std::max<size_t>(r->m_commentcolumn, ri->text().size());

    const Disassembler* d = CPTR(const Disassembler, rip->disassembler);
    std::string comment = d->document()->comment(rip->documentitem->address, false, COMMENT_SEPARATOR);
    if(comment.empty()) return;

    ri->push(std::string((r->m_commentcolumn - ri->text().size()) + INDENT_COMMENT, ' '));
    ri->push("# " + Utils::simplified(comment), "comment_fg");
}

bool Renderer::renderSeparator(const RDDocumentItem* item, RDRendererItem* ritem) const
{
    if(m_flags & RendererFlags_NoSeparators) return true;

    RendererItem* ri = CPTR(RendererItem, ritem);
    if(!(m_flags & RendererFlags_NoSegmentAndAddress)) this->renderAddressIndent(item, ri);

    ri->push(std::string(SEPARATOR_LENGTH, '-'), "separator");
    return true;
}

bool Renderer::renderUnexplored(const RDDocumentItem* item, RDRendererItem* ritem) const
{
    RendererItem* ri = CPTR(RendererItem, ritem);
    this->renderAddress(item, ri);
    this->renderIndent(ri, 3);

    RDBlock block;
    m_disassembler->document()->block(item->address, &block);

    size_t size = BlockContainer::size(&block);

    switch(size)
    {
        case 2:  ri->push("dw "); break;
        case 4:  ri->push("dw "); break;
        case 8:  ri->push("dq "); break;
        default: ri->push("db "); break;
    }

    if(size > 8)
        ri->push(Utils::hex(size), "immediate_fg");
    else if(!(size & 2) || (size == 1))
    {
        u64 value = 0;
        bool ok = m_disassembler->readAddress(block.start, size, &value);
        ri->push(ok ? Utils::hex(value, size * CHAR_BIT) : "??", "immediate_fg");
    }
    else
    {
        address_t address = block.start;
        ri->push("[");

        for(size_t i = 0; i < size; i++, address++)
        {
            u64 value = 0;
            bool ok = m_disassembler->readAddress(address, 1, &value);

            if(i) ri->push(", ");
            ri->push(ok ? Utils::hex(value, 8) : "??", "immediate_fg");
        }

        ri->push("]");
    }

    return true;
}

void Renderer::renderAddressIndent(const RDDocumentItem* item, RendererItem* ritem) const
{
    if((m_flags & RendererFlags_NoAddress) || (m_flags & RendererFlags_NoIndent)) return;

    size_t c = m_disassembler->bits() / 4;
    const auto& doc = m_disassembler->document();

    RDSegment s;
    if(doc->segment(item->address, &s)) c += std::strlen(s.name);
    ritem->push(std::string(c + INDENT_WIDTH, ' '));
}

void Renderer::renderIndent(RendererItem* ritem, size_t n, bool ignoreflags) const
{
    if(!ignoreflags && (m_flags & RendererFlags_NoIndent)) return;
    ritem->push(std::string(n * INDENT_WIDTH, ' '));
}

void Renderer::renderSymbol(const RDAssemblerPlugin* plugin, RDRenderItemParams* rip, address_t address)
{
    const Disassembler* d = CPTR(const Disassembler, rip->disassembler);
    RendererItem* ri = CPTR(RendererItem, rip->rendereritem);
    const char* name = d->document()->name(address);
    RDSymbol symbol;

    if(!name || !d->document()->symbol(address, &symbol))
    {
        ri->push(Utils::hex(address, plugin->bits), "immediate_fg");
        return;
    }

    if(HAS_FLAG(&symbol, SymbolFlags_Pointer))
    {
        ri->push(name, "pointer_fg");
        return;
    }

    switch(symbol.type)
    {
        case SymbolType_Data:     ri->push(name, "data_fg"); break;
        case SymbolType_String:   ri->push(name, "string_fg"); break;
        case SymbolType_Label:    ri->push(name, "label_fg"); break;
        case SymbolType_Function: ri->push(name, "function_fg"); break;
        case SymbolType_Import:   ri->push(name, "import_fg"); break;
        default: ri->push(name); break;
    }
}

void Renderer::renderRegister(RDRenderItemParams* rip, register_t r)
{
    const Disassembler* d = CPTR(const Disassembler, rip->disassembler);
    RendererItem* ri = CPTR(RendererItem, rip->rendereritem);
    ri->push(d->registerName(rip->instruction, r), "register_fg");
}
