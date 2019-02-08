#ifndef BINARY_H
#define BINARY_H

#include "../../plugins/plugins.h"

namespace REDasm {

class BinaryFormat : public FormatPluginB
{
    DEFINE_FORMAT_PLUGIN_TEST(u8)

    public:
        BinaryFormat(AbstractBuffer* buffer);
        virtual std::string name() const;
        virtual std::string assembler() const;
        virtual u32 bits() const;
        virtual bool isBinary();
        virtual void load();
        void build(const std::string& assembler, u32 bits, offset_t offset, address_t baseaddress, address_t entrypoint, u32 segmenttype = SegmentTypes::Code | SegmentTypes::Data);

    private:
        std::string m_assembler;
        u32 m_bits;
};

DECLARE_FORMAT_PLUGIN(BinaryFormat, binary)

} // namespace REDasm

#endif // BINARY_H
