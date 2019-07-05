#include "factory.h"
#include <impl/types/factory_impl.h>

namespace REDasm {

bool Factory::registerAs(object_id_t id, const Factory::ObjectCreator &cb) { return FactoryImpl::registerAs(id, cb); }
Object *Factory::create(object_id_t id) { return FactoryImpl::create(id); }

} // namespace REDasm
