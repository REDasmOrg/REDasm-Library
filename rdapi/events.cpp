#include "events.h"
#include <rdcore/eventdispatcher.h>

void RDEvent_Subscribe(void* owner, Callback_Event eventcb, void* userdata) { EventDispatcher::subscribe(owner, eventcb, userdata); }
void RDEvent_Unsubscribe(void* owner) { EventDispatcher::unsubscribe(owner); }
