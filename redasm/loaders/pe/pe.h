#pragma once

#include <type_traits>
#include "../../plugins/plugins.h"
#include "pe_header.h"
#include "pe_resources.h"
#include "pe_imports.h"
#include "pe_utils.h"
#include "pe_utils.h"
#include "pe_classifier.h"
#include "dotnet/dotnet_header.h"
#include "dotnet/dotnet_reader.h"

namespace REDasm {

template<size_t b> class PELoader: public LoaderPluginT<ImageDosHeader>
{
    DECLARE_LOADER_PLUGIN_TEST(ImageDosHeader)

    private:
        typedef typename std::conditional<b == 64, u64, u32>::type pe_integer_t;
        typedef typename std::conditional<b == 64, ImageOptionalHeader64, ImageOptionalHeader32>::type ImageOptionalHeader;
        typedef typename std::conditional<b == 64, ImageThunkData64, ImageThunkData32>::type ImageThunkData;
        typedef typename std::conditional<b == 64, ImageTlsDirectory64, ImageTlsDirectory32>::type ImageTlsDirectory;
        typedef typename std::conditional<b == 64, ImageLoadConfigDirectory64, ImageLoadConfigDirectory32>::type ImageLoadConfigDirectory;

    public:
        PELoader(AbstractBuffer* buffer);
        virtual std::string assembler() const;
        virtual Analyzer* createAnalyzer(DisassemblerAPI *disassembler) const;
        virtual void load();
        const DotNetReader *dotNetReader() const;
        address_t rvaToVa(address_t rva) const;
        address_t vaToRva(address_t rva) const;

    private:
        ImageCorHeader *checkDotNet();
        void readDescriptor(const ImageImportDescriptor& importdescriptor, pe_integer_t ordinalflag);
        void readTLSCallbacks(const ImageTlsDirectory* tlsdirectory);
        void checkResources();
        void checkDebugInfo();
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
            offset_location offset = PEUtils::rvaToOffset(m_ntheaders, rva);

            if(!offset.valid) return nullptr;
            return this->pointer<T>(offset);
        }

    private:
        PEClassifier m_classifier;
        std::unique_ptr<DotNetReader> m_dotnetreader;
        ImageDosHeader* m_dosheader;
        ImageNtHeaders* m_ntheaders;
        ImageOptionalHeader* m_optionalheader;
        ImageSectionHeader* m_sectiontable;
        ImageDataDirectory* m_datadirectory;
        pe_integer_t m_imagebase, m_sectionalignment, m_entrypoint;
        std::unordered_set<std::string> m_validimportsections;
};

LOADER_INHERIT(PE32Loader, (PELoader<32>), "Portable Executable (32 bits)")
LOADER_INHERIT(PE64Loader, (PELoader<64>), "Portable Executable (64 bits)")

DECLARE_LOADER_PLUGIN(PE32Loader, pe32)
DECLARE_LOADER_PLUGIN(PE64Loader, pe64)

}

#include "pe.cpp"
