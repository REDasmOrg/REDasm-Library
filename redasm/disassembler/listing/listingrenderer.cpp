#include "listingrenderer.h"
#include "../../plugins/assembler/assembler.h"
#include "../../plugins/format.h"

#define INDENT_WIDTH         2
#define INDENT_COMMENT       10
#define HEX_ADDRESS(address) REDasm::hex(address, m_disassembler->format()->bits())

namespace REDasm {

ListingRenderer::ListingRenderer(DisassemblerAPI *disassembler): m_document(disassembler->document()), m_disassembler(disassembler), m_flags(ListingRenderer::Normal)
{
    m_cursor = m_document->cursor();
    m_printer = PrinterPtr(disassembler->assembler()->createPrinter(disassembler));
}

void ListingRenderer::render(u64 start, u64 count, void *userdata)
{
    auto lock = s_lock_safe_ptr(m_document);
    const ListingCursor* cur = lock->cursor();
    u64 end = start + count, line = start;

    for(u64 i = 0; line < std::min(lock->size(), end); i++, line++)
    {
        RendererLine rl;
        rl.userdata = userdata;
        rl.documentindex = line;
        rl.index = i;
        rl.highlighted = cur->currentLine() == line;

        this->getRendererLine(lock, line, rl);
        this->renderLine(rl);
    }
}

u64 ListingRenderer::getLastColumn(u64 line)
{
    RendererLine rl;
    this->getRendererLine(line, rl);
    u64 len = static_cast<u64>(rl.length());

    if(!len)
        return 0;

    return len - 1;
}

std::string ListingRenderer::getLine(u64 line)
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

void ListingRenderer::setFlags(u32 flags) { m_flags = flags; }

bool ListingRenderer::getRendererLine(size_t line, RendererLine &rl)
{
    auto lock = document_lock(m_document);
    return this->getRendererLine(lock, line, rl);
}

bool ListingRenderer::getRendererLine(const document_lock &lock, size_t line, RendererLine& rl)
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
    else if(item->is(ListingItem::InfoItem))
        this->renderInfo(lock, item, rl);
    else if(!item->is(ListingItem::EmptyItem))
        rl.push("Unknown Type: " + std::to_string(item->type));

    return true;
}

void ListingRenderer::renderSegment(const document_lock& lock, const ListingItem *item, RendererLine &rl)
{
    m_printer->segment(lock->segment(item->address), [&](const std::string& line) {
        rl.push(line, "segment_fg");
    });
}

void ListingRenderer::renderFunction(const document_lock& lock, const ListingItem *item, RendererLine& rl)
{
    if(!(m_flags & ListingRenderer::HideSegmentAndAddress))
        this->renderAddressIndent(lock, item, rl);

    m_printer->function(lock->symbol(item->address), [&](const std::string& pre, const std::string& sym, const std::string& post) {
        if(!pre.empty())
            rl.push(pre, "function_fg");

        rl.push(sym, "function_fg");

        if(!post.empty())
            rl.push(post, "function_fg");
    });
}

void ListingRenderer::renderInstruction(const document_lock& lock, const ListingItem *item, RendererLine &rl)
{
    InstructionPtr instruction = lock->instruction(item->address);

    this->renderAddress(lock, item, rl);
    this->renderIndent(rl, 3);
    this->renderMnemonic(instruction, rl);
    this->renderOperands(instruction, rl);
    this->renderComments(lock, instruction, rl);
}

