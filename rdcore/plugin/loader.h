#pragma once

#include <memory>
#include <rdapi/plugin/loader.h>
#include "../document/document.h"
#include "../buffer/buffer.h"
#include "../object.h"

class Disassembler;

class Loader: public Object
{
    public:
        Loader(const RDLoaderRequest* req, RDLoaderPlugin* ploader);
        ~Loader();
        bool load();
        bool build(const RDLoaderBuildRequest* req);
        bool analyze(Disassembler* disassembler);
        BufferView* view(address_t address) const;
        BufferView* view(address_t address, size_t size) const;
        BufferView* view(const RDSegment& segment) const;
        flag_t flags() const;
        MemoryBuffer* buffer();
        SafeDocument& document();

    public:
        RDLocation offset(address_t address) const;
        RDLocation address(offset_t offset) const;
        RDLocation addressof(const void* ptr) const;
        RDLocation fileoffset(const void* ptr) const;
        u8* addrpointer(address_t address) const;
        u8* pointer(offset_t offset) const;

    private:
        RDLoaderPlugin* m_ploader;
        std::unique_ptr<MemoryBuffer> m_buffer;
        SafeDocument m_document;
        std::string m_filepath;
};
