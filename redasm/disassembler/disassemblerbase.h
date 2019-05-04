#pragma once

#include <functional>
#include "../plugins/loader.h"
#include "types/referencetable.h"
#include "disassemblerapi.h"

namespace REDasm {

class DisassemblerBase: public DisassemblerAPI
{
    public:
        DisassemblerBase(AssemblerPlugin* assembler, LoaderPlugin* loader);
        virtual ~DisassemblerBase() = default;

    public: // Primitive functions
        LoaderPlugin* loader() const override;
        AssemblerPlugin* assembler() const override;
        const ListingDocument& document() const override;
        ListingDocument& document() override;
        ReferenceTable* references() override;
        ReferenceVector getReferences(address_t address) const override;
        ReferenceSet getTargets(address_t address) const override;
        ListingItems getCalls(address_t address) override;
        address_location getTarget(address_t address) const override;
        u64 getTargetsCount(address_t address) const override;
        u64 getReferencesCount(address_t address) const override;
        void computeBounds() override;
        void popTarget(address_t address, address_t pointedby) override;
        void pushTarget(address_t address, address_t pointedby) override;
        void pushReference(address_t address, address_t refby) override;
        void checkLocation(address_t fromaddress, address_t address) override;
        bool checkString(address_t fromaddress, address_t address) override;
        s64 checkAddressTable(const InstructionPtr &instruction, address_t startaddress) override;
        u64 locationIsString(address_t address, bool *wide = nullptr) const override;
        Symbol* dereferenceSymbol(const Symbol* symbol, u64 *value = nullptr) override;
        bool dereference(address_t address, u64 *value) const override;
        BufferView getFunctionBytes(address_t address) override;
        bool readAddress(address_t address, size_t size, u64 *value) const override;
        bool readOffset(offset_t offset, size_t size, u64 *value) const override;
        std::string readString(const Symbol *symbol, u64 len = std::numeric_limits<u64>::max()) const override;
        std::string readString(address_t address, u64 len = std::numeric_limits<u64>::max()) const override;
        std::string readWString(address_t address, u64 len = std::numeric_limits<u64>::max()) const override;
        std::string readWString(const Symbol *symbol, u64 len = std::numeric_limits<u64>::max()) const override;
        std::string getHexDump(address_t address, const Symbol** ressymbol = nullptr) override;
        bool loadSignature(const std::string& signame) override;

   private:
        void computeBounds(document_x_lock &lock, const ListingItem *functionitem);
        template<typename T> std::string readStringT(address_t address, u64 len, std::function<bool(T, std::string&)> fill) const;
        template<typename T> u64 locationIsStringT(address_t address, std::function<bool(T)> isp, std::function<bool(T)> isa) const;

   protected:
        std::unique_ptr<AssemblerPlugin> m_assembler;
        std::unique_ptr<LoaderPlugin> m_loader;
        ReferenceTable m_referencetable;
};

template<typename T> std::string DisassemblerBase::readStringT(address_t address, u64 len, std::function<bool(T, std::string&)> fill) const
{
    BufferView view = m_loader->view(address);
    std::string s;
    u64 i;

    for(i = 0; (i < len) && !view.eob() && fill(static_cast<T>(view), s); i++)
        view += sizeof(T);

    std::string res = REDasm::simplified(s);

    if(i >= len)
        res += "...";

    return res;
}

template<typename T> u64 DisassemblerBase::locationIsStringT(address_t address, std::function<bool(T)> isp, std::function<bool(T)> isa) const
{
    BufferView view = m_loader->view(address);

    if(view.eob())
        return 0;

    u64 alphacount = 0, count = 0;
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
