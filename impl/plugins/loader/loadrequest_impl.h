#pragma once

#include <redasm/plugins/loader/loader.h>
#include <redasm/pimpl.h>

namespace REDasm {

class LoadRequestImpl
{
    PIMPL_DECLARE_Q(LoadRequest)
    PIMPL_DECLARE_PUBLIC(LoadRequest)

    public:
        LoadRequestImpl(const String& filepath, AbstractBuffer* buffer);
        String filePath() const;
        AbstractBuffer* buffer() const;
        const BufferView& view() const;

    private:
        String m_filepath;
        AbstractBuffer* m_buffer;
        BufferView m_view;
};

} // namespace REDasm
