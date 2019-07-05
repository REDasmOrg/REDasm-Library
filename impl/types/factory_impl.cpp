#include "factory_impl.h"

namespace REDasm {

bool FactoryImpl::registerAs(object_id_t id, const Factory::ObjectCreator &cb)
{
    auto it = FactoryImpl::creators().find(id);

    if(it != FactoryImpl::creators().end())
        return false;

    FactoryImpl::creators()[id] = cb;
    return true;
}

Object *FactoryImpl::create(object_id_t id)
{
    auto it = FactoryImpl::creators().find(id);

    if(it != FactoryImpl::creators().end())
        return it->second();

    return nullptr;
}

FactoryImpl::ObjectCreators &FactoryImpl::creators()
{
    static ObjectCreators* m_creators = new ObjectCreators();
    return *m_creators;
}

} // namespace REDasm
