#pragma once

#include <deque>
#include <rdapi/types.h>
#include "../../object.h"

class Disassembler;
struct RDBufferView;

class EmulateResult: public Object
{
    public:
        enum {
            None = 0,
            Branch, BranchTrue, BranchFalse, BranchIndirect, BranchUnresolved,
            Return, SysCall, Call, Ref
        };

        union Value { rd_address address; u64 syscall; };

    private:
        typedef std::deque<std::pair<rd_type, Value>> Results;

    public:
        EmulateResult(rd_address address, const RDBufferView* view, Disassembler* disassembler);
        bool canFlow() const;
        const Results& results() const;
        Disassembler* disassembler() const;
        const RDBufferView* view() const;
        rd_address address() const;
        size_t size() const;
        void setSize(size_t size);
        size_t delaySlot() const;
        void setDelaySlot(size_t ds);

    public:
        void addReturn();
        void addBranchUnresolved();
        void addBranchIndirect();
        void addBranch(rd_address address);
        void addBranchTrue(rd_address address);
        void addBranchFalse(rd_address address);
        void addSysCall(u64 n);
        void addCall(rd_address address);
        void addRef(rd_address address);

    private:
        bool m_canflow{true};
        rd_address m_address;
        size_t m_size{0}, m_delayslot{0};
        Disassembler* m_disassembler;
        const RDBufferView* m_view;
        Results m_results;
};

