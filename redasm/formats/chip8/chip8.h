#ifndef CHIP8_FORMAT_H
#define CHIP8_FORMAT_H

#include "../../plugins/plugins.h"

namespace REDasm {

class CHIP8Format: public FormatPluginB
{
    public:
        CHIP8Format(AbstractBuffer* buffer);
        virtual std::string name() const;
        virtual std::string assembler() const;
        virtual u32 bits() const;
        virtual void load();
};

DECLARE_FORMAT_PLUGIN(CHIP8Format, chip8)

} // namespace REDasm

#endif // CHIP8_FORMAT_H
