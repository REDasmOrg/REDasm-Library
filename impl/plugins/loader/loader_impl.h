#pragma once

#include <memory>
#include <redasm/buffer/bufferview.h>
#include <redasm/disassembler/listing/listingdocument.h>
#include <redasm/plugins/loader/loader.h>
#include "../plugin_impl.h"

namespace REDasm {

class LoaderImpl: public PluginImpl
{
    PIMPL_DECLARE_Q(Loader)
    PIMPL_DECLARE_PUBLIC(Loader)

    public:
        LoaderImpl();
        virtual ~LoaderImpl();
        AbstractBuffer* buffer() const;
        BufferView viewOffset(offset_t offset) const;
        const BufferView& view() const;
        BufferView view(address_t address) const;
        BufferView viewSegment(const Segment* segment) const;
        ListingDocument& createDocument();
        const ListingDocument& document() const;
        ListingDocument& document();
        SignatureIdentifiers &signatures();
        void signature(const std::string& sig);

    public:
        offset_location offset(address_t address) const;
        address_location address(offset_t offset) const;
        AssemblerRequest assembler() const;
        Analyzer *analyzer(Disassembler* disassembler);
        void build(const std::string& assembler, offset_t offset, address_t baseaddress, address_t entrypoint);
        void init(const LoadRequest& loader);

    private:
        std::unique_ptr<Analyzer> m_analyzer;
        SignatureIdentifiers m_signatures;
        AbstractBuffer* m_buffer;
        ListingDocument m_document;
        BufferView m_view;
};

} // namespace REDasm
