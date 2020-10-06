#include "object.h"
#include "config.h"

Object::Object(Context* context): m_context(context) { }
Context* Object::context() const { return m_context; }
void Object::log(const std::string& s) const { rd_cfg->log(s); }
void Object::status(const std::string& s) const { rd_cfg->status(s); }
