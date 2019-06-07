#pragma once

#include "assembler_impl.h"
#include <redasm/plugins/assembler/capstoneassembler.h>
#include <capstone/capstone.h>

namespace REDasm {

class CapstoneAssemblerImpl : public AssemblerImpl
{
    PIMPL_DECLARE_PUBLIC(CapstoneAssembler)

    public:
        CapstoneAssemblerImpl(int arch, int mode);
        virtual ~CapstoneAssemblerImpl();
        csh handle() const;

    public:
        static void free(void* userdata);

    private:
        csh m_handle;
};

} // namespace REDasm
