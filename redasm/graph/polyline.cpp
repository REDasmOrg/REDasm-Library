#include "polyline.h"
#include <impl/graph/polyline_impl.h>

namespace REDasm {

Polyline::Polyline(): m_pimpl_p(new PolylineImpl()) { }
size_t Polyline::size() const { PIMPL_P(const Polyline); return p->size(); }
bool Polyline::empty() const { PIMPL_P(const Polyline); return p->empty(); }
const Polyline::Point &Polyline::p(size_t idx) const { PIMPL_P(const Polyline); return p->p(idx); }
void Polyline::p(const Polyline::Point &pt) { PIMPL_P(Polyline); p->p(pt); }
void Polyline::p(int x, int y) { PIMPL_P(Polyline); p->p({x, y}); }

} // namespace REDasm
