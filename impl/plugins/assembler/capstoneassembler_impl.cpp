#include "capstoneassembler_impl.h"

namespace REDasm {

CapstoneAssemblerImpl::CapstoneAssemblerImpl(): AssemblerImpl(), m_handle(0), m_arch(0), m_mode(0) { }
CapstoneAssemblerImpl::~CapstoneAssemblerImpl() { this->close(); }
csh CapstoneAssemblerImpl::handle() const { return m_handle; }
int CapstoneAssemblerImpl::arch() const { return m_arch; }
int CapstoneAssemblerImpl::mode() const { return m_mode; }
void CapstoneAssemblerImpl::free(uintptr_t userdata) { cs_free(reinterpret_cast<cs_insn*>(userdata), 1); }

void CapstoneAssemblerImpl::open(int arch, int mode)
{
    this->close();

    m_arch = arch;
    m_mode = mode;

    cs_open(static_cast<cs_arch>(arch), static_cast<cs_mode>(mode), &m_handle);
    cs_option(m_handle, CS_OPT_DETAIL, CS_OPT_ON);
}

void CapstoneAssemblerImpl::close()
{
    if(m_handle)
        cs_close(&m_handle);

    m_handle = 0;
}

} // namespace REDasm
