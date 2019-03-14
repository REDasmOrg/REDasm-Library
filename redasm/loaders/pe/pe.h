#ifndef PELOADER_H
#define PELOADER_H

#include <type_traits>
#include "../../plugins/plugins.h"
#include "pe_header.h"
#include "pe_resources.h"
#include "pe_imports.h"
#include "pe_utils.h"
#include "dotnet/dotnet_header.h"
#include "dotnet/dotnet_reader.h"

namespace REDasm {

template<size_t b> class PELoader: public LoaderPluginT<ImageDosHeader>
{
    DEFINE_LOADER_PLUGIN_TEST(ImageDosHeader)

    private:
        typedef typename std::conditional<b == 64, u64, u32>::type pe_integer_t;
        typedef typename std::conditional<b == 64, ImageOptionalHeader64, ImageOptionalHeader32>::type ImageOptionalHeader;
        typedef typename std::conditional<b == 64, ImageThunkData64, ImageThunkData32>::type ImageThunkData;
        typedef typename std::conditional<b == 64, ImageTlsDirectory64, ImageTlsDirectory32>::type ImageTlsDirectory;
        typedef typename std::conditional<b == 64, ImageLoadConfigDirectory64, ImageLoadConfigDirectory32>::type ImageLoadConfigDirectory;

    public:
        PELoader(AbstractBuffer* buffer);
        virtual std::string name() const;
        virtual std::string assembler() const;
        virtual u32 bits() const;
        virtual Analyzer* createAnalyzer(DisassemblerAPI *disassembler, const SignatureFiles &signatures) const;
        virtual void load();
        const DotNetReader *dotNetReader() const;
        address_t rvaToVa(address_t rva) const;
        address_t vaToRva(address_t rva) const;

    private:
        offset_location rvaToOffset(u64 rva) const;
        void readDescriptor(const ImageImportDescriptor& importdescriptor, pe_integer_t ordinalflag);
        void readTLSCallbacks(const ImageTlsDirectory* tlsdirectory);
        void checkPeTypeHeuristic();
        void checkDelphi(const REDasm::PEResources &peresources);
        void checkResources();
        void checkDebugInfo();
        ImageCorHeader *checkDotNet();
        void loadDotNet(ImageCor20Header* corheader);
        void loadSymbolTable();
        void loadDefault();
        void loadSections();
        void loadExports();
        bool loadImports();
        void loadExceptions();
        void loadConfig();
        void loadTLS();

    private:
        template<typename T> T* rvaPointer(u64 rva) const {
            offset_location offset = this->rvaToOffset(rva);

            if(!offset.valid) return nullptr;
            return this->pointer<T>(offset);
        }

    private:
        std::unique_ptr<DotNetReader> m_dotnetreader;
        ImageDosHeader* m_dosheader;
        ImageNtHeaders* m_ntheaders;
        ImageOptionalHeader* m_optionalheader;
        ImageSectionHeader* m_sectiontable;
        ImageDataDirectory* m_datadirectory;
        pe_integer_t m_petype, m_imagebase, m_sectionalignment, m_entrypoint;
        std::unordered_set<std::string> m_validimportsections;
};

typedef PELoader<32> PELoader32;
typedef PELoader<64> PELoader64;

DECLARE_LOADER_PLUGIN(PELoader32, pe32)
DECLARE_LOADER_PLUGIN(PELoader64, pe64)

}

#include "pe_impl.h"

#endif // PELOADER_H
