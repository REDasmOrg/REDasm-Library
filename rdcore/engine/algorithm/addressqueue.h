#pragma once

#include <deque>
#include "../../document/document_fwd.h"
#include "../../object.h"

class Disassembler;

class AddressQueue: public Object
{
    public:
        AddressQueue(Context* ctx);
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
        SafeDocument& m_document;
        DocumentNet* m_net;

    private:
        std::deque<rd_address> m_pending;
};

