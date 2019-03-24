#ifndef PE_CLASSIFIER_H
#define PE_CLASSIFIER_H

// https://en.wikipedia.org/wiki/Microsoft_Foundation_Class_Library
// https://github.com/horsicq/Detect-It-Easy/blob/master/db/PE/Microsoft%20Visual%20Studio.4.sg
// http://matthew-brett.github.io/pydagogue/python_msvc.html

#include <unordered_set>
#include "pe_header.h"
#include "pe_resources.h"
#include "dotnet/dotnet_header.h"

namespace REDasm {

namespace PEClassifications {

enum {
    Unclassified = 0,
    VisualBasic_5, VisualBasic_6,
    VisualStudio, VisualStudio_4, VisualStudio_5, VisualStudio_6,
    VisualStudio_2002, VisualStudio_2003, VisualStudio_2005, VisualStudio_2008,
    VisualStudio_2010, VisualStudio_2012, VisualStudio_2013, VisualStudio_2015, VisualStudio_2017,
    DotNet_1, DotNet,
    BorlandDelphi, BorlandDelphi_3, BorlandDelphi_6, BorlandDelphi_7, BorlandDelphi_9_10,
    BorlandDelphi_XE, BorlandDelphi_XE2_6,
    BorlandCpp,
};

}

class PEClassifier
{
    public:
        PEClassifier();
        const std::unordered_set<std::string>& signatures() const;
        bool isClassified() const;
        size_t checkDotNet() const;
        size_t checkVisualBasic() const;
        size_t checkVisualStudio() const;
        size_t checkBorland() const;
        size_t checkDelphi() const;
        size_t bits() const;
        void setBits(size_t bits);
        void classifyVisualStudio();
        void classifyDotNet(ImageCorHeader* corheader);
        void classifyImport(const std::string& library);
        void classifyDelphi(ImageDosHeader *dosheader, const ImageNtHeaders *ntheaders, const PEResources& peresources);
        void classify(const ImageNtHeaders* ntheaders);
        void display();

    private:
        void checkLinkerVersion(u8 major, u8 minor);
        void addSignature(const std::string& s);

    private:
        size_t m_classification, m_bits;
        std::string m_borlandsignature;
        std::unordered_set<std::string> m_signatures;
};

} // namespace REDasm

#endif // PE_CLASSIFIER_H
