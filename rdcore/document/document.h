#pragma once

#include "document_fwd.h"
#include "documentnet.h"
#include "model/functioncontainer.h"
#include "model/addressspace.h"
#include "../engine/stringfinder.h"
#include "../types/definitions.h"
#include "../object.h"
#include <rdapi/support/utils.h>
#include <functional>
#include <optional>

#define COMMENT_SEPARATOR  " | "

class Document: public Object
{
    public:
        typedef std::function<bool(rd_address, rd_address, size_t)> TableCallback;

    public:
        Document(const MemoryBufferPtr& buffer, Context* ctx);
        rd_endianness endianness() const;
        void setEndianness(rd_endianness endianness);
        inline const DocumentNet* net() const { return const_cast<Document*>(this)->net(); }
        MemoryBuffer* buffer() const;
        DocumentNet* net();

    public: // Setters
        bool setCode(rd_address address, size_t size);
        bool setData(rd_address address, size_t size, const std::string& label = std::string());
        bool setLocation(rd_address address);
        bool setUnknown(rd_address address, size_t size);
        bool setSegment(const std::string& name, rd_offset offset, rd_address address, size_t psize, size_t vsize, rd_flag flags);
        void setFunction(rd_address address, const std::string& label = std::string());
        bool setString(rd_address address, size_t size, rd_flag flags, const std::string& label = std::string());
        void setBranch(rd_address address, int direction = 1);
        void setLabel(rd_address address, rd_flag flags = AddressFlags_None, const std::string& label = std::string());
        void setPointer(rd_address address, const std::string& label = std::string());
        bool setTypeName(rd_address address, const std::string& q);
        bool setType(rd_address address, const Type* type);
        void setExportedFunction(rd_address address, const std::string& label = std::string());
        void setExported(rd_address address, size_t size, const std::string& label = std::string());
        void setImported(rd_address address, size_t size, const std::string& label = std::string());
        void setEntry(rd_address address);
        void setGraph(FunctionGraph* g);
        void setComments(rd_address address, const std::string& s);
        void addComment(rd_address address, const std::string& s);
        bool createFunction(rd_address address, const std::string& label);
        bool updateLabel(rd_address address, const std::string& s);

    public: // Getters
        const char* readString(rd_address address, size_t* len) const;
        const char16_t* readWString(rd_address address, size_t* len) const;
        std::string readString(rd_address address, size_t len = RD_NVAL) const;  // Internal C++ Helper
        std::string readWString(rd_address address, size_t len = RD_NVAL) const; // Internal C++ Helper
        const AddressSpace* addressSpace() const;
        const BlockContainer* getBlocks(rd_address address) const;
        RDLocation getEntry() const;
        bool pointerToSegment(const void* ptr, RDSegment* segment) const;
        bool addressToSegment(rd_address address, RDSegment* segment) const;
        bool offsetToSegment(rd_address address, RDSegment* segment) const;
        bool addressToBlock(rd_address address, RDBlock* block) const;
        bool getBlockView(rd_address address, RDBufferView* view) const;
        bool getView(rd_address address, size_t size, RDBufferView* view) const;
        const Type* getTypeField(rd_address address, int* indent) const;
        const Type* getType(rd_address address) const;
        std::string getComments(rd_address address) const;
        rd_flag getFlags(rd_address address) const;
        size_t getLabels(const rd_address** addresses) const;
        size_t getLabelsByFlag(rd_flag flag, const rd_address** addresses) const;
        size_t getSegments(const rd_address** addresses) const;
        size_t getFunctions(const rd_address** addresses) const;
        size_t getFunctionInstrCount(rd_address address) const;
        rd_address getAddress(const std::string& label) const;
        rd_address firstAddress() const;
        rd_address lastAddress() const;
        std::optional<std::string> getLabel(rd_address address) const;
        RDLocation offset(rd_address address) const;
        RDLocation address(rd_offset offset) const;
        RDLocation addressof(const void* ptr) const;
        RDLocation fileoffset(const void* ptr) const;
        u8* filepointer(rd_offset offset) const;
        u8* addrpointer(rd_address address) const;
        u8* offspointer(rd_offset offset) const;
        bool isAddress(rd_address address) const;
        bool isBasicBlockTail(rd_address address) const;

    public:
        bool findLabel(const std::string& q, rd_address* resaddress) const;
        bool findLabelR(const std::string& q, rd_address* resaddress) const;
        size_t findLabels(const std::string& q, const rd_address** resaddresses) const;
        size_t findLabelsR(const std::string& q, const rd_address** resaddresses) const;
        rd_address checkLocation(rd_address fromaddress, rd_address address, size_t size = RD_NVAL);
        void checkTypeName(rd_address fromaddress, rd_address address, const char* q);
        void checkType(rd_address fromaddress, rd_address address, const Type* t);
        void checkString(rd_address fromaddress, rd_address address, size_t size = RD_NVAL);
        size_t checkTable(rd_address fromaddress, rd_address address, size_t size, const TableCallback& cb);
        bool checkPointer(rd_address fromaddress, rd_address address, size_t size, rd_address* firstaddress);
        FunctionGraph* getGraph(rd_address address) const;
        RDLocation getFunctionStart(rd_address address) const;
        std::string getHexDump(rd_address address, size_t size) const;
        RDLocation dereference(rd_address address) const;
        void invalidateGraphs();

    private:
        bool setTypeFields(rd_address address, const Type* type, int level);
        bool readAddress(rd_address address, u64 *value) const;
        RDLocation dereferenceAddress(rd_address address) const;
        size_t checkString(rd_address address, rd_flag* resflags);

    public:
        static std::string makeLabel(rd_address address, const std::string& prefix);

    private:
        template<typename T> const T* readStringT(rd_address address, size_t* len) const;

    private:
        rd_endianness m_endianness{Endianness_Little};
        RDLocation m_entry{ };
        MemoryBufferPtr m_buffer;
        FunctionContainer m_functions;
        AddressSpace m_addressspace;
        DocumentNet m_net;
};

template<typename T>
const T* Document::readStringT(rd_address address, size_t* len) const {
    size_t clen = 0, maxlen = RD_NVAL;
    if(len && *len) maxlen = *len;

    RDBufferView view;
    if(!this->getView(address, maxlen, &view)) return nullptr;
    const T* pstart = reinterpret_cast<T*>(view.data);

    for(size_t i = 0 ; (i < maxlen) && !BufferView::empty(&view); i++, BufferView::advance(&view, sizeof(T))) {
        if(!StringFinder::toAscii(*reinterpret_cast<T*>(view.data), nullptr)) break;
        clen++;
    }

    if(len) *len = clen;
    return clen ? reinterpret_cast<const T*>(pstart) : nullptr;
}
