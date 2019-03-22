#ifndef MSCOFF_LOADER_H
#define MSCOFF_LOADER_H

#include "../../plugins/plugins.h"
#include "mscoff_header.h"

// Common structures
#include "../pe/pe_header.h"
#include "../pe/pe_utils.h"

namespace REDasm {

class MSCOFFLoader : public LoaderPluginT<ImageArchiveHeader>
{
    PLUGIN_NAME("MS Common Object File Format (COFF)")
    DECLARE_LOADER_PLUGIN_TEST(ImageArchiveHeader)

    public:
        MSCOFFLoader(AbstractBuffer *buffer);
        virtual std::string assembler() const;
        virtual void load();

    private:
        template<typename T> const T* getMemberData(const ImageArchiveMemberHeader* memberhdr) const;
        std::string getLongName(const std::string& stroffset);
        void readMember(const ImageArchiveMemberHeader* memberhdr, const std::string &name);
        void readMemberHeaders();

    private:
        const ImageSectionHeader* loadSegments(const ImageFileHeader* fileheader, const std::string& membername);

    private:
        const ImageArchiveMemberHeader *m_firstlinkerhdr, *m_secondlinkerhdr, *m_longnameshdr;
        std::set<u16> m_machines;
};

template<typename T> const T* MSCOFFLoader::getMemberData(const ImageArchiveMemberHeader* memberhdr) const { return relpointer<const T>(memberhdr, sizeof(ImageArchiveMemberHeader)); }

DECLARE_LOADER_PLUGIN(MSCOFFLoader, mscoff)

} // namespace REDasm

#endif // MSCOFF_LOADER_H