void ListingRenderer::renderSymbol(const document_lock& lock, const ListingItem *item, RendererLine &rl)
{
    SymbolPtr symbol = lock->symbol(item->address);

    if(symbol->is(SymbolTypes::Code)) // Label or Callback
    {
        const Segment* segment = lock->segment(symbol->address);

        if(segment->is(SegmentTypes::Bss))
        {
            this->renderAddress(lock, item, rl);
            this->renderIndent(rl);
            rl.push(symbol->name, "label_fg");
            rl.push(" <").push("dynamic branch", "label_fg").push(">");
        }
        else
        {
            if(m_flags & ListingRenderer::HideSegmentAndAddress)
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

        if(!segment->is(SegmentTypes::Bss))
        {
            if(symbol->is(SymbolTypes::Pointer))
            {
                if(symbol->isTable())
                {
                    this->renderTable(lock, symbol, rl);
                    return;
                }
                else if(this->renderSymbolPointer(lock, symbol, rl))
                    return;
            }

            if(symbol->is(SymbolTypes::WideStringMask))
                rl.push(REDasm::quoted(m_disassembler->readWString(symbol)), "string_fg");
            else if(symbol->is(SymbolTypes::StringMask))
                rl.push(REDasm::quoted(m_disassembler->readString(symbol)), "string_fg");
            else if(symbol->is(SymbolTypes::ImportMask))
                rl.push("<").push("import", "label_fg").push(">");
            else
            {
                u64 value = 0;
                FormatPlugin* format = m_disassembler->format();

                if(m_disassembler->readAddress(symbol->address, format->addressWidth(), &value))
                    rl.push(REDasm::hex(value, format->bits()), "data_fg");
                else
                    rl.push("??", "data_fg");
            }
        }
        else if(symbol->is(SymbolTypes::ImportMask))
            rl.push("<").push("import", "label_fg").push(">");
        else
            rl.push("??", "data_fg");
    }
}

void ListingRenderer::renderInfo(const document_lock &lock, const ListingItem *item, RendererLine &rl)
{
    this->renderAddressIndent(lock, item, rl);
    rl.push(".info ", "dotted_fg").push(lock->info(item->address), "comment_fg");
}

void ListingRenderer::renderAddress(const document_lock &lock, const ListingItem *item, RendererLine &rl)
{
    if(m_flags & ListingRenderer::HideSegmentName && !(m_flags & ListingRenderer::HideAddress))
        rl.push(HEX_ADDRESS(item->address), "address_fg");
    else if(!(m_flags & ListingRenderer::HideAddress))
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
    else if(instruction->is(REDasm::InstructionTypes::Stop))
        rl.push(mnemonic, "instruction_stop");
    else if(instruction->is(REDasm::InstructionTypes::Nop))
        rl.push(mnemonic, "instruction_nop");
    else if(instruction->is(REDasm::InstructionTypes::Call))
        rl.push(mnemonic, "instruction_call");
    else if(instruction->is(REDasm::InstructionTypes::Jump))
    {
        if(instruction->is(REDasm::InstructionTypes::Conditional))
            rl.push(mnemonic, "instruction_jmp_c");
        else
            rl.push(mnemonic, "instruction_jmp");
    }
    else
        rl.push(mnemonic);
}

void ListingRenderer::renderOperands(const InstructionPtr &instruction, RendererLine &rl)
{
    m_printer->out(instruction, [&](const REDasm::Operand& operand, const std::string& opsize, const std::string& opstr) {
        if(operand.index > 0)
            rl.push(", ");

        if(!opsize.empty())
            rl.push(opsize + " ");

        if(operand.isNumeric()) {
            if(operand.is(REDasm::OperandTypes::Memory))
                rl.push(opstr, "memory_fg");
            else
                rl.push(opstr, "immediate_fg");
        }
        else if(operand.is(REDasm::OperandTypes::Displacement))
            rl.push(opstr, "displacement_fg");
        else if(operand.is(REDasm::OperandTypes::Register))
            rl.push(opstr, "register_fg");
        else
            rl.push(opstr);
    });
}

void ListingRenderer::renderComments(const document_lock &lock, const InstructionPtr &instruction, RendererLine &rl)
{
    std::string s = lock->comment(instruction->address);

    if(s.empty())
        return;

    this->renderIndent(rl, INDENT_COMMENT);
    rl.push("# " + ListingRenderer::escapeString(s), "comment_fg");
}

void ListingRenderer::renderAddressIndent(const document_lock& lock, const ListingItem* item, RendererLine &rl)
{
    FormatPlugin* format = m_disassembler->format();
    const Segment* segment = lock->segment(item->address);

    s64 count = format->bits() / 4;

    if(segment)
        count += segment->name.length();

    rl.push(std::string(count + INDENT_WIDTH, ' '));
}

void ListingRenderer::renderIndent(RendererLine &rl, int n) { rl.push(std::string(n * INDENT_WIDTH, ' ')); }

void ListingRenderer::renderTable(const document_lock &lock, const SymbolPtr &symbol, RendererLine& rl) const
{
    u64 value = 0;
    FormatPlugin* format = m_disassembler->format();
    address_t address = symbol->address;

    rl.push("[");

    for(size_t i = 0; i < symbol->tag; i++, address += format->addressWidth())
    {
        if(i)
            rl.push(", ");

        if(!m_disassembler->readAddress(address, format->addressWidth(), &value))
        {
            rl.push("??", "data_fg");
            continue;
        }

        SymbolPtr ptrsymbol = lock->symbol(value);

        if(!ptrsymbol)
            rl.push(REDasm::hex(value, format->bits()), "data_fg");
        else
            rl.push(ptrsymbol->name, "label_fg");
    }

    rl.push("]");
}

bool ListingRenderer::renderSymbolPointer(const document_lock &lock, const SymbolPtr &symbol, RendererLine &rl) const
{
    u64 value = 0;
    FormatPlugin* format = m_disassembler->format();

   if(!m_disassembler->readAddress(symbol->address, format->addressWidth(), &value))
       return false;

   SymbolPtr ptrsymbol = lock->symbol(value);

   if(!ptrsymbol)
       return false;

   rl.push(ptrsymbol->name, "label_fg");
   return true;
}

std::string ListingRenderer::escapeString(const std::string &s)
{
    std::string res;

    for(size_t i = 0; i < s.size(); i++)
    {
        switch(s[i])
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
                res += s[i];
                break;
        }
    }

    return res;
}

} // namespace REDasm
