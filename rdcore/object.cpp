#include "object.h"

Object::Object(EventDispatcher* dispatcher): m_dispatcher(dispatcher) { }
EventDispatcher* Object::dispatcher() const { return m_dispatcher; }
