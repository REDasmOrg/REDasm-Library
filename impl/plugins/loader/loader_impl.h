#pragma once

#include <memory>
#include <redasm/plugins/loader/loader.h>
#include "../plugin_impl.h"

namespace REDasm {

class LoaderImpl: public PluginImpl
{
    PIMPL_DECLARE_Q(Loader)
    PIMPL_DECLARE_PUBLIC(Loader)

    public:
        LoaderImpl(Loader* loader);
        AbstractBuffer* buffer() const;
        BufferView viewOffset(offset_t offset) const;
        BufferView viewOffset(offset_t offset, size_t size) const;
        const BufferView& view() const;
        BufferView view(address_t address) const;
        BufferView view(address_t address, size_t size) const;
        BufferView viewSegment(const Segment* segment) const;
        ListingDocumentNew& createDocumentNew();
        const ListingDocumentNew& documentNew() const;
        ListingDocumentNew& documentNew();
        SignatureIdentifiers &signatures();
        void signature(const String& sig);

    public:
        offset_location offset(address_t address) const;
        address_location address(offset_t offset) const;
        AssemblerRequest assembler() const;
        Analyzer *analyzer();
        void build(const String &assembler, offset_t offset, address_t baseaddress, address_t entrypoint);
        void init(const LoadRequest& request);

    private:
        std::unique_ptr<Analyzer> m_analyzer;
        SignatureIdentifiers m_signatures;
        AbstractBuffer* m_buffer;
        ListingDocumentNew m_documentnew;
        BufferView m_view;
};

} // namespace REDasm
