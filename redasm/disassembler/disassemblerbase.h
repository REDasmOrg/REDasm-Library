#ifndef DISASSEMBLERBASE_H
#define DISASSEMBLERBASE_H

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
        virtual LoaderPlugin* loader() const;
        virtual AssemblerPlugin* assembler() const;
        virtual const ListingDocument& document() const;
        virtual ListingDocument& document();
        virtual ReferenceTable* references();
        virtual ReferenceVector getReferences(address_t address) const;
        virtual ReferenceSet getTargets(address_t address) const;
        virtual ListingItems getCalls(address_t address);
        virtual address_location getTarget(address_t address) const;
        virtual u64 getTargetsCount(address_t address) const;
        virtual u64 getReferencesCount(address_t address) const;
        virtual void popTarget(address_t address, address_t pointedby);
        virtual void pushTarget(address_t address, address_t pointedby);
        virtual void pushReference(address_t address, address_t refby);
        virtual void checkLocation(address_t fromaddress, address_t address);
        virtual bool checkString(address_t fromaddress, address_t address);
        virtual s64 checkAddressTable(const InstructionPtr &instruction, address_t startaddress);
        virtual u64 locationIsString(address_t address, bool *wide = nullptr) const;
        virtual Symbol* dereferenceSymbol(const Symbol* symbol, u64 *value = nullptr);
        virtual bool dereference(address_t address, u64 *value) const;
        virtual BufferView getFunctionBytes(address_t address);
        virtual bool readAddress(address_t address, size_t size, u64 *value) const;
        virtual bool readOffset(offset_t offset, size_t size, u64 *value) const;
        virtual std::string readString(const Symbol *symbol, u64 len = std::numeric_limits<u64>::max()) const;
        virtual std::string readString(address_t address, u64 len = std::numeric_limits<u64>::max()) const;
        virtual std::string readWString(address_t address, u64 len = std::numeric_limits<u64>::max()) const;
        virtual std::string readWString(const Symbol *symbol, u64 len = std::numeric_limits<u64>::max()) const;
        virtual std::string getHexDump(address_t address, const Symbol** ressymbol = nullptr);
        virtual bool loadSignature(const std::string& signame);

   private:
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

#endif // DISASSEMBLERBASE_H
