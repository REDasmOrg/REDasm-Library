#pragma once

#include "../buffer/bufferview.h"
#include "../support/safe_ptr.h"
#include "../types/api_types.h"
#include "../support/event.h"
#include "../pimpl.h"
#include "concurrent/jobstate.h"
#include "types/referencetable.h"
#include "types/symboltable.h"

namespace REDasm {

class ListingDocumentType;
class DisassemblerImpl;
class ListingItem;
class Assembler;
class Loader;
class Printer;

class LIBREDASM_API Disassembler
{
    PIMPL_DECLARE_P(Disassembler)
    PIMPL_DECLARE_PRIVATE(Disassembler)

    public:
        SimpleEvent busyChanged;

    public:
        Disassembler(Assembler* assembler, Loader* loader);
        ~Disassembler() = default;
        Loader* loader() const;
        Assembler* assembler() const;
        const safe_ptr<ListingDocumentType>& document() const;
        safe_ptr<ListingDocumentType>& document();
        std::deque<ListingItem*> getCalls(address_t address);
        ReferenceTable* references();
        ReferenceVector getReferences(address_t address) const;
        ReferenceSet getTargets(address_t address) const;
        BufferView getFunctionBytes(address_t address);
        Symbol* dereferenceSymbol(const Symbol* symbol, u64* value = nullptr);
        InstructionPtr disassembleInstruction(address_t address);
        address_location getTarget(address_t address) const;
        size_t getTargetsCount(address_t address) const;
        size_t getReferencesCount(address_t address) const;
        size_t checkAddressTable(const InstructionPtr& instruction, address_t address);
        size_t locationIsString(address_t address, bool *wide = nullptr) const;
        JobState state() const;
        std::string readString(const Symbol* symbol, size_t len = REDasm::npos) const;
        std::string readString(address_t address, size_t len = REDasm::npos) const;
        std::string readWString(const Symbol* symbol, size_t len = REDasm::npos) const;
        std::string readWString(address_t address, size_t len = REDasm::npos) const;
        std::string getHexDump(address_t address, const Symbol** ressymbol = nullptr);
        bool loadSignature(const std::string& signame);
        bool busy() const;
        bool checkString(address_t fromaddress, address_t address);
        bool readAddress(address_t address, size_t size, u64 *value) const;
        bool readOffset(offset_t offset, size_t size, u64 *value) const;
        bool dereference(address_t address, u64* value) const;
        void disassemble(address_t address);
        void disassemble();
        void popTarget(address_t address, address_t pointedby);
        void pushTarget(address_t address, address_t pointedby);
        void pushReference(address_t address, address_t refby);
        void checkLocation(address_t fromaddress, address_t address);
        void computeBasicBlocks();
        void stop();
        void pause();
        void resume();
};

typedef std::shared_ptr<Disassembler> DisassemblerPtr;

} // namespace REDasm
