#pragma once

#include <redasm/graph/polyline.h>
#include <deque>

namespace REDasm {

class PolylineImpl
{
    public:
        PolylineImpl() = default;
        size_t size() const;
        bool empty() const;
        const Polyline::Point& p(size_t idx) const;
        void p(const Polyline::Point& pt);

    private:
        std::deque<Polyline::Point> m_points;
};

} // namespace REDasm
