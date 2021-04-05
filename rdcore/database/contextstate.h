#pragma once

#include "../document/document_fwd.h"
#include "../object.h"

class SerializerWriter;

class ContextState : public Object
{
    public:
        ContextState(Context* ctx);
        bool load(const std::string& filepath);
        bool save(const std::string& filepath);

    private:
        void serializeHeader(const SafeDocument& doc, SerializerWriter& writer) const;
        void serializeSegments(const SafeDocument& doc, SerializerWriter& writer) const;
};

