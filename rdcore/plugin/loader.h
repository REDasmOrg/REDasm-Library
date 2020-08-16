#pragma once

#include <memory>
#include <rdapi/plugin/loader.h>
#include "../document/document.h"
#include "../database/database.h"
#include "../buffer/buffer.h"
#include "../object.h"

class Disassembler;

class Loader: public Object
{
    public:
        Loader(RDLoaderPlugin* ploader, const RDLoaderRequest* req);
        ~Loader();
        bool load();
        bool build(const RDLoaderBuildRequest* req);
        bool analyze(Disassembler* disassembler);
        bool view(rd_address address, RDBufferView* view) const;
        bool view(rd_address address, size_t size, RDBufferView* view) const;
        bool view(const RDSegment& segment, RDBufferView* view) const;
        Database* database(const std::string& dbname);
        rd_flag flags() const;
        const char* id() const;
        MemoryBuffer* buffer();
        SafeDocument& document();

    public:
        RDLocation offset(rd_address address) const;
        RDLocation address(rd_offset offset) const;
        RDLocation addressof(const void* ptr) const;
        RDLocation fileoffset(const void* ptr) const;
        u8* addrpointer(rd_address address) const;
        u8* pointer(rd_offset offset) const;

    private:
        RDLoaderPlugin* m_ploader;
        std::unique_ptr<MemoryBuffer> m_buffer;
        std::unordered_map<std::string, std::unique_ptr<Database>> m_database;
        SafeDocument m_document;
        std::string m_filepath;
};
