#include "plugin_types.h"

namespace REDasm {

void Object::release() { delete this; }

} // namespace REDasm
