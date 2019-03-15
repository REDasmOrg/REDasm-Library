#ifndef BINARYLOADER_H
#define BINARYLOADER_H

#include "../../plugins/plugins.h"

namespace REDasm {

class BinaryLoader : public LoaderPluginB
{
    PLUGIN_NAME("Binary")
    DECLARE_LOADER_PLUGIN_TEST(u8)

    public:
        BinaryLoader(AbstractBuffer* buffer);
        virtual std::string assembler() const;
        virtual void load();
        virtual void build(const std::string& assembler, offset_t offset, address_t baseaddress, address_t entrypoint);

    private:
        std::string m_assembler;
        u32 m_bits;
};

DECLARE_LOADER_PLUGIN_FLAGS(BinaryLoader, binary, LoaderFlags::Binary)

} // namespace REDasm

#endif // BINARYLOADER_H
