#include "database_impl.h"
#include <redasm/support/utils.h>

namespace REDasm {

String DatabaseImpl::m_lasterror;
RDBHeader DatabaseImpl::m_header = { RDB_SIGNATURE_ARRAY,
                                     RDB_VERSION,
                                     static_cast<u32>(bits_count<size_t>::value) }; // CLang 8.x workaround

} // namespace REDasm
