#pragma once

#define MIN_STRING 4

#include <chrono>
#include <redasm/disassembler/listing/listingdocument.h>
#include <redasm/disassembler/concurrent/jobspool.h>
#include <redasm/disassembler/disassembler.h>
#include <redasm/plugins/assembler/algorithm/algorithm.h>
#include <redasm/plugins/loader/loader.h>
#include <redasm/support/utils.h>
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
        const safe_ptr<ListingDocumentType>& document() const;
        safe_ptr<ListingDocumentType>& document();
        const safe_ptr<ListingDocumentTypeNew>& documentNew() const;
        safe_ptr<ListingDocumentTypeNew>& documentNew();
        SortedList getCalls(address_t address);
        ReferenceTable* references();
        SortedSet getReferences(address_t address) const;
        SortedSet getTargets(address_t address) const;
        BufferView getFunctionBytes(address_t address);
        Symbol* dereferenceSymbol(const Symbol* symbol, u64* value = nullptr);
        CachedInstruction disassembleInstruction(address_t address);
        address_location getTarget(address_t address) const;
        size_t getTargetsCount(address_t address) const;
        size_t getReferencesCount(address_t address) const;
        size_t checkAddressTable(const CachedInstruction &instruction, address_t startaddress);
        size_t locationIsString(address_t address, bool *wide = nullptr) const;
        JobState state() const;
        String readString(const Symbol* symbol, size_t len = REDasm::npos) const;
        String readString(address_t address, size_t len = REDasm::npos) const;
        String readWString(const Symbol* symbol, size_t len = REDasm::npos) const;
        String readWString(address_t address, size_t len = REDasm::npos) const;
        String getHexDump(address_t address, const Symbol** ressymbol = nullptr);
        bool loadSignature(const String& signame);
        bool busy() const;
        bool checkString(address_t fromaddress, address_t address);
        bool readAddress(address_t address, size_t size, u64 *value) const;
        bool readOffset(offset_t offset, size_t size, u64 *value) const;
        bool dereference(address_t address, u64* value) const;
        void disassemble(address_t address);
        void popTarget(address_t address, address_t pointedby);
        void pushTarget(address_t address, address_t pointedby);
        void pushReference(address_t address, address_t refby);
        void checkLocation(address_t fromaddress, address_t address);
        void computeBasicBlocks();
        void disassemble();
        void stop();
        void pause();
        void resume();

    private:
        void analyzeStep();
        void computeBasicBlocks(document_x_lock &lock, address_t address);
        template<typename T> String readStringT(address_t address, size_t len, std::function<bool(T, String&)> fill) const;
        template<typename T> size_t locationIsStringT(address_t address, std::function<bool(T)> isp, std::function<bool(T)> isa) const;

    private:
        std::chrono::steady_clock::time_point m_starttime;
        std::unique_ptr<DisassemblerEngine> m_engine;
        safe_ptr<Algorithm> m_algorithm;
        ReferenceTable m_referencetable;
        Assembler* m_assembler;
        Loader* m_loader;
        Job m_analyzejob;
};

template<typename T> String DisassemblerImpl::readStringT(address_t address, size_t len, std::function<bool(T, String&)> fill) const
{
    BufferView view = m_loader->view(address);
    String s;
    size_t i;

    for(i = 0; (i < len) && !view.eob() && fill(static_cast<T>(view), s); i++)
        view += sizeof(T);

    String res = s.simplified();

    if(i >= len)
        res += "...";

    return res;
}

template<typename T> size_t DisassemblerImpl::locationIsStringT(address_t address, std::function<bool(T)> isp, std::function<bool(T)> isa) const
{
    BufferView view = m_loader->view(address);

    if(view.eob())
        return 0;

    size_t alphacount = 0, count = 0;
    u8 firstchar = *view;

    while(!view.eob() && isp(static_cast<T>(view)))
    {
        count++;

        if(isa(static_cast<T>(view)))
            alphacount++;

        if(alphacount >= MIN_STRING)
            break;

        view += sizeof(T);
    }

    if(!count)
        return 0;

    if(firstchar == '%') // Special case for C-style formatting
        return MIN_STRING;

    if((static_cast<double>(alphacount) / count) < 0.51) // ...it might be just data, check alpha ratio...
        return 0;

    return count;
}

} // namespace REDasm
