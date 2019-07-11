#include "ordinals.h"
#include <impl/support/ordinals_impl.h>

namespace REDasm {

Ordinals::Ordinals(): m_pimpl_p(new OrdinalsImpl()) { }
bool Ordinals::load(const String &filepath) { PIMPL_P(Ordinals); return p->load(filepath); }
String Ordinals::name(ordinal_t ordinal, const String &fallbackprefix) const { PIMPL_P(const Ordinals); return p->name(ordinal, fallbackprefix); }
void Ordinals::clear() { PIMPL_P(Ordinals); p->clear(); }
String Ordinals::ordinal(ordinal_t ord, const String &fallbackprefix) { return OrdinalsImpl::ordinal(ord, fallbackprefix); }

} // namespace REDasm
