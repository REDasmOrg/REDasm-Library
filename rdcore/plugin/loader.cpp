#include "loader.h"
#include "../document/document.h"
#include "../support/utils.h"

Loader::Loader(const MemoryBufferPtr& buffer, const std::string& filepath, const RDEntryLoader* entry, Context* ctx): Entry<RDEntryLoader>(entry, ctx), m_filepath(filepath)
{
    m_document = SafeDocument(new Document(buffer, ctx));
}

bool Loader::load()
{
    if(!m_entry->load) return false;
    return m_entry->load(CPTR(RDContext, this->context()));
}

bool Loader::build(const RDLoaderBuildParams* buildparams)
{
    static RDLoaderBuildParams bp{ };

    if(!m_entry->build) return false;
    return m_entry->build(CPTR(RDContext, this->context()), buildparams ? buildparams : &bp);
}

rd_flag Loader::flags() const { return m_entry->flags; }
const std::string& Loader::filePath() const { return m_filepath; }
const SafeDocument& Loader::document() const { return m_document; }
SafeDocument& Loader::document() { return m_document; }

const char* Loader::test(const RDEntryLoader* entry, const RDLoaderRequest* req)
{
    if(!entry->test) return nullptr;
    return entry->test(req);
}
