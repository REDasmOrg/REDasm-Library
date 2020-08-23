#include "binary.h"
#include "../../object.h"
#include "../../plugin/loader.h"
#include "../builtin.h"

static const char* test(const struct RDLoaderPlugin*, const RDLoaderRequest*) { return ""; /* Always valid */ }

static bool build(RDLoaderPlugin*, RDLoader* ldr, const RDLoaderBuildRequest* req)
{
    auto* loader = CPTR(Loader, ldr);
    auto& doc = loader->document();
    size_t sz = loader->buffer()->size();

    doc->segment("BINARY", req->offset, req->baseaddress, sz, sz, SegmentFlags_CodeData);
    doc->entry(req->entrypoint);
    return true;
}

RDLoaderPlugin loader_Binary = RD_BUILTIN_PLUGIN(binary_builtin, "Binary",
                                                 LoaderFlags_CustomAssembler | LoaderFlags_CustomAddressing,
                                                 &test, nullptr, &build);
