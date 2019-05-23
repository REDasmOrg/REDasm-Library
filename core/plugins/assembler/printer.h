#pragma once

#include <memory>
#include <capstone.h>
#include "../../redasm.h"
#include "../../disassembler/listing/listingdocument.h"
#include "../../disassembler/disassemblerapi.h"

namespace REDasm {

class Printer
{
    public:
        typedef std::function<void(const Operand*, const std::string&, const std::string&)> OpCallback;
        typedef std::function<void(const Symbol*, const std::string&)> SymbolCallback;
        typedef std::function<void(const std::string&, const std::string&, const std::string&)> FunctionCallback;
        typedef std::function<void(const std::string&)> LineCallback;

    public:
        Printer(DisassemblerAPI* disassembler);
        virtual ~Printer() = default;
        std::string symbol(const Symbol *symbol) const;
        std::string out(const InstructionPtr& instruction) const;

    public:
        virtual void segment(const Segment* segment, const LineCallback &segmentfunc);
        virtual void function(const Symbol *symbol, const FunctionCallback &functionfunc);
        virtual void symbol(const Symbol *symbol, const SymbolCallback& symbolfunc) const;
        virtual std::string out(const InstructionPtr& instruction, const OpCallback& opfunc) const;

    public: // Operand privitives
        virtual std::string reg(const RegisterOperand &regop) const;
        virtual std::string disp(const Operand *operand) const;
        virtual std::string loc(const Operand* operand) const;
        virtual std::string mem(const Operand* operand) const;
        virtual std::string imm(const Operand* operand) const;
        virtual std::string size(const Operand* operand) const;

    protected:
        ListingDocument& m_document;
        DisassemblerAPI* m_disassembler;
};

class CapstonePrinter: public Printer
{
    public:
        CapstonePrinter(csh cshandle, DisassemblerAPI* disassembler);

    protected:
        virtual std::string reg(const RegisterOperand &regop) const;

    private:
        csh m_cshandle;
};

typedef std::unique_ptr<Printer> PrinterPtr;

}
