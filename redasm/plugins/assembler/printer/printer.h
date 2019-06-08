#pragma once

#include "../../../disassembler/listing/listingdocument.h"
#include "../../../types/plugin_types.h"

namespace REDasm {

class Disassembler;
class PrinterImpl;

class Printer: public Object
{
    PIMPL_DECLARE_PRIVATE(Printer)

    public:
        typedef std::function<void(const Operand*, const std::string&, const std::string&)> OpCallback;
        typedef std::function<void(const Symbol*, const std::string&)> SymbolCallback;
        typedef std::function<void(const std::string&, const std::string&, const std::string&)> FunctionCallback;
        typedef std::function<void(const std::string&)> LineCallback;

    protected:
        Printer(PrinterImpl* p);

    public:
        Printer(Disassembler* disassembler);
        virtual ~Printer() = default;
        Disassembler* disassembler() const;
        const ListingDocument& document() const;
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
};

} // namespace REDasm
