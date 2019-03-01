#ifndef DISASSEMBLERBASE_H
#define DISASSEMBLERBASE_H

#include <functional>
#include "../plugins/format.h"
#include "types/referencetable.h"
#include "disassemblerapi.h"

namespace REDasm {

class DisassemblerBase: public DisassemblerAPI
{
    public:
        DisassemblerBase(FormatPlugin* format);

    public: // Primitive functions
        virtual FormatPlugin* format();
        virtual ListingDocument &document();
        virtual ReferenceTable* references();
        virtual ReferenceVector getReferences(address_t address);
        virtual u64 getReferencesCount(address_t address);
        virtual void pushReference(address_t address, address_t refbyaddress);
        virtual void checkLocation(address_t fromaddress, address_t address);
        virtual bool checkString(address_t fromaddress, address_t address);
        virtual s64 checkAddressTable(const InstructionPtr &instruction, address_t startaddress);
        virtual u64 locationIsString(address_t address, bool *wide = nullptr, bool *middle = nullptr) const;
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
        virtual bool loadSignature(const std::string& sdbfile);

   private:
        template<typename T> std::string readStringT(address_t address, u64 len, std::function<bool(T, std::string&)> fill) const;
        template<typename T> u64 locationIsStringT(address_t address, std::function<bool(T)> isp, std::function<bool(T)> isa, bool* middle = nullptr) const;

   protected:
        ListingDocument& m_document;
        std::unique_ptr<FormatPlugin> m_format;
        ReferenceTable m_referencetable;
};

template<typename T> std::string DisassemblerBase::readStringT(address_t address, u64 len, std::function<bool(T, std::string&)> fill) const
{
    BufferView view = m_format->view(address);
    std::string s;
    u64 i;

    for(i = 0; (i < len) && !view.eob() && fill(static_cast<T>(view), s); i++)
        view += sizeof(T);

    std::string res = REDasm::simplified(s);

    if(i >= len)
        res += "...";

    return res;
}

template<typename T> u64 DisassemblerBase::locationIsStringT(address_t address, std::function<bool(T)> isp, std::function<bool(T)> isa, bool* middle) const
{
    Segment* segment = m_document->segment(address);

    if(!segment)
        return 0;

    u64 alphacount = 0, count = 0;
    BufferView view = m_format->view(address);

    while(!view.eob() && isp(static_cast<T>(view)))
    {
        count++;

        if(isa(static_cast<T>(view)))
            alphacount++;

        if(alphacount >= MIN_STRING)
            break;

        view += sizeof(T);
    }

    if(!count || ((static_cast<double>(alphacount) / count) < 0.51)) // ...it might be just data, check alpha ratio...
        return 0;

    if(middle)
    {
        *middle = false;
        address_t prevaddress = address - sizeof(T);

        if((address >= sizeof(T)) && (m_document->segment(prevaddress) == segment))
            *middle = isa(static_cast<T>(m_format->view(prevaddress)));
    }

    return count;
}

} // namespace REDasm

#endif // DISASSEMBLERBASE_H
