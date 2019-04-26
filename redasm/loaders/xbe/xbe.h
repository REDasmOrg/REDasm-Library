#pragma once

// Documentation: www.caustik.com/cxbx/download/xbe.htm

#include "../../plugins/plugins.h"
#include "xbe_header.h"

namespace REDasm {

class XbeLoader : public LoaderPluginT<XbeImageHeader>
{
    PLUGIN_NAME("XBox Executable")
    DECLARE_LOADER_PLUGIN_TEST(XbeImageHeader)

    public:
        XbeLoader(AbstractBuffer* buffer);
        virtual std::string assembler() const;
        virtual void load();

    private:
        void displayXbeInfo();
        bool decodeEP(u32 encodedep, address_t &ep);
        bool decodeKernel(u32 encodedthunk, u32 &thunk);
        void loadSections(XbeSectionHeader* sectionhdr);
        bool loadXBoxKrnl();

    private:
        template<typename T> T* memoryoffset(u32 memaddress) const;
};

template<typename T> T* XbeLoader::memoryoffset(u32 memaddress) const { return this->pointer<T>(memaddress - this->m_header->BaseAddress); }

DECLARE_LOADER_PLUGIN(XbeLoader, xbe)

} // namespace REDasm
