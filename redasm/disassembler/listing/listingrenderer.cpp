#include "listingrenderer.h"
#include "../../plugins/assembler/assembler.h"
#include "../../plugins/loader.h"
#include <regex>

#define INDENT_WIDTH         2
#define STRING_THRESHOLD     48
#define HEX_ADDRESS(address) REDasm::hex(address, m_disassembler->assembler()->bits())

namespace REDasm {

ListingRenderer::ListingRenderer(DisassemblerAPI *disassembler): m_document(disassembler->document()), m_disassembler(disassembler), m_flags(ListingRenderer::Normal)
{
    m_cursor = m_document->cursor();
    m_printer = PrinterPtr(disassembler->assembler()->createPrinter(disassembler));
}

void ListingRenderer::render(size_t start, size_t count, void *userdata)
{
    auto lock = s_lock_safe_ptr(m_document);
    const ListingCursor* cur = lock->cursor();
    size_t end = start + count, line = start;
    std::string word = this->getCurrentWord();

    for(size_t i = 0; line < std::min(lock->size(), end); i++, line++)
    {
        RendererLine rl;
        rl.userdata = userdata;
        rl.documentindex = line;
        rl.index = i;
        rl.highlighted = cur->currentLine() == line;

        this->getRendererLine(lock, line, rl);

        if(m_cursor->isLineSelected(line))
            this->highlightSelection(rl);
        else
            this->highlightWord(rl, word);

        if(rl.highlighted)
            this->blinkCursor(rl);

        this->renderLine(rl);
    }
}

DisassemblerAPI *ListingRenderer::disassembler() const { return m_disassembler; }
const ListingDocument &ListingRenderer::document() const { return m_document; }

const Symbol *ListingRenderer::symbolUnderCursor()
{
    auto lock = REDasm::s_lock_safe_ptr(m_document);
    return lock->symbol(this->getCurrentWord());
}

ListingDocument &ListingRenderer::document() { return m_document; }

std::string ListingRenderer::wordFromPosition(const ListingCursor::Position &pos, ListingRenderer::Range *wordpos)
{
    RendererLine rl;
    this->getRendererLine(pos.first, rl);

    for(const RendererFormat& rf : rl.formats)
    {
        if(!rf.contains(pos.second))
            continue;

        std::string word = rl.formatText(rf);

        if(m_document->symbol(word))
        {
            if(wordpos)
                *wordpos = std::make_pair(rf.start, rf.end);

            return word;
        }
    }

    // Fallback to word matching
    std::regex rgxword(REDASM_WORD_REGEX);
    auto it = std::sregex_token_iterator(rl.text.begin(), rl.text.end(), rgxword);
    auto end = std::sregex_token_iterator();

    for(; it != end; it++)
    {
        u64 start = static_cast<u64>(it->first - rl.text.begin());
        u64 end = static_cast<u64>(it->second - rl.text.begin() - 1);

        if((pos.second < start) || (pos.second > end))
            continue;

        if(wordpos)
            *wordpos = std::make_pair(start, end);

        return *it;
    }

    if(wordpos)
        *wordpos = std::make_pair(1, 0);

    return std::string();
}

std::string ListingRenderer::getCurrentWord() { return this->wordFromPosition(m_cursor->currentPosition()); }

size_t ListingRenderer::getLastColumn(size_t line)
{
    RendererLine rl;
    this->getRendererLine(line, rl);
    size_t len = rl.length();

    if(!len)
        return 0;

    return len - 1;
}

std::string ListingRenderer::getLine(size_t line)
{
    RendererLine rl;
    this->getRendererLine(line, rl);
    return rl.text;
}

std::string ListingRenderer::getSelectedText()
{
    auto lock = s_lock_safe_ptr(m_document);
    const ListingCursor* cur = lock->cursor();

    if(!cur->hasSelection())
        return std::string();

    const ListingCursor::Position& startpos = cur->startSelection();
    const ListingCursor::Position& endpos = cur->endSelection();

    std::string copied;

    if(startpos.first != endpos.first)
    {
        u64 line = startpos.first;

        while(line <= endpos.first)
        {
            RendererLine rl;
            this->getRendererLine(lock, line, rl);
            std::string s = rl.text;

            if(line == startpos.first)
                copied += s.substr(startpos.second);
            else if(line == endpos.first)
                copied += s.substr(0, endpos.second + 1);
            else
                copied += s;

            copied += "\n";
            line++;
        }
    }
    else
    {
        RendererLine rl;
        this->getRendererLine(lock, startpos.first, rl);
        copied = rl.text.substr(startpos.second, endpos.second - startpos.second + 1);
    }

    return copied;
}

bool ListingRenderer::hasFlag(u32 flag) const { return flag & m_flags; }
void ListingRenderer::setFlags(u32 flags) { m_flags = flags; }

bool ListingRenderer::getRendererLine(size_t line, RendererLine &rl)
{
    auto lock = document_s_lock(m_document);
    return this->getRendererLine(lock, line, rl);
}

bool ListingRenderer::getRendererLine(const document_s_lock &lock, size_t line, RendererLine& rl)
{
    const ListingItem* item = lock->itemAt(std::min(line, lock->lastLine()));

    if(!item)
        return false;

    if(item->is(ListingItem::SegmentItem))
        this->renderSegment(lock, item, rl);
    else if(item->is(ListingItem::FunctionItem))
        this->renderFunction(lock, item, rl);
    else if(item->is(ListingItem::InstructionItem))
        this->renderInstruction(lock, item, rl);
    else if(item->is(ListingItem::SymbolItem))
        this->renderSymbol(lock, item, rl);
    else if(item->is(ListingItem::MetaItem))
        this->renderMeta(lock, item, rl);
    else if(item->is(ListingItem::TypeItem))
        this->renderType(lock, item, rl);
    else if(item->is(ListingItem::EmptyItem))
        rl.push(" ");
    else
        rl.push("Unknown Type: " + std::to_string(item->type));

    return true;
}

void ListingRenderer::highlightSelection(RendererLine &rl)
{
    if(rl.text.empty())
        return;

    const REDasm::ListingCursor::Position& startsel = m_cursor->startSelection();
    const REDasm::ListingCursor::Position& endsel = m_cursor->endSelection();

    if(startsel.first != endsel.first)
    {
        size_t start = (rl.documentindex == startsel.first) ? startsel.second : 0;
        size_t end = (rl.documentindex == endsel.first) ? endsel.second : (rl.text.length() - 1);
        rl.format(start, end, "selection_fg", "selection_bg");
    }
    else
        rl.format(startsel.second, endsel.second, "selection_fg", "selection_bg");
}

void ListingRenderer::blinkCursor(RendererLine &rl)
{
    if(!m_cursor->active())
        return;

    rl.format(m_cursor->currentColumn(), m_cursor->currentColumn(), "cursor_fg", "cursorbg");
}

void ListingRenderer::highlightWord(RendererLine &rl, const std::string word)
{
    if(word.empty())
        return;

    size_t pos = rl.text.find(word, 0);
    std::list<size_t> locations;

    while(pos != std::string::npos)
    {
        locations.push_back(pos);
        pos = rl.text.find(word, pos + 1);
    }

    for(size_t loc : locations)
        rl.format(loc, loc + word.size() - 1, "highlight_fg", "highlight_bg");
}

void ListingRenderer::renderSegment(const document_s_lock& lock, const ListingItem *item, RendererLine &rl)
{
    m_printer->segment(lock->segment(item->address), [&](const std::string& line) {
        rl.push(line, "segment_fg");
    });
}

void ListingRenderer::renderFunction(const document_s_lock& lock, const ListingItem *item, RendererLine& rl)
{
    if(rl.ignoreflags || !this->hasFlag(ListingRenderer::HideSegmentAndAddress))
        this->renderAddressIndent(lock, item, rl);

    m_printer->function(lock->symbol(item->address), [&](const std::string& pre, const std::string& sym, const std::string& post) {
        if(!pre.empty())
            rl.push(pre, "function_fg");

        rl.push(sym, "function_fg");

        if(!post.empty())
            rl.push(post, "function_fg");
    });
}

void ListingRenderer::renderInstruction(const document_s_lock& lock, const ListingItem *item, RendererLine &rl)
{
    InstructionPtr instruction = lock->instruction(item->address);

    this->renderAddress(lock, item, rl);
    this->renderIndent(rl, 3);
    this->renderMnemonic(instruction, rl);
    this->renderOperands(instruction, rl);
    this->renderComments(lock, item, rl);
}

void ListingRenderer::renderSymbol(const document_s_lock& lock, const ListingItem *item, RendererLine &rl)
{
    LoaderPlugin* loader = m_disassembler->loader();
    AssemblerPlugin* assembler = m_disassembler->assembler();
    const Symbol* symbol = lock->symbol(item->address);

    if(symbol->is(SymbolType::Code)) // Label or Callback
    {
        const Segment* segment = lock->segment(symbol->address);

        if(segment->is(SegmentType::Bss))
        {
            this->renderAddress(lock, item, rl);
            this->renderIndent(rl);
            rl.push(symbol->name, "label_fg");
            rl.push(" <").push("dynamic branch", "label_fg").push(">");
        }
        else
        {
            if(!rl.ignoreflags && this->hasFlag(ListingRenderer::HideSegmentAndAddress))
                this->renderIndent(rl, 2);
            else
                this->renderAddressIndent(lock, item, rl);

            rl.push(symbol->name, "label_fg").push(":");
        }
    }
    else // Data
    {
        const Segment* segment = lock->segment(item->address);
        this->renderAddress(lock, item, rl);
        this->renderIndent(rl, 3);
        rl.push(symbol->name, "label_fg");
        this->renderIndent(rl);

        if(!segment->is(SegmentType::Bss) && loader->offset(symbol->address).valid)
        {
            if(symbol->is(SymbolType::Pointer))
            {
                if(this->renderSymbolPointer(lock, symbol, rl))
                    return;
            }

            if(symbol->is(SymbolType::WideStringMask))
                rl.push(REDasm::quoted(m_disassembler->readWString(symbol, STRING_THRESHOLD)), "string_fg");
            else if(symbol->is(SymbolType::StringMask))
                rl.push(REDasm::quoted(m_disassembler->readString(symbol, STRING_THRESHOLD)), "string_fg");
            else if(symbol->is(SymbolType::ImportMask))
                rl.push("<").push("import", "label_fg").push(">");
            else
            {
                u64 value = 0;
                m_disassembler->readAddress(symbol->address, assembler->addressWidth(), &value);
                rl.push(REDasm::hex(value, m_disassembler->assembler()->bits()), m_document->segment(value) ? "pointer_fg" : "data_fg");
            }
        }
        else if(symbol->is(SymbolType::ImportMask))
            rl.push("<").push("import", "label_fg").push(">");
        else
            rl.push("??", "data_fg");
    }
}

void ListingRenderer::renderMeta(const document_s_lock &lock, const ListingItem *item, RendererLine &rl)
{
    this->renderAddressIndent(lock, item, rl);
    auto metaitem = lock->meta(item);
    rl.push(metaitem.type + " ", "meta_fg").push(metaitem.name, "comment_fg");
}

void ListingRenderer::renderType(const document_s_lock &lock, const ListingItem *item, RendererLine &rl)
{
    this->renderAddressIndent(lock, item, rl);
    rl.push(".type ", "meta_fg").push(lock->type(item), "comment_fg");
}

void ListingRenderer::renderAddress(const document_s_lock &lock, const ListingItem *item, RendererLine &rl)
{
    if(!rl.ignoreflags && (this->hasFlag(ListingRenderer::HideSegmentName) && !this->hasFlag(ListingRenderer::HideAddress)))
        rl.push(HEX_ADDRESS(item->address), "address_fg");
    else if(rl.ignoreflags || !this->hasFlag(ListingRenderer::HideAddress))
    {
        const Segment* segment = lock->segment(item->address);
        rl.push((segment ? segment->name : "unk") + ":" + HEX_ADDRESS(item->address), "address_fg");
    }
}

void ListingRenderer::renderMnemonic(const InstructionPtr &instruction, RendererLine &rl)
{
    std::string mnemonic = instruction->mnemonic + " ";

    if(instruction->isInvalid())
        rl.push(mnemonic, "instruction_invalid");
    else if(instruction->is(REDasm::InstructionType::Stop))
        rl.push(mnemonic, "instruction_stop");
    else if(instruction->is(REDasm::InstructionType::Nop))
        rl.push(mnemonic, "instruction_nop");
    else if(instruction->is(REDasm::InstructionType::Call))
        rl.push(mnemonic, "instruction_call");
    else if(instruction->is(REDasm::InstructionType::Compare))
        rl.push(mnemonic, "instruction_compare");
    else if(instruction->is(REDasm::InstructionType::Jump))
    {
        if(instruction->is(REDasm::InstructionType::Conditional))
            rl.push(mnemonic, "instruction_jmp_c");
        else
            rl.push(mnemonic, "instruction_jmp");
    }
    else
        rl.push(mnemonic);
}

void ListingRenderer::renderOperands(const InstructionPtr &instruction, RendererLine &rl)
{
    m_printer->out(instruction, [&](const REDasm::Operand* op, const std::string& opsize, const std::string& opstr) {
        if(!op) {
            rl.push(opstr, "immediate_fg");
            return;
        }

        if(op->index > 0)
            rl.push(", ");

        if(!opsize.empty())
            rl.push(opsize + " ");

        if(op->isNumeric())
        {
            if(op->is(REDasm::OperandType::Memory))
                rl.push(opstr, "memory_fg");
            else
                rl.push(opstr, "immediate_fg");
        }
        else if(op->is(REDasm::OperandType::Displacement))
            rl.push(opstr, "displacement_fg");
        else if(op->is(REDasm::OperandType::Register))
            rl.push(opstr, "register_fg");
        else
            rl.push(opstr);
    });
}

void ListingRenderer::renderComments(const document_s_lock &lock, const ListingItem* item, RendererLine &rl)
{
    std::string s = lock->comment(item);

    if(s.empty())
        return;

    rl.push("   # " + ListingRenderer::escapeString(s), "comment_fg");
}

void ListingRenderer::renderAddressIndent(const document_s_lock& lock, const ListingItem* item, RendererLine &rl)
{
    const Segment* segment = lock->segment(item->address);

    s64 count = m_disassembler->assembler()->bits() / 4;

    if(segment)
        count += segment->name.length();

    rl.push(std::string(count + INDENT_WIDTH, ' '));
}

void ListingRenderer::renderIndent(RendererLine &rl, int n) { rl.push(std::string(n * INDENT_WIDTH, ' ')); }

bool ListingRenderer::renderSymbolPointer(const document_s_lock &lock, const Symbol* symbol, RendererLine &rl) const
{
    u64 value = 0;
    AssemblerPlugin* assembler = m_disassembler->assembler();

   if(!m_disassembler->readAddress(symbol->address, assembler->addressWidth(), &value))
       return false;

   const Symbol* ptrsymbol = lock->symbol(value);

   if(!ptrsymbol)
       return false;

   rl.push(ptrsymbol->name, ptrsymbol->isLocked() ? "locked_fg" : "label_fg");
   return true;
}

std::string ListingRenderer::escapeString(const std::string &s)
{
    std::string res;

    for(char ch : s)
    {
        switch(ch)
        {
            case '\n':
                res += "\\\n";
                break;

            case '\r':
                res += "\\\r";
                break;

            case '\t':
                res += "\\\t";
                break;

            default:
                res += ch;
                break;
        }
    }

    return res;
}

} // namespace REDasm
