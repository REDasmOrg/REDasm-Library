#pragma once

#include <memory>
#include <rdapi/document/block.h>
#include <rdapi/plugin/loader.h>
#include "../document/document_fwd.h"
#include "../buffer/buffer.h"
#include "entry.h"

class Disassembler;

class Loader: public Entry<RDEntryLoader>
{
    public:
        Loader(const std::shared_ptr<MemoryBuffer>& buffer, const std::string& filepath, const RDEntryLoader* entry, Context* ctx);
        bool load();
        bool build();
        bool view(rd_address address, RDBufferView* view) const;
        bool view(rd_address address, size_t size, RDBufferView* view) const;
        bool view(const RDSegment& segment, RDBufferView* view) const;
        bool view(const RDBlock& block, RDBufferView* view) const;
        rd_flag flags() const;
        MemoryBuffer* buffer();
        SafeDocument& document();

    public:
        bool isAddress(rd_address address) const;
        RDLocation offset(rd_address address) const;
        RDLocation address(rd_offset offset) const;
        RDLocation addressof(const void* ptr) const;
        RDLocation fileoffset(const void* ptr) const;
        u8* addrpointer(rd_address address) const;
        u8* pointer(rd_offset offset) const;

    public:
        static const char* test(const RDEntryLoader* entry, const RDLoaderRequest* req);

    private:
        RDLoaderBuildParams m_buildparams{ };
        std::shared_ptr<MemoryBuffer> m_buffer;
        SafeDocument m_document;
        std::string m_filepath;
};
