#ifndef FORMAT_H
#define FORMAT_H

#include "../disassembler/disassemblerapi.h"
#include "../disassembler/types/symboltable.h"
#include "../disassembler/listing/listingdocument.h"
#include "../analyzer/analyzer.h"
#include "../types/endianness/endianness.h"
#include "base.h"

#define DECLARE_FORMAT_PLUGIN(T, id) inline FormatPlugin* id##_formatPlugin(AbstractBuffer* buffer) { return REDasm::getFormatPlugin<T>(buffer); }
#define DEFINE_FORMAT_PLUGIN_TEST(T) public: static bool test(const T* format, BufferView& buffer); private:
#define FORMAT_PLUGIN_TEST(T, H) bool T::test(const H* format, BufferView& view)

namespace REDasm {

template<typename T> T* getFormatPlugin(AbstractBuffer* buffer)
{
    const typename T::FormatHeader* format = reinterpret_cast<const typename T::FormatHeader*>(buffer->data());
    BufferView view = buffer->view();

    if((sizeof(typename T::FormatHeader) > buffer->size()) || !T::test(format, view))
        return NULL;

    T* formatplugin = new T(buffer);
    formatplugin->load();
    return formatplugin;
}

namespace FormatFlags {
    enum: u32 { None = 0, Binary = 1, };
}

class FormatPlugin: public Plugin
{
    public:
        FormatPlugin(AbstractBuffer *view);
        bool isBinary() const;
        AbstractBuffer* buffer() const;
        BufferView viewOffset(offset_t offset) const;
        BufferView view(address_t address) const;
        ListingDocument& document();
        const SignatureFiles& signatures() const;
        u64 addressWidth() const;

    public:
        virtual offset_t offset(address_t address) const;
        virtual address_t address(offset_t offset) const;
        virtual Analyzer *createAnalyzer(DisassemblerAPI* disassembler, const SignatureFiles &signatures) const;
        virtual std::string assembler() const = 0;
        virtual u32 bits() const = 0;
        virtual u32 flags() const;
        virtual void load() = 0;

    protected:
        std::unique_ptr<AbstractBuffer> m_buffer;
        BufferView m_view;
        ListingDocument m_document;
        SignatureFiles m_signatures;
};

template<typename T> class FormatPluginT: public FormatPlugin
{
    public:
        typedef T FormatHeader;

    public:
        FormatPluginT(AbstractBuffer* buffer): FormatPlugin(buffer) { m_format = reinterpret_cast<T*>(m_buffer->data()); }
        template<typename U> inline offset_t fileoffset(U* ptr) const { return reinterpret_cast<u8*>(ptr) - reinterpret_cast<u8*>(m_format); }
        template<typename U, typename O> inline U* pointer(O offset) const { return reinterpret_cast<U*>(reinterpret_cast<u8*>(m_format) + offset); }
        template<typename U, typename A> inline U* addrpointer(A address) const { return reinterpret_cast<U*>(reinterpret_cast<u8*>(m_format) + offset(address)); }
        template<typename U, typename V, typename O> inline static const U* relpointer(const V* base, O offset) { return reinterpret_cast<const U*>(reinterpret_cast<const u8*>(base) + offset); }
        template<typename U, typename V, typename O> inline static U* relpointer(V* base, O offset) { return reinterpret_cast<U*>(reinterpret_cast<u8*>(base) + offset); }

    public:
        static bool test(const T* format, const AbstractBuffer* buffer) { RE_UNUSED(format); RE_UNUSED(buffer); return false; }

    protected:
        T* m_format;
};

class FormatPluginB: public FormatPluginT<u8>
{
    public:
        FormatPluginB(AbstractBuffer* buffer): FormatPluginT<u8>(buffer) { }
};

typedef std::function<FormatPlugin*(AbstractBuffer*)> FormatPlugin_Entry;

}

#endif // FORMAT_H
