#pragma once

#include "../object.h"
#include "../buffer/view.h"
#include <rdapi/assembler.h>

class Assembler: public Object
{
    public:
        Assembler(const RDAssemblerPlugin* descriptor);
        bool decode(BufferView* bufferview, RDInstruction* instruction);
        size_t addressWidth() const;
        size_t bits() const;

    private:
        const RDAssemblerPlugin* m_plugin;
};
