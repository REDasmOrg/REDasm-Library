#pragma once

#include "../buffer/bufferview.h"
#include "../support/safe_ptr.h"
#include "../types/containers/list.h"
#include "../types/api.h"
#include "../pimpl.h"
#include "listing/cachedinstruction.h"
#include "listing/document/listingitem.h"
#include "listing/backend/referencetable.h"
#include "listing/backend/symbols/symboltable.h"

namespace REDasm {

class ListingDocumentType;
class DisassemblerImpl;
class Assembler;
class Loader;

class LIBREDASM_API Disassembler: public Object
{
    REDASM_OBJECT(Disassembler)
    PIMPL_DECLARE_P(Disassembler)
    PIMPL_DECLARE_PRIVATE(Disassembler)

    public:
        Disassembler(Assembler* assembler, Loader* loader);
        virtual ~Disassembler();
        Loader* loader() const;
        Assembler* assembler() const;
        const safe_ptr<ListingDocumentType>& document() const;
        safe_ptr<ListingDocumentType>& document();
        ReferenceTable* references();
        SortedSet getReferences(address_t address) const;
        SortedSet getTargets(address_t address) const;
        BufferView getFunctionBytes(address_t address);
        const Symbol* dereferenceSymbol(const Symbol* symbol, u64* value = nullptr);
        CachedInstruction decodeInstruction(address_t address);
        address_location getTarget(address_t address) const;
        size_t getTargetsCount(address_t address) const;
        size_t getReferencesCount(address_t address) const;
        size_t checkAddressTable(const CachedInstruction& instruction, address_t address);
        String readString(const Symbol* symbol, size_t len = REDasm::npos) const;
        String readString(address_t address, size_t len = REDasm::npos) const;
        String readWString(const Symbol* symbol, size_t len = REDasm::npos) const;
        String readWString(address_t address, size_t len = REDasm::npos) const;
        String getHexDump(address_t address, const Symbol** ressymbol = nullptr);
        void loadSignature(const String& signame);
        bool busy() const;
        bool needsWeak() const;
        bool readAddress(address_t address, size_t size, u64 *value) const;
        bool readOffset(offset_t offset, size_t size, u64 *value) const;
        bool dereference(address_t address, u64* value) const;
        void disassemble(address_t address);
        void disassemble();
        void popTarget(address_t address, address_t pointedby);
        void pushTarget(address_t address, address_t pointedby);
        void pushReference(address_t address, address_t refby);
        void checkLocation(address_t fromaddress, address_t address);
        void stop();
};

typedef std::shared_ptr<Disassembler> DisassemblerPtr;

} // namespace REDasm
