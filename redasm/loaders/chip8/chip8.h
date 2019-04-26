#pragma once

#include "../../plugins/plugins.h"

namespace REDasm {

class CHIP8Loader: public LoaderPluginB
{
    PLUGIN_NAME("CHIP-8 ROM")
    DECLARE_LOADER_PLUGIN_TEST(u8)

    public:
        CHIP8Loader(AbstractBuffer* buffer);
        virtual std::string assembler() const;
        virtual void load();
};

DECLARE_LOADER_PLUGIN(CHIP8Loader, chip8)

} // namespace REDasm
