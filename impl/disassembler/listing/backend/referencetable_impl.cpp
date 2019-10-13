#include "referencetable_impl.h"
#include "../../../libs/cereal/cereal.hpp"
#include "../../../libs/cereal/types/set.hpp"
#include "../../../libs/cereal/types/unordered_map.hpp"
#include "../../../libs/cereal/archives/binary.hpp"

namespace REDasm {

void ReferenceTableImpl::save(cereal::BinaryOutputArchive &a) const { a(m_references, m_targets); }
void ReferenceTableImpl::load(cereal::BinaryInputArchive &a) { a(m_references, m_targets); }

} // namespace REDasm
