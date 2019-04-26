#pragma once

// https://docs.microsoft.com/en-us/windows/desktop/Debug/pe-format#archive-library-file-format

#include "../../plugins/plugins.h"
#include "../../support/relocation.h"
#include "../../support/coff/coff_symboltable.h"
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
        struct FunctionItem { std::string name; offset_t start; u64 size; const ImageSectionHeader* section; };
        typedef std::unordered_map<const ImageSectionHeader*, RelocationList> RelocationMap;
        typedef std::forward_list<FunctionItem> FunctionList;

    public:
        MSCOFFLoader(AbstractBuffer *buffer);
        const FunctionList& functions() const;
        const RelocationList *relocations(const ImageSectionHeader* section) const;
        virtual std::string assembler() const;
        virtual void load();

    private:
        template<typename T> const T* getMemberData(const ImageArchiveMemberHeader* memberhdr) const;
        const ImageSectionHeader* loadSegments(const ImageFileHeader* fileheader, const std::string& membername);
        std::string getLongName(const std::string& stroffset);
        void readMember(const ImageArchiveMemberHeader* memberhdr, const std::string &name);
        void readMemberHeaders();

    private:
        void readRelocations(const ImageSectionHeader* sectionheader, const ImageFileHeader *fileheader);
        void readRelocations_x86_32(const ImageSectionHeader* sectionheader, const ImageRelocation* relocations);
        void readRelocations_x86_64(const ImageSectionHeader* sectionheader, const ImageRelocation* relocations);
        void readRelocations_arm(const ImageSectionHeader* sectionheader, const ImageRelocation* relocations);

    private:
        const ImageArchiveMemberHeader *m_firstlinkerhdr, *m_secondlinkerhdr, *m_longnameshdr;
        RelocationMap m_relocations;
        FunctionList m_functions;
        std::set<u16> m_machines;
};

template<typename T> const T* MSCOFFLoader::getMemberData(const ImageArchiveMemberHeader* memberhdr) const { return relpointer<const T>(memberhdr, sizeof(ImageArchiveMemberHeader)); }

DECLARE_LOADER_PLUGIN(MSCOFFLoader, mscoff)

} // namespace REDasm
