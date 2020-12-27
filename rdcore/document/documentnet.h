#pragma once

#include <unordered_map>
#include <rdapi/types.h>
#include "../engine/algorithm/emulateresult.h"
#include "../containers/sortedcontainer.h"
#include "../object.h"

typedef SortedContainer<rd_address, std::equal_to<rd_address>, std::less<rd_address>, true> AddressContainer;

struct DocumentNetNode {
    AddressContainer prev;
    rd_address address{RD_NVAL}, next{RD_NVAL};

    u64 syscall{0};
    rd_type branchtype{EmulateResult::None};

    AddressContainer from;
    AddressContainer branchestrue, branchesfalse;
    AddressContainer calls;
};

struct ReferenceComparator {
    bool operator ()(const RDReference& r1, const RDReference& r2) const { return r1.address == r2.address; }
};

struct ReferenceSorter {
    bool operator ()(const RDReference& r1, const RDReference& r2) const { return r1.address < r2.address; }
};

class DocumentNet: public Object
{
    private:
        typedef SortedContainer<RDReference, ReferenceComparator, ReferenceSorter, true> References;
        typedef std::unordered_map<rd_address, References> ReferencesMap;

    public:
        DocumentNet(Context* ctx);
        void linkSysCall(rd_address address, u64 syscall);
        void linkBranchIndirect(rd_address address);
        void linkBranchUnresolved(rd_address address);
        void linkNext(rd_address fromaddress, rd_address toaddress);
        void linkBranch(rd_address fromaddress, rd_address toaddress, rd_type type);
        void linkCall(rd_address fromaddress, rd_address toaddress);
        void addRef(rd_address fromaddress, rd_address toaddress, rd_flag flags = ReferenceFlags_Direct);
        void unlinkNext(rd_address fromaddress);
        void unlinkBranch(rd_address fromaddress, rd_address toaddress);
        void unlinkCall(rd_address fromaddress, rd_address toaddress);
        void removeRef(rd_address fromaddress, rd_address toaddress);
        const DocumentNetNode* findNode(rd_address address) const;
        const DocumentNetNode* prevNode(const DocumentNetNode* n) const;
        const DocumentNetNode* nextNode(const DocumentNetNode* n) const;
        size_t getReferences(rd_address address, const RDReference** refs) const;

    public:
        static bool isConditional(const DocumentNetNode* n);
        static bool isBranch(const DocumentNetNode* n);
        static bool isCall(const DocumentNetNode* n);

    private:
        DocumentNetNode& n(rd_address address);

    private:
        std::unordered_map<rd_address, DocumentNetNode> m_netnodes;
        ReferencesMap m_refs;
};

