#pragma once

#include <redasm/disassembler/disassembler.h>
#include <redasm/disassembler/model/calltree.h>
#include <redasm/plugins/assembler/algorithm/algorithm.h>
#include <redasm/plugins/loader/loader.h>
#include <redasm/pimpl.h>
#include "engine/disassemblerengine.h"

namespace REDasm {

class DisassemblerImpl
{
    PIMPL_DECLARE_Q(Disassembler)
    PIMPL_DECLARE_PUBLIC(Disassembler)

    public:
        DisassemblerImpl(Disassembler *q, Assembler *assembler, Loader *loader);
        ~DisassemblerImpl();
        Loader* loader() const;
        Assembler* assembler() const;
        const safe_ptr<ListingDocumentType>& documentNew() const;
        safe_ptr<ListingDocumentType>& documentNew();
        ReferenceTable* references();
        SortedSet getReferences(address_t address) const;
        SortedSet getTargets(address_t address) const;
        BufferView getFunctionBytes(address_t address);
        const Symbol* dereferenceSymbol(const Symbol* symbol, u64* value = nullptr);
        CachedInstruction decodeInstruction(address_t address);
        address_location getTarget(address_t address) const;
        size_t getTargetsCount(address_t address) const;
        size_t getReferencesCount(address_t address) const;
        size_t checkAddressTable(const CachedInstruction &instruction, address_t startaddress);
        String readString(const Symbol* symbol, size_t len = REDasm::npos) const;
        String readString(address_t address, size_t len = REDasm::npos) const;
        String readWString(const Symbol* symbol, size_t len = REDasm::npos) const;
        String readWString(address_t address, size_t len = REDasm::npos) const;
        String getHexDump(address_t address, const Symbol** ressymbol = nullptr);
        bool loadSignature(const String& signame);
        bool busy() const;
        bool needsWeak() const;
        bool readAddress(address_t address, size_t size, u64 *value) const;
        bool readOffset(offset_t offset, size_t size, u64 *value) const;
        bool dereference(address_t address, u64* value) const;
        void disassemble(address_t address);
        void popTarget(address_t address, address_t pointedby);
        void pushTarget(address_t address, address_t pointedby);
        void pushReference(address_t address, address_t refby);
        void checkLocation(address_t fromaddress, address_t address);
        void disassemble();
        void stop();

    private:
        template<typename T> String readStringT(address_t address, size_t len) const;

    private:
        std::unique_ptr<DisassemblerEngine> m_engine;
        ReferenceTable m_referencetable;
        Assembler* m_assembler;
        Loader* m_loader;
};

} // namespace REDasm
