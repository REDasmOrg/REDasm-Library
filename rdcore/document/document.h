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
        bool pointer(rd_address address, rd_type type, const std::string& name);
        bool typeName(rd_address address, const std::string& q);
        bool type(rd_address address, const Type* type);
        void checkLocation(rd_address fromaddress, rd_address address);

    private:
        bool type(rd_address address, const Type* type, int level);
        void updateComments(rd_address fromaddress, rd_address address, const char* symbolname, rd_type type, rd_flag flags);
        void markPointer(rd_address fromaddress, rd_address address);
        void markLocation(rd_address fromaddress, rd_address address);
        bool markString(rd_address address, rd_flag* resflags);

    private:
        std::unique_ptr<DocumentNet> m_net;
};
