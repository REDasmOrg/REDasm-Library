#pragma once

#include <unordered_map>
#include <rdapi/types.h>
#include "../containers/sortedcontainer.h"

typedef SortedContainer<rd_address, std::less<rd_address>, std::equal_to<rd_address>, true> AddressContainer;

struct InstructionNetNode {
    AddressContainer prev;
    rd_address next{RD_NPOS};

    AddressContainer from;
    AddressContainer truejumps, falsejumps;
    AddressContainer calls;
};

class DocumentNet
{
    public:
        DocumentNet() = default;
        void linkNext(rd_address fromaddress, rd_address toaddress);
        void unlinkNext(rd_address fromaddress);
        void linkJump(rd_address fromddress, rd_address toaddress, bool condition);
        void unlinkJump(rd_address fromaddress, rd_address toaddress);
        void linkCall(rd_address fromaddress, rd_address toaddress);
        void unlinkCall(rd_address fromaddress, rd_address toaddress);
        const InstructionNetNode* findNode(rd_address address) const;

    private:
        std::unordered_map<rd_address, InstructionNetNode> m_netnodes;
};

