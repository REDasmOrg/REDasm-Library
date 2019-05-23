#pragma once

#include "../../plugins/plugins.h"

namespace REDasm {

class BinaryLoader : public LoaderPluginB
{
    PLUGIN_NAME("Binary")
    DECLARE_LOADER_PLUGIN_TEST(u8)

    public:
        BinaryLoader(AbstractBuffer* buffer);
        std::string assembler() const override;
        void load() override;
        void build(const std::string& assembler, offset_t offset, address_t baseaddress, address_t entrypoint) override;

    private:
        std::string m_assembler;
};

DECLARE_LOADER_PLUGIN_FLAGS(BinaryLoader, binary, LoaderFlags::Binary)

} // namespace REDasm
