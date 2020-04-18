#pragma once

#include <vector>
#include <string>
#include <rdapi/renderer.h>
#include "../object.h"

class RendererItem: public Object
{
    private:
        typedef std::vector<RDRendererFormat> FormatsVector;
        typedef FormatsVector::iterator FormatsIterator;

    public:
        RendererItem() = default;
        void setDocumentIndex(size_t index);
        const std::string& text() const;
        size_t size() const;
        std::string formatText(const RDRendererFormat* format) const;
        FormatsIterator unformat(s32 start, s32 end);
        const RDRendererFormat& format(size_t idx) const;
        RendererItem& format(s32 start, s32 end, const std::string& fgstyle = std::string(), const std::string& bgstyle = std::string());
        RendererItem& push(const std::string& text, const std::string& fgstyle = std::string(), const std::string& bgstyle = std::string());
        size_t formats(const RDRendererFormat** formats) const;
        size_t documentIndex() const;

    public:
        static bool formatContains(const RDRendererFormat* rf, s32 pos);
        static bool formatEmpty(const RDRendererFormat* rf);

    private:
        size_t indexFromPos(s32 pos) const;

    private:
        size_t m_docindex{RD_NPOS};
        FormatsVector m_formats;
        std::string m_text;
};

