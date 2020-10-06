#pragma once

#include <rdapi/plugin/command.h>
#include "entry.h"

class Disassembler;

class Command: public Entry<RDEntryCommand>
{
    public:
        Command(RDEntryCommand* entry, Context* d);
};
