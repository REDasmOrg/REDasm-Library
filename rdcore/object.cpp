#include "object.h"
#include "config.h"

Object::Object(Context* context): m_context(context) { }
Context* Object::context() const { return m_context; }
void Object::setContext(Context* ctx) { m_context = ctx; }
void Object::log(const std::string& s) const { rd_cfg->log(s); }
void Object::status(const std::string& s) const { rd_cfg->status(s); }
void Object::statusAddress(const std::string& s, rd_address address) const { rd_cfg->statusAddress(s.c_str(), address); }
void Object::subscribe(void* owner, const SubscribedListener& listener, void* userdata) { m_listeners[owner] = { listener, userdata }; }
void Object::unsubscribe(void* owner) { m_listeners.erase(owner); }
