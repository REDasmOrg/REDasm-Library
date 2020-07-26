#pragma once

#include <rdapi/types.h>

class Disassembler;

class ILFunction
{
    public:
        ILFunction(Disassembler* disassembler, rd_address address);

    private:
        Disassembler* m_disassembler;
};
