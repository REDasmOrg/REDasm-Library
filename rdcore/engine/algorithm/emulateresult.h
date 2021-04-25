#pragma once

#include <deque>
#include <rdapi/types.h>
#include "../../types/type.h"
#include "../../object.h"

struct RDBufferView;

class EmulateResult: public Object
{
    public:
        enum {
            None = 0,
            Invalid,
            Branch, BranchTrue, BranchFalse, BranchTable, BranchIndirect, BranchUnresolved,
            Call, CallTable, CallIndirect, CallUnresolved,
            Return, SysCall, Ref, RefString, RefType, RefTypeName, Table,
        };

        struct Value {
            union {
                uintptr_t v1;
                rd_address address;
                u64 syscall;
            };

            union {
                uintptr_t v2;
                size_t size;
                const char* name;
            };

            std::shared_ptr<Type> type;
        };

    private:
        typedef std::deque<std::pair<rd_type, Value>> Results;

    public:
        EmulateResult(rd_address address, const RDBufferView* view);
        bool invalid() const;
        bool canFlow() const;
        const Results& results() const;
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
        void addBranchTable(rd_address address, size_t size);
        void addSysCall(u64 n);
        void addCall(rd_address address);
        void addCallIndirect();
        void addCallUnresolved();
        void addCallTable(rd_address address, size_t size);
        void addReference(rd_address address);
        void addReferenceSize(rd_address address, size_t size);
        void addString(rd_address address);
        void addStringSize(rd_address address, size_t size);
        void addTable(rd_address address, size_t size);
        void addType(rd_address address, const Type* t);
        void addTypeName(rd_address address, const char* name);
        void addInvalid(size_t size);

    private:
        bool m_canflow{true}, m_invalid{false};
        rd_address m_address;
        size_t m_size{0}, m_delayslot{0};
        const RDBufferView* m_view;
        Results m_results;
};

