#pragma once

#include <unordered_map>
#include <rdapi/types.h>
#include "../engine/algorithm/emulateresult.h"
#include "../containers/sortedcontainer.h"
#include "../object.h"

typedef SortedContainer<rd_address, std::equal_to<rd_address>, std::less<rd_address>, true> AddressContainer;

struct DocumentNetNode {
    AddressContainer prev;
    rd_address next{RD_NVAL};

    u64 syscall{0};
    rd_type branchtype{EmulateResult::None};

    AddressContainer from;
    AddressContainer branchestrue, branchesfalse;
    AddressContainer calls;
};

class DocumentNet: public Object
{
    private:
        typedef SortedContainer<rd_address, std::equal_to<rd_address>, std::less<rd_address>, true> References;
        typedef std::unordered_map<rd_address, References> ReferencesMap;

    public:
        DocumentNet() = default;
        void linkSysCall(rd_address address, u64 syscall);
        void linkBranchIndirect(rd_address address);
        void linkBranchUnresolved(rd_address address);
        void linkNext(rd_address fromaddress, rd_address toaddress);
        void linkBranch(rd_address fromaddress, rd_address toaddress, rd_type type);
        void linkCall(rd_address fromaddress, rd_address toaddress);
        void addRef(rd_address fromaddress, rd_address toaddress);
        void unlinkNext(rd_address fromaddress);
        void unlinkBranch(rd_address fromaddress, rd_address toaddress);
        void unlinkCall(rd_address fromaddress, rd_address toaddress);
        void removeRef(rd_address fromaddress, rd_address toaddress);
        const DocumentNetNode* findNode(rd_address address) const;
        size_t getReferences(rd_address address, const rd_address** refs) const;

    public:
        static bool isConditional(const DocumentNetNode* n);
        static bool isBranch(const DocumentNetNode* n);
        static bool isCall(const DocumentNetNode* n);

    private:
        std::unordered_map<rd_address, DocumentNetNode> m_netnodes;
        ReferencesMap m_refs;
};

