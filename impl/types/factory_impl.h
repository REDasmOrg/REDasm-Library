#pragma once

#include <unordered_map>
#include <redasm/types/factory.h>

namespace REDasm {

class FactoryImpl
{
    private:
        typedef std::unordered_map<object_id_t, Factory::ObjectCreator> ObjectCreators;

    public:
        FactoryImpl() = default;
        static bool registerAs(object_id_t id, const Factory::ObjectCreator& cb);
        static Object* create(object_id_t id);

    private:
        static ObjectCreators& creators();

};

} // namespace REDasm
