#include "object.h"
#include "../context.h"

namespace REDasm {

bool Object::objectIs(object_id_t id) const { return this->objectId() == id; }
const char* Object::objectName() const { return Object::NAME; }
object_id_t Object::objectId() const { return Object::ID; }
void Object::save(cereal::BinaryOutputArchive &a) const { r_ctx->log("Cannot serialize " + String(this->objectName()).quoted()); }
void Object::load(cereal::BinaryInputArchive &a) { r_ctx->log("Cannot deserialize " + String(this->objectName()).quoted()); }
void Object::release() { delete this; }

} // namespace REDasm
