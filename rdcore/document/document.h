#pragma once

#include "document_fwd.h"
#include "documentdata.h"
#include "documentnet.h"

class Document: public DocumentData
{
    public:
        Document(const MemoryBufferPtr& buffer, Context* ctx);
        DocumentNet* net() const;

    public:
        const Type* type(const RDDocumentItem* item) const;
        bool typeName(rd_address address, const std::string& q);
        bool type(rd_address address, const Type* type);
        void checkLocation(rd_address fromaddress, rd_address address);

    private:
        bool type(rd_address address, const Type* type, int level);
        void updateComments(rd_address address, rd_address symboladdress, const std::string& prefix = std::string());
        size_t markString(rd_address address, rd_flag* resflags);

    private:
        std::unique_ptr<DocumentNet> m_net;
};
