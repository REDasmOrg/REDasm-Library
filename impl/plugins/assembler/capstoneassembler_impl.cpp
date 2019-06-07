#include "capstoneassembler_impl.h"

namespace REDasm {

CapstoneAssemblerImpl::CapstoneAssemblerImpl(int arch, int mode): AssemblerImpl()
{
    cs_open(static_cast<cs_arch>(arch), static_cast<cs_mode>(mode), &m_handle);
    cs_option(m_handle, CS_OPT_DETAIL, CS_OPT_ON);
}

CapstoneAssemblerImpl::~CapstoneAssemblerImpl()
{
    if(m_handle)
        cs_close(&m_handle);

    m_handle = 0;
}

csh CapstoneAssemblerImpl::handle() const { return m_handle; }
void CapstoneAssemblerImpl::free(void *userdata) { cs_free(reinterpret_cast<cs_insn*>(userdata), 1); }

} // namespace REDasm
