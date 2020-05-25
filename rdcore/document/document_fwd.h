#pragma once

#include "../support/safe_ptr.h"
#include <rdapi/document/document.h>

class Document;

typedef safe_ptr<Document> SafeDocument;
using s_lock_document = s_locked_safe_ptr<SafeDocument>;
using x_lock_document = x_locked_safe_ptr<SafeDocument>;
