#include "events.h"
#include <rdcore/eventdispatcher.h>

event_t RDEvent_Subscribe(event_id_t id, RD_EventCallback eventcb, void* userdata) { return EventDispatcher::subscribe(id, eventcb, userdata); }
void RDEvent_Unsubscribe(event_t event) { EventDispatcher::unsubscribe(event); }
void RDEvent_UnsubscribeAll() { EventDispatcher::unsubscribeAll(); }
