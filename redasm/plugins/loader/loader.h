#pragma once

#include <unordered_set>
#include "../../disassembler/listing/listingdocument.h"
#include "../../buffer/bufferview.h"
#include "../assembler/assemblerrequest.h"
#include "../plugin.h"

namespace REDasm {

class LoadRequestImpl;
class LoaderImpl;
class Disassembler;
class Analyzer;

enum class LoaderFlags: u32
{
    None               = 0,
    CustomAssembler    = (1 << 0),
    CustomAddressing   = (1 << 1),
    SkipUnexploredCode = (1 << 2),

    Binary             = 0xFFFFFFFF
};

ENUM_FLAGS_OPERATORS(LoaderFlags)

class LIBREDASM_API LoadRequest
{
    PIMPL_DECLARE_P(LoadRequest)
    PIMPL_DECLARE_PRIVATE(LoadRequest)

    public:
        LoadRequest(const String& filepath, AbstractBuffer* buffer);
        String filePath() const;
        AbstractBuffer* buffer() const;
        const BufferView& view() const;

    public:
        template<typename T> const T* pointer() const { return reinterpret_cast<T*>(this->buffer()->data()); }
};

typedef std::unordered_set<String> SignatureIdentifiers;

class LIBREDASM_API Loader: public Plugin
{
    REDASM_OBJECT(Loader)
    PIMPL_DECLARE_PRIVATE(Loader)

    public:
        Loader();
        AbstractBuffer* buffer() const;
        BufferView viewOffset(offset_t offset) const;
        BufferView viewOffset(offset_t offset, size_t size) const;
        const BufferView& view() const;
        BufferView view(address_t address) const;
        BufferView view(address_t address, size_t size) const;
        BufferView viewSegment(const Segment* segment) const;
        ListingDocument& createDocument();
        const ListingDocument& document() const;
        ListingDocument& document();
        SignatureIdentifiers &signatures();
        Loader* signature(const String& sig);
        Analyzer* analyzer();
        u8* data() const;
        template<typename T> const T* pointer() const { return reinterpret_cast<T*>(this->buffer()->data()); }

    public:
        virtual LoaderFlags flags() const;
        virtual offset_location offset(address_t address) const;
        virtual address_location address(offset_t offset) const;
        virtual address_location reladdress(address_t absaddress) const;
        virtual address_location absaddress(address_t reladdress) const;
        virtual AssemblerRequest assembler() const;
        virtual void build(const String &assembler, offset_t offset, address_t baseaddress, address_t entrypoint);
        virtual void init(const LoadRequest& request);
        virtual bool test(const LoadRequest& request) const = 0;
        virtual void load() = 0;

    protected:
        virtual Analyzer* createAnalyzer() const;

    public:
        template<typename U> inline offset_location fileoffset(const U* ptr) const { return REDasm::make_location<offset_t>(reinterpret_cast<const u8*>(ptr) - reinterpret_cast<const u8*>(this->buffer()->data()), this->view().inRange(ptr)); }
        template<typename U> inline address_location addressof(const U* ptr) const { return this->view().inRange(ptr) ? this->address(this->fileoffset(ptr)) : REDasm::invalid_location<address_t>();  }
        template<typename U> inline U* pointer(offset_t offset) const { return this->view().inRange(offset) ? reinterpret_cast<U*>(reinterpret_cast<u8*>(this->buffer()->data()) + offset) : nullptr; }
        template<typename U> inline U* addrpointer(address_t address) const { auto o = offset(address); return o ? reinterpret_cast<U*>(reinterpret_cast<u8*>(this->buffer()->data()) + o) : nullptr; }
        template<typename U, typename V, typename O> inline static const U* relpointer(const V* base, O offset) { return reinterpret_cast<const U*>(reinterpret_cast<const u8*>(base) + offset); }
        template<typename U, typename V, typename O> inline static U* relpointer(V* base, O offset) { return reinterpret_cast<U*>(reinterpret_cast<u8*>(base) + offset); }
};

} // namespace REDasm
