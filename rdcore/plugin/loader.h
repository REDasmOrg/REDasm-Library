#pragma once

#include <memory>
#include <rdapi/document/block.h>
#include <rdapi/plugin/loader.h>
#include <rdapi/support/utils.h>
#include "../document/document_fwd.h"
#include "../buffer/buffer.h"
#include "entry.h"

class Loader: public Entry<RDEntryLoader>
{
    public:
        Loader(const MemoryBufferPtr& buffer, const std::string& filepath, const RDEntryLoader* entry, Context* ctx);
        bool load();
        bool build(const RDLoaderBuildParams* buildparams);
        rd_flag flags() const;
        const std::string& filePath() const;
        const SafeDocument& document() const;
        SafeDocument& document();

    public:
        static const char* test(const RDEntryLoader* entry, const RDLoaderRequest* req);

    private:
        SafeDocument m_document;
        std::string m_filepath;
};
