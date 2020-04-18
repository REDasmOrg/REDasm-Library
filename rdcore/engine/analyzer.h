#pragma once

#include <rdapi/types.h>
#include "../disassembler.h"

class Analyzer
{
    public:
        Analyzer(Disassembler* disassembler);
        void analyze();

    private:
        void analyzeDefault();
        void checkFunctions();
        bool findNullSubs(x_lock_document& lock, address_t address);
        void findTrampoline(x_lock_document& lock, address_t address);

    private:
        Disassembler* m_disassembler;
        bool m_loaderdone{false};
};

