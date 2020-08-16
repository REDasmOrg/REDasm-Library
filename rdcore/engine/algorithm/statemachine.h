#pragma once

#include <deque>
#include <rdapi/disassembler.h>
#include "../../document/document_fwd.h"

class Disassembler;

class StateMachine
{
    public:
        StateMachine(Disassembler* disassembler);
        virtual ~StateMachine() = default;
        size_t pending() const;
        bool hasNext() const;
        void enqueue(rd_address address);
        void schedule(rd_address address);
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

