#pragma once

#include <rdapi/types.h>
#include "../object.h"

class AbstractBuffer: public Object
{
    public:
        AbstractBuffer() = default;
        u8 at(size_t offset) const;
        const u8* data() const;
        const u8* endData() const;
        bool contains(const u8* ptr) const;
        bool empty() const;
        void fill(u8 val);

    public:
        virtual size_t size() const = 0;
        virtual u8* data() = 0;
};
