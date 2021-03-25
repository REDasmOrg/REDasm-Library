#include "binary.h"
#include "../../object.h"
#include "../../context.h"
#include "../../document/document.h"
#include "../builtin.h"

static const char* test(const RDLoaderRequest*) { return ""; /* Always valid */ }

static bool build(RDContext* ctx, const RDLoaderBuildParams* req)
{
    auto* context = CPTR(Context, ctx);
    auto& doc = context->document();
    size_t sz = context->buffer()->size();

    doc->setSegment("BINARY", req->offset, req->baseaddress, sz, sz, SegmentFlags_CodeData);
    doc->setEntry(req->entrypoint);
    return true;
}

RDEntryLoader loaderEntry_Binary = RD_BUILTIN_ENTRY(binary_builtin, "Binary",
                                                    LoaderFlags_CustomAssembler | LoaderFlags_CustomAddressing,
                                                    &test, nullptr, &build);
