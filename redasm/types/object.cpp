#include "object.h"

namespace REDasm {

bool Object::objectIs(object_id_t id) const { return this->objectId() == id; }
void Object::save(cereal::BinaryOutputArchive &a) const { }
void Object::load(cereal::BinaryInputArchive &a) { }
void Object::release() { delete this; }

} // namespace REDasm
