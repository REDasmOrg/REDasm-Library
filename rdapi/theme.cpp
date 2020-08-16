#include "theme.h"
#include <rdcore/context.h>

void RDTheme_Set(rd_type theme, const char* color) { rd_ctx->setTheme(theme, color); }
const char* RDTheme_Get(rd_type theme) { return rd_ctx->getTheme(theme); }
