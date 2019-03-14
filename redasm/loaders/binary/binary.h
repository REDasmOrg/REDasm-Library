#ifndef BINARYLOADER_H
#define BINARYLOADER_H

#include "../../plugins/plugins.h"

namespace REDasm {

class BinaryLoader : public LoaderPluginB
{
    DEFINE_LOADER_PLUGIN_TEST(u8)

    public:
        BinaryLoader(AbstractBuffer* buffer);
        virtual std::string name() const;
        virtual std::string assembler() const;
        virtual u32 bits() const;
        virtual bool isBinary() const;
        virtual void load();
        void build(const std::string& assembler, u32 bits, offset_t offset, address_t baseaddress, address_t entrypoint, u32 segmenttype = SegmentTypes::Code | SegmentTypes::Data);

    private:
        std::string m_assembler;
        u32 m_bits;
};

DECLARE_LOADER_PLUGIN(BinaryLoader, binary)

} // namespace REDasm

#endif // BINARYLOADER_H
