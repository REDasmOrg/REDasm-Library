#include "ordinals.h"
#include <impl/support/ordinals_impl.h>

namespace REDasm {

Ordinals::Ordinals(): m_pimpl_p(new OrdinalsImpl()) { }
bool Ordinals::load(const std::string &filepath) { PIMPL_P(Ordinals); return p->load(filepath); }
std::string Ordinals::name(ordinal_t ordinal, const std::string &fallbackprefix) const { PIMPL_P(const Ordinals); return p->name(ordinal, fallbackprefix); }
void Ordinals::clear() { PIMPL_P(Ordinals); p->clear(); }

} // namespace REDasm
