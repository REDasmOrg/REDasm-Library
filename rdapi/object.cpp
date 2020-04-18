#include "object.h"
#include <rdcore/object.h>

void RD_Free(void* obj) { delete CPTR(Object, obj); }
