#pragma once

#include "../types/object.h"
#include "../macros.h"

namespace REDasm {

class PolylineImpl;

class Polyline: public Object
{
    REDASM_OBJECT(Polyline)
    PIMPL_DECLARE_P(Polyline)
    PIMPL_DECLARE_PRIVATE(Polyline)

    public:
        struct Point { int x, y; };

    public:
        Polyline();
        size_t size() const;
        bool empty() const;
        const Point& p(size_t idx) const;
        void p(const Point& pt);
        void p(int x, int y);
};

} // namespace REDasm
