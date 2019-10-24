#pragma once

#define MIN_STRING          4

#include <functional>
#include <redasm/plugins/loader/loader.h>
#include <redasm/buffer/bufferview.h>

namespace REDasm {

struct StringFinderResult
{
    StringFinderResult();

    address_location address;
    size_t len{REDasm::npos};
    bool wide{false};
};

class StringFinder
{
    public:
        StringFinder(const BufferView& view);
        void find();

    private:
        SymbolFlags categorize(const BufferView& view, size_t* totalsize);

    private:
        template<typename T> static bool inline isAscii(T c) { return (c >= 0x09 && c <= 0x0D) || (c >= 0x20 && c <= 0x7E); }
        template<typename T> size_t locationIsStringT(const BufferView& view, size_t* size, std::function<bool(T)> isp, std::function<bool(T)> isa) const;

    private:
        BufferView m_view;
};

template<typename T> size_t StringFinder::locationIsStringT(const BufferView &view, size_t* size, std::function<bool(T)> isp, std::function<bool(T)> isa) const
{
    size_t alphacount = 0, count = 0;
    const T* p = reinterpret_cast<const T*>(view.data());
    T firstchar = *p;

    for(size_t i = 0; (i < view.size()) && (isp(*p) || !(*p)); i++, p++)
    {
        count += sizeof(T);
        if(isa(*p)) alphacount++;
        if(!(*p)) break;
    }

    //if((static_cast<double>(alphacount) / count) < 0.51) // ...it might be just data, check alpha ratio...
        //return 0;

    if(size) *size = count * sizeof(T);
    return count;
}

} // namespace REDasm
