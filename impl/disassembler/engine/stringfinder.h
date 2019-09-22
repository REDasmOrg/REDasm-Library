#pragma once

#define MIN_STRING 4

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
        size_t locationIsString(const BufferView &view, bool *wide = nullptr) const;

    private:
        template<typename T> size_t locationIsStringT(const BufferView& view, std::function<bool(T)> isp, std::function<bool(T)> isa) const;

    private:
        BufferView m_view;
};

template<typename T> size_t StringFinder::locationIsStringT(const BufferView &view, std::function<bool(T)> isp, std::function<bool(T)> isa) const
{
    size_t alphacount = 0, count = 0;
    const T* p = reinterpret_cast<const T*>(view.data());
    T firstchar = *p;

    for(size_t i = 0; (i < view.size()) && isp(*p); i++, p++)
    {
        count += sizeof(T);

        if(isa(*p))
            alphacount++;
    }

    //if((static_cast<double>(alphacount) / count) < 0.51) // ...it might be just data, check alpha ratio...
        //return 0;

    return count;
}

} // namespace REDasm
