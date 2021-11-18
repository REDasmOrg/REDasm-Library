#pragma once

#include <optional>
#include <unordered_map>
#include <vector>
#include <memory>
#include <rdapi/document/block.h>
#include "../containers/addresscontainer.h"
#include "../object.h"

class Type;

class AddressDatabase: public Object
{
    private:
        typedef std::vector<std::string> Comments;

        struct Entry {
            bool weak{true};
            u8 indent{0};
            std::string label;
            std::shared_ptr<Type> type, typefield;
            Comments comments;
            rd_flag flags{AddressFlags_None};
        };

    public:
        AddressDatabase(Context* context);

    public: // Labels
        rd_address getAddress(const std::string& label) const;
        std::optional<std::string> getLabel(rd_address address) const;
        size_t getLabels(const rd_address** addresses) const;
        size_t getLabelsByFlag(rd_flag flag, const rd_address** addresses) const;
        void setLabel(rd_address address, const std::string& label, rd_flag flags = AddressFlags_None);
        bool updateLabel(rd_address address, const std::string& label);
        bool findLabel(const std::string& q, rd_address* resaddress) const;
        bool findLabelR(const std::string& q, rd_address* resaddress) const;
        size_t findLabels(const std::string& q, const rd_address** resaddresses) const;
        size_t findLabelsR(const std::string& q, const rd_address** resaddresses) const;

    public: // Flags
        void setAddressAssembler(rd_address address, const std::string& assembler);
        std::optional<std::string> getAddressAssembler(rd_address address) const;
        rd_flag getFlags(rd_address address) const;
        void updateFlags(rd_address address, rd_flag flags, bool set = true);

    public: // Types
        bool setTypeField(rd_address address, const Type* type, int indent, const std::string& name);
        void setType(rd_address address, const Type* type, const std::string& label = std::string());
        const Type* getTypeField(rd_address address, int* indent) const;
        const Type* getType(rd_address address) const;

    public: // Comments
        Comments getComments(rd_address address) const;
        void addComment(rd_address address, const std::string& s);
        void setComments(rd_address address, const Comments& c);

    private:
        Entry* getEntry(rd_address address);

    private:
        mutable std::vector<rd_address> m_result;
        std::unordered_map<rd_address, std::string> m_assemblers;
        std::unordered_map<rd_type, SortedAddresses> m_labelflags;
        std::unordered_map<std::string, rd_address> m_labels;
        AddressContainer<Entry> m_entries;
};
