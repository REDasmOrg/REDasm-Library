#pragma once

#include <unordered_map>
#include <functional>
#include <optional>
#include <string>
#include <deque>
#include <rdapi/document/document.h>
#include "../../containers/addresscontainer.h"
#include "../../containers/treecontainer.h"
#include "../../types/definitions.h"
#include "../../buffer/buffer.h"
#include "../../object.h"
#include "blockcontainer.h"

bool operator ==(const RDSegment& s1, const RDSegment& s2);

class AddressSpace: public Object
{
    public:
        typedef std::deque<std::string> Comments;

    private:
        struct Info {
            bool weak{true};
            u8 indent{0};
            std::string label;
            std::shared_ptr<Type> type, typefield;
            Comments comments;
            rd_flag flags{AddressFlags_None};
        };

    public:
        AddressSpace(Context* ctx);
        bool markUnknown(rd_address address, size_t size);
        bool markExplored(rd_address address, size_t size);
        bool markCode(rd_address address, size_t size);
        bool markData(rd_address address, size_t size);
        bool markString(rd_address address, size_t size);
        inline BlockContainer* findBlocks(rd_address address) const { return const_cast<AddressSpace*>(this)->findBlocks(address); }
        BlockContainer* findBlocks(rd_address address);
        bool findLabel(const std::string& q, rd_address* resaddress) const;
        bool findLabelR(const std::string& q, rd_address* resaddress) const;
        size_t findLabels(const std::string& q, const rd_address** resaddresses) const;
        size_t findLabelsR(const std::string& q, const rd_address** resaddresses) const;

    public:
        const BlockContainer* getBlocks(rd_address address) const;
        const BlockContainer* getBlocksAt(size_t index) const;
        std::optional<std::string> getLabel(rd_address address) const;
        Comments getComments(rd_address address) const;
        rd_address getAddress(const std::string& label) const;
        rd_address firstAddress() const;
        rd_address lastAddress() const;
        size_t size() const;
        size_t data(const rd_address** addresses) const;
        size_t getLabels(const rd_address** addresses) const;
        size_t getLabelsByFlag(rd_flag flag, const rd_address** addresses) const;
        size_t indexOfSegment(const RDSegment* segment) const;
        size_t indexOfSegment(rd_address address) const;
        bool pointerToSegment(const void* ptr, RDSegment* segment) const;
        bool addressToSegment(rd_address address, RDSegment* segment) const;
        bool offsetToSegment(rd_offset offset, RDSegment* segment) const;
        bool addressToBlock(rd_address address, RDBlock* block) const;
        bool indexToSegment(size_t index, RDSegment* segment) const;
        bool addressToView(rd_address address, size_t size, RDBufferView* view) const;
        bool offsetToView(rd_offset offset, size_t size, RDBufferView* view) const;
        bool insert(const RDSegment& segment);
        void setLabel(rd_address address, const std::string& label, rd_flag flags = AddressFlags_None);
        void updateFlags(rd_address address, rd_flag flags, bool set = true);
        bool updateLabel(rd_address address, const std::string& label);
        void addComment(rd_address address, const std::string& s);
        void setComments(rd_address address, const Comments& c);
        bool setTypeField(rd_address address, const Type* type, int indent, const std::string& name);
        void setType(rd_address address, const Type* type, const std::string& label = std::string());
        const Type* getTypeField(rd_address address, int* indent) const;
        const Type* getType(rd_address address) const;
        rd_flag getFlags(rd_address address) const;
        RDLocation offset(rd_address address) const;
        RDLocation address(rd_offset offset) const;
        RDLocation addressof(const void* ptr) const;
        u8* addrpointer(rd_address address) const;
        u8* offspointer(rd_offset offset) const;

    public: // Serialization
        const MemoryBuffer* getBuffer(rd_address address) const;

    public:
        static size_t addressSize(const RDSegment& segment);
        static size_t offsetSize(const RDSegment& segment);
        static bool containsAddress(const RDSegment* segment, rd_address address);
        static bool containsOffset(const RDSegment* segment, rd_offset offset);

    private:
        Info* getInfo(rd_address address);
        mutable std::vector<rd_address> m_result;

    private:
        AddressContainer<Info> m_info;
        AddressContainer<RDSegment> m_segments;
        std::unordered_map<rd_address, MemoryBuffer> m_buffers;
        std::unordered_map<rd_address, BlockContainer> m_blocks;
        std::unordered_map<rd_type, SortedAddresses> m_labelflags;
        std::unordered_map<std::string, rd_address> m_labels;
};
