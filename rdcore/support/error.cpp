#include "error.h"
#include "utils.h"
#include <stdexcept>
#include <rdapi/events.h>

std::string REDasmError::m_message;

REDasmError::REDasmError(const std::string& msg, size_t location)
{
    m_message = msg + " @ " + Utils::hex(location);
    this->notifyAndThrow();
}

REDasmError::REDasmError(const std::string& msg)
{
    m_message = msg;
    this->notifyAndThrow();
}

void REDasmError::notifyAndThrow()
{
    //FIXME: EventDispatcher::enqueue<RDErrorEventArgs>(Event_Error, Context::instance(), m_message.c_str());
    throw std::runtime_error(m_message);
}
