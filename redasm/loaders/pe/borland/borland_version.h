#pragma once

#include "../pe_resources.h"
#include "borland_types.h"

namespace REDasm {

class BorlandVersion
{
    public:
        BorlandVersion(PackageInfoHeader* packageinfo, const PEResources::ResourceItem& resourceitem, u64 size);
        bool isDelphi() const;
        bool isCpp() const;
        std::string getSignature() const;

    private:
        bool contains(const std::string& s) const;

    private:
        PackageInfoHeader* m_packageinfo;
        PEResources::ResourceItem m_resourceitem;
        u64 m_size;
};

} // namespace REDasm
