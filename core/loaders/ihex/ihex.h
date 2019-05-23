#pragma once

// https://en.wikipedia.org/wiki/Intel_HEX

#include "../../plugins/plugins.h"

namespace REDasm {

class IHexLoader: public LoaderPluginB
{
    PLUGIN_NAME("Intel HEX")
    DECLARE_LOADER_PLUGIN_TEST(u8)

    public:
        IHexLoader(AbstractBuffer* buffer);
        void load() override;
};

DECLARE_LOADER_PLUGIN_FLAGS(IHexLoader, ihex, LoaderFlags::CustomAssembler)

} // namespace REDasm
