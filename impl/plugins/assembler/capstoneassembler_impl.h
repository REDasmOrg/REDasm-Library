#pragma once

#include "assembler_impl.h"
#include <redasm/plugins/assembler/capstoneassembler.h>
#include <capstone/capstone.h>

namespace REDasm {

class CapstoneAssemblerImpl : public AssemblerImpl
{
    PIMPL_DECLARE_PUBLIC(CapstoneAssembler)

    public:
        CapstoneAssemblerImpl();
        virtual ~CapstoneAssemblerImpl();
        csh handle() const;
        int arch() const;
        int mode() const;

    public:
        static void free(uintptr_t userdata);

    private:
        void open(int arch, int mode);
        void close();

    private:
        csh m_handle;
        int m_arch, m_mode;
};

} // namespace REDasm
