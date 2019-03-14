#ifndef LOADERPLUGIN_H
#define LOADERPLUGIN_H

#include "../disassembler/disassemblerapi.h"
#include "../disassembler/types/symboltable.h"
#include "../disassembler/listing/listingdocument.h"
#include "../analyzer/analyzer.h"
#include "../types/endianness/endianness.h"
#include "base.h"

#define DECLARE_LOADER_PLUGIN_BASE(T, id)         inline bool id##_plugin_loader_test(const LoadRequest& request) { return REDasm::testLoaderPlugin<T>(request); } \
                                                  inline LoaderPlugin* id##_plugin_loader_init(const LoadRequest& request) { return REDasm::initLoaderPlugin<T>(request); } \
                                                  inline std::string id##_plugin_loader_name() { return T::Name; }

#define DECLARE_LOADER_PLUGIN(T, id)              DECLARE_LOADER_PLUGIN_BASE(T, id) \
                                                  inline u32 id##_plugin_loader_flags() { return LoaderFlags::None; }

#define DECLARE_LOADER_PLUGIN_FLAGS(T, id, flags) DECLARE_LOADER_PLUGIN_BASE(T, id) \
                                                  inline u32 id##_plugin_loader_flags() { return flags; }


#define DEFINE_LOADER_PLUGIN_TEST(T)              public: static bool test(const LoadRequest& request, const T* header); private:
#define LOADER_PLUGIN_TEST(T, H)                  bool T::test(const LoadRequest& request, const H* header)

#define LOADER_PLUGIN_ENTRY(id)                   { &id##_plugin_loader_test, &id##_plugin_loader_init, &id##_plugin_loader_name, &id##_plugin_loader_flags }

#define LOADER_CTOR AbstractBuffer* buffer
#define LOADER_ARGS buffer
#define LOADER_INHERIT(classname, basename, name) PLUGIN_INHERIT(classname, basename, name, LOADER_CTOR, LOADER_ARGS)

namespace REDasm {

namespace LoaderFlags {
    enum: u32 { None = 0, CustomAssembler = 1, CustomAddressing = 2, Binary = 0xFFFFFFFF};
}

struct LoadRequest
{
    LoadRequest(const std::string& filepath, AbstractBuffer* buffer): filepath(filepath), buffer(buffer), view(buffer->view()) { }
    std::string filepath;
    AbstractBuffer* buffer;
    BufferView view;
};

template<typename T> bool testLoaderPlugin(const LoadRequest& request)
{
    const typename T::HeaderType* header = reinterpret_cast<const typename T::HeaderType*>(request.buffer->data());

    if((sizeof(typename T::HeaderType) > request.buffer->size()) || !T::test(request, header))
        return false;

    return true;
}

template<typename T> LoaderPlugin* initLoaderPlugin(const LoadRequest& request)
{
    T* loaderplugin = new T(request.buffer);
    loaderplugin->load();
    return loaderplugin;
}

class LoaderPlugin: public Plugin
{
    public:
        LoaderPlugin(AbstractBuffer *buffer);
        AbstractBuffer* buffer() const;
        BufferView viewOffset(offset_t offset) const;
        BufferView view(address_t address) const;
        BufferView viewSegment(const Segment* segment) const;
        ListingDocument& createDocument();
        ListingDocument& document();
        const SignatureFiles& signatures() const;
        u64 addressWidth() const;

    public:
        virtual offset_location offset(address_t address) const;
        virtual address_location address(offset_t offset) const;
        virtual Analyzer *createAnalyzer(DisassemblerAPI* disassembler, const SignatureFiles &signatures) const;
        virtual std::string assembler() const = 0;
        virtual u32 bits() const = 0;
        virtual void load() = 0;

    public:
        template<typename U> inline offset_location fileoffset(const U* ptr) const { return REDasm::make_location<offset_t>(reinterpret_cast<const u8*>(ptr) - reinterpret_cast<const u8*>(m_buffer->data()), m_view.inRange(ptr)); }
        template<typename U> inline address_location addressof(const U* ptr) const { return m_view.inRange(ptr) ? this->address(this->fileoffset(ptr)) : REDasm::invalid_location<address_t>();  }
        template<typename U, typename O> inline U* pointer(O offset) const { return m_view.inRange(offset) ? reinterpret_cast<U*>(reinterpret_cast<u8*>(m_buffer->data()) + offset) : nullptr; }
        template<typename U, typename A> inline U* addrpointer(A address) const { auto o = offset(address); return o ? reinterpret_cast<U*>(reinterpret_cast<u8*>(m_buffer->data()) + o) : nullptr; }
        template<typename U, typename V, typename O> inline static const U* relpointer(const V* base, O offset) { return reinterpret_cast<const U*>(reinterpret_cast<const u8*>(base) + offset); }
        template<typename U, typename V, typename O> inline static U* relpointer(V* base, O offset) { return reinterpret_cast<U*>(reinterpret_cast<u8*>(base) + offset); }

    protected:
        std::unique_ptr<AbstractBuffer> m_buffer;
        BufferView m_view;
        ListingDocument m_document;
        SignatureFiles m_signatures;
};

template<typename T> class LoaderPluginT: public LoaderPlugin
{
    public:
        typedef T HeaderType;

    public:
        LoaderPluginT(AbstractBuffer* buffer): LoaderPlugin(buffer) { m_header = reinterpret_cast<T*>(m_buffer->data()); }
        static bool test(const T* loader, const AbstractBuffer* buffer) { RE_UNUSED(loader); RE_UNUSED(buffer); return false; }

    protected:
        T* m_header;
};

class LoaderPluginB: public LoaderPluginT<u8>
{
    DEFINE_LOADER_PLUGIN_TEST(u8);

    public:
        LoaderPluginB(AbstractBuffer* buffer): LoaderPluginT<u8>(buffer) { }
};

struct LoaderPlugin_Entry
{
    std::function<bool(const LoadRequest&)> test;
    std::function<LoaderPlugin*(const LoadRequest&)> init;
    std::function<std::string()> name;
    std::function<u32()> flags;
};

}

#endif // LOADERPLUGIN_H
