#include "object.h"
#include <rdcore/object.h>

void RDObject_Subscribe(RDObject* obj, void* owner, Callback_Event listener, void* userdata) { CPTR(Object, obj)->subscribe(owner, listener, userdata); }
void RDObject_Unsubscribe(RDObject* obj, void* owner) { CPTR(Object, obj)->unsubscribe(owner); }
void RDObject_Free(RDObject* obj) { delete CPTR(Object, obj); }
