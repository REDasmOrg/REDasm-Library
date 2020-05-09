#pragma once

#include <list>
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
        void enqueue(address_t address);
        void schedule(address_t address);
        void next();

    private:
        virtual void nextAddress(address_t address) = 0;
        bool getNext(address_t* address);

    protected:
        Disassembler* m_disassembler;
        SafeDocument& m_document;

    private:
        std::list<address_t> m_pending;
};

