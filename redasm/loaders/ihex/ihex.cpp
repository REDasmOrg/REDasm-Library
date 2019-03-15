#include "ihex.h"
#include "ihex_parser.h"

namespace REDasm {

LOADER_PLUGIN_TEST(IHexLoader, u8)
{
    IHexParser parser(request.view);
    return parser.test();
}

IHexLoader::IHexLoader(AbstractBuffer *buffer): LoaderPluginB(buffer)
{

}

u32 IHexLoader::bits() const
{
    return 32;
}

void IHexLoader::load()
{

}

} // namespace REDasm
