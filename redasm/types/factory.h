#pragma once

#include "base.h"
#include "../pimpl.h"

namespace REDasm {

class Object;

class Factory
{
    public:
        typedef Object* (*ObjectCreator)();

    private:
        Factory() = default;

    public:
        static bool registerAs(object_id_t id, const ObjectCreator& cb);
        static Object* create(object_id_t id);
};

} // namespace REDasm
