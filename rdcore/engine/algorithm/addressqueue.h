#pragma once

#include <deque>
#include <rdapi/disassembler.h>
#include "../../document/document_fwd.h"

class Disassembler;

class AddressQueue
{
    public:
        AddressQueue(Disassembler* disassembler);
        virtual ~AddressQueue() = default;
        size_t pending() const;
        bool hasNext() const;
        virtual void enqueue(rd_address address);
        virtual void schedule(rd_address address);
        void next();

    private:
        virtual void nextAddress(rd_address address) = 0;
        bool getNext(rd_address* address);

    protected:
        Disassembler* m_disassembler;
        SafeDocument& m_document;

    private:
        std::deque<rd_address> m_pending;
};

