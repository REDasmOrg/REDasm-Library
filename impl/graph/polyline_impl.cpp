#include "polyline_impl.h"

namespace REDasm {

size_t PolylineImpl::size() const { return m_points.size(); }
bool PolylineImpl::empty() const { return m_points.empty(); }
const Polyline::Point &PolylineImpl::p(size_t idx) const { return m_points.at(idx); }
void PolylineImpl::p(const Polyline::Point& pt) { m_points.push_back(pt); }

} // namespace REDasm
