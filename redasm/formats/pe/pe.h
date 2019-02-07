#ifndef PE_H
#define PE_H

#include <type_traits>
#include "../../plugins/plugins.h"
#include "pe_header.h"
#include "pe_resources.h"
#include "pe_imports.h"
#include "pe_utils.h"
#include "dotnet/dotnet_header.h"
#include "dotnet/dotnet_reader.h"

#define RVA_POINTER(type, rva)        (pointer<type>(rvaToOffset(rva)))
#define RVA_POINTER_OK(type, rva, ok) (pointer<type>(rvaToOffset(rva, ok)))

namespace REDasm {

template<size_t b> class PeFormat: public FormatPluginT<ImageDosHeader>
{
    DEFINE_FORMAT_PLUGIN_TEST(ImageDosHeader)

    private:
        typedef typename std::conditional<b == 64, u64, u32>::type pe_integer_t;
        typedef typename std::conditional<b == 64, ImageOptionalHeader64, ImageOptionalHeader32>::type ImageOptionalHeader;
        typedef typename std::conditional<b == 64, ImageThunkData64, ImageThunkData32>::type ImageThunkData;
        typedef typename std::conditional<b == 64, ImageTlsDirectory64, ImageTlsDirectory32>::type ImageTlsDirectory;
        typedef typename std::conditional<b == 64, ImageLoadConfigDirectory64, ImageLoadConfigDirectory32>::type ImageLoadConfigDirectory;
        enum PeType { None = 0, DotNet, VisualBasic, Delphi, TurboCpp };

    public:
        PeFormat(AbstractBuffer* buffer);
        virtual std::string name() const;
        virtual std::string assembler() const;
        virtual u32 bits() const;
        virtual Analyzer* createAnalyzer(DisassemblerAPI *disassembler, const SignatureFiles &signatures) const;
        virtual void load();
        const DotNetReader *dotNetReader() const;

    private:
        u64 rvaToOffset(u64 rva, bool *ok = NULL) const;
        void readDescriptor(const ImageImportDescriptor& importdescriptor, pe_integer_t ordinalflag);
        void readTLSCallbacks(const ImageTlsDirectory* tlsdirectory);
        void checkDelphi(const REDasm::PEResources &peresources);
        void checkResources();
        void checkDebugInfo();
        ImageCorHeader *checkDotNet();
        void loadDotNet(ImageCor20Header* corheader);
        void loadSymbolTable();
        void loadDefault();
        void loadSections();
        void loadExports();
        void loadImports();
        void loadConfig();
        void loadTLS();

    private:
        std::unique_ptr<DotNetReader> m_dotnetreader;
        ImageDosHeader* m_dosheader;
        ImageNtHeaders* m_ntheaders;
        ImageOptionalHeader* m_optionalheader;
        ImageSectionHeader* m_sectiontable;
        ImageDataDirectory* m_datadirectory;
        pe_integer_t m_petype, m_imagebase, m_sectionalignment, m_entrypoint;
};

typedef PeFormat<32> PeFormat32;
typedef PeFormat<64> PeFormat64;

DECLARE_FORMAT_PLUGIN(PeFormat32, pe32)
DECLARE_FORMAT_PLUGIN(PeFormat64, pe64)

}

#include "pe_impl.h"

#endif // PE_H
