#include "pe_classifier.h"
#include "borland/borland_types.h"
#include "borland/borland_version.h"

namespace REDasm {

PEClassifier::PEClassifier(): m_classification(PEClassifications::Unclassified), m_bits(0) { }
const std::unordered_set<std::string> &PEClassifier::signatures() const { return m_signatures; }

size_t PEClassifier::checkVisualBasic() const
{
    if((m_classification == PEClassifications::VisualBasic_5) || (m_classification == PEClassifications::VisualBasic_6))
        return m_classification;

    return PEClassifications::Unclassified;
}

size_t PEClassifier::checkDotNet() const
{
    if((m_classification == PEClassifications::DotNet_1) || (m_classification == PEClassifications::DotNet))
        return m_classification;

    return PEClassifications::Unclassified;
}

size_t PEClassifier::checkVisualStudio() const
{
    if((m_classification >= PEClassifications::VisualStudio) && (m_classification <= PEClassifications::VisualStudio_2017))
        return m_classification;

    return PEClassifications::Unclassified;
}

size_t PEClassifier::checkBorland() const
{
    if((m_classification == PEClassifications::BorlandDelphi) || (m_classification == PEClassifications::BorlandCpp))
        return m_classification;

    return PEClassifications::Unclassified;
}

size_t PEClassifier::checkDelphi() const
{
    if((m_classification >= PEClassifications::BorlandDelphi) && (m_classification <= PEClassifications::BorlandDelphi_XE2_6))
        return m_classification;

    return PEClassifications::Unclassified;
}

size_t PEClassifier::bits() const { return m_bits; }
void PEClassifier::setBits(size_t bits) { m_bits = bits; }

void PEClassifier::classifyVisualStudio()
{
    if(this->isClassified())
        return;

    m_classification = PEClassifications::VisualStudio;
}

void PEClassifier::classifyDotNet(ImageCorHeader *corheader)
{
    if(!corheader || (corheader->cb < sizeof(ImageCorHeader)))
        return;

    if(corheader->MajorRuntimeVersion == 1)
        m_classification = PEClassifications::DotNet_1;
    else
        m_classification = PEClassifications::DotNet;
}

void PEClassifier::classifyImport(const std::string& library)
{
    if(REDasm::startsWith(library, "msvbvm50"))
        m_classification = PEClassifications::VisualBasic_5;
    else if(REDasm::startsWith(library, "msvbvm60"))
        m_classification = PEClassifications::VisualBasic_6;

    if((this->checkVisualBasic() || this->isClassified()) && (m_classification != PEClassifications::VisualStudio))
        return;

    if(REDasm::startsWith(library, "msvcp50"))
        m_classification = PEClassifications::VisualStudio_5;
    else if(REDasm::startsWith(library, "msvcp60") || REDasm::startsWith(library, "msvcrt."))
        m_classification = PEClassifications::VisualStudio_6;
    else if(REDasm::startsWith(library, "msvcp70") || REDasm::startsWith(library, "msvcr70"))
        m_classification = PEClassifications::VisualStudio_2002;
    else if(REDasm::startsWith(library, "msvcp71") || REDasm::startsWith(library, "msvcr71"))
        m_classification = PEClassifications::VisualStudio_2003;
    else if(REDasm::startsWith(library, "msvcp80") || REDasm::startsWith(library, "msvcr80"))
        m_classification = PEClassifications::VisualStudio_2005;
    else if(REDasm::startsWith(library, "msvcp90") || REDasm::startsWith(library, "msvcr90"))
        m_classification = PEClassifications::VisualStudio_2008;
    else if(REDasm::startsWith(library, "msvcp100") || REDasm::startsWith(library, "msvcr100"))
        m_classification = PEClassifications::VisualStudio_2010;
    else if(REDasm::startsWith(library, "msvcp110") || REDasm::startsWith(library, "msvcr110"))
        m_classification = PEClassifications::VisualStudio_2012;
    else if(REDasm::startsWith(library, "msvcp120") || REDasm::startsWith(library, "msvcr120"))
        m_classification = PEClassifications::VisualStudio_2013;
    else if(REDasm::startsWith(library, "msvcp140") || REDasm::startsWith(library, "vcruntime140"))
        m_classification = PEClassifications::VisualStudio_2015;
}

void PEClassifier::classifyDelphi(ImageDosHeader* dosheader, const ImageNtHeaders* ntheaders, const PEResources &peresources)
{
    PEResources::ResourceItem ri = peresources.find(PEResources::RCDATA);

    if(!ri.second)
        return;

    ri = peresources.find("PACKAGEINFO", ri);

    if(!ri.second)
        return;

    u64 datasize = 0;
    PackageInfoHeader* packageinfo = peresources.data<PackageInfoHeader>(ri, dosheader, ntheaders, &datasize);

    if(!packageinfo)
    {
        REDasm::log("Cannot parse 'PACKAGEINFO' header");
        return;
    }

    BorlandVersion borlandver(packageinfo, ri, datasize);

    if(borlandver.isDelphi())
    {
        m_borlandsignature = borlandver.getSignature();

        if(m_borlandsignature == "delphi3")
            m_classification = PEClassifications::BorlandDelphi_3;
        else if(m_borlandsignature == "delphiXE2_6")
            m_classification = PEClassifications::BorlandDelphi_XE2_6;
        else if(m_borlandsignature == "delphiXE")
            m_classification = PEClassifications::BorlandDelphi_XE;
        else if(m_borlandsignature == "delphi9_10")
            m_classification = PEClassifications::BorlandDelphi_9_10;
        else if(m_borlandsignature == "delphi6")
            m_classification = PEClassifications::BorlandDelphi_6;
        else if(m_borlandsignature == "delphi6")
            m_classification = PEClassifications::BorlandDelphi_7;
        else
            m_classification = PEClassifications::BorlandDelphi;
    }
    else if(borlandver.isCpp())
        m_classification = PEClassifications::BorlandCpp;
}

void PEClassifier::classify(const ImageNtHeaders *ntheaders)
{
    if(this->checkVisualStudio() || !this->isClassified())
    {
        if(ntheaders->OptionalHeaderMagic == IMAGE_NT_OPTIONAL_HDR64_MAGIC)
            this->checkLinkerVersion(ntheaders->OptionalHeader64.MajorLinkerVersion, ntheaders->OptionalHeader64.MinorLinkerVersion);
        else
            this->checkLinkerVersion(ntheaders->OptionalHeader32.MajorLinkerVersion, ntheaders->OptionalHeader32.MinorLinkerVersion);
    }

    if(this->checkBorland())
        this->addSignature(m_borlandsignature);
    else if(m_classification == PEClassifications::VisualStudio_6)
        this->addSignature("msvc6");
    else if(m_classification == PEClassifications::VisualStudio_2003)
        this->addSignature("msvc2003");
    else if(m_classification == PEClassifications::VisualStudio_2005)
        this->addSignature("msvc2005");
    else if(m_classification == PEClassifications::VisualStudio_2008)
        this->addSignature("msvc2008");
    else if(m_classification == PEClassifications::VisualStudio_2017)
        this->addSignature("msvc2017");
}

void PEClassifier::display()
{
    if(m_classification == PEClassifications::VisualBasic_5)
        REDasm::log("PE Classification: Visual Basic 5");
    else if(m_classification == PEClassifications::VisualBasic_6)
        REDasm::log("PE Classification: Visual Basic 6");
    else if(m_classification == PEClassifications::VisualStudio)
        REDasm::log("PE Classification: Visual Studio");
    else if(m_classification == PEClassifications::VisualStudio_4)
        REDasm::log("PE Classification: Visual Studio 4");
    else if(m_classification == PEClassifications::VisualStudio_5)
        REDasm::log("PE Classification: Visual Studio 5");
    else if(m_classification == PEClassifications::VisualStudio_6)
        REDasm::log("PE Classification: Visual Studio 6");
    else if(m_classification == PEClassifications::VisualStudio_2002)
        REDasm::log("PE Classification: Visual Studio 2002");
    else if(m_classification == PEClassifications::VisualStudio_2003)
        REDasm::log("PE Classification: Visual Studio 2003");
    else if(m_classification == PEClassifications::VisualStudio_2005)
        REDasm::log("PE Classification: Visual Studio 2005");
    else if(m_classification == PEClassifications::VisualStudio_2008)
        REDasm::log("PE Classification: Visual Studio 2008");
    else if(m_classification == PEClassifications::VisualStudio_2010)
        REDasm::log("PE Classification: Visual Studio 2010");
    else if(m_classification == PEClassifications::VisualStudio_2012)
        REDasm::log("PE Classification: Visual Studio 2012");
    else if(m_classification == PEClassifications::VisualStudio_2013)
        REDasm::log("PE Classification: Visual Studio 2013");
    else if(m_classification == PEClassifications::VisualStudio_2015)
        REDasm::log("PE Classification: Visual Studio 2015");
    else if(m_classification == PEClassifications::VisualStudio_2017)
        REDasm::log("PE Classification: Visual Studio 2017");
    else if(m_classification == PEClassifications::DotNet_1)
        REDasm::log("PE Classification: .NET 1.x");
    else if(m_classification == PEClassifications::DotNet)
        REDasm::log("PE Classification: .NET >= 2.x");
    else if(m_classification == PEClassifications::BorlandDelphi)
        REDasm::log("PE Classification: Borland Delphi");
    else if(m_classification == PEClassifications::BorlandDelphi_3)
        REDasm::log("PE Classification: Borland Delphi 3");
    else if(m_classification == PEClassifications::BorlandDelphi_6)
        REDasm::log("PE Classification: Borland Delphi 6");
    else if(m_classification == PEClassifications::BorlandDelphi_7)
        REDasm::log("PE Classification: Borland Delphi 7");
    else if(m_classification == PEClassifications::BorlandDelphi_9_10)
        REDasm::log("PE Classification: Borland Delphi 9/10");
    else if(m_classification == PEClassifications::BorlandDelphi_XE)
        REDasm::log("PE Classification: Borland Delphi XE");
    else if(m_classification == PEClassifications::BorlandDelphi_XE2_6)
        REDasm::log("PE Classification: Borland Delphi XE 2.6");
    else if(m_classification == PEClassifications::BorlandCpp)
        REDasm::log("PE Classification: Borland C++");
    else
        REDasm::log("PE Classification: Unclassified");
}

bool PEClassifier::isClassified() const { return m_classification != PEClassifications::Unclassified;  }

void PEClassifier::checkLinkerVersion(u8 major, u8 minor)
{
    if(major == 4)
        m_classification = PEClassifications::VisualStudio_4;
    else if(major == 5)
        m_classification = PEClassifications::VisualStudio_5;
    else if(major == 6)
        m_classification = PEClassifications::VisualStudio_6;
    else if(major == 7)
    {
        if(minor < 10)
            m_classification = PEClassifications::VisualStudio_2002;
        else
            m_classification = PEClassifications::VisualStudio_2003;
    }
    else if(major == 8)
        m_classification = PEClassifications::VisualStudio_2005;
    else if(major == 9)
        m_classification = PEClassifications::VisualStudio_2008;
    else if(major == 10)
        m_classification = PEClassifications::VisualStudio_2010;
    else if(major == 11)
        m_classification = PEClassifications::VisualStudio_2012;
    else if(major == 12)
        m_classification = PEClassifications::VisualStudio_2013;
    else if(major == 14)
    {
        if(!minor)
            m_classification = PEClassifications::VisualStudio_2015;
        else
            m_classification = PEClassifications::VisualStudio_2017;
    }
}

void PEClassifier::addSignature(const std::string &s)
{
    if(s.empty())
        return;

    m_signatures.insert(s);
}

} // namespace REDasm
