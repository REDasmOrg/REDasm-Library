#ifndef RTTIMSVC_H
#define RTTIMSVC_H

#include "../../../types/buffer/bufferview.h"
#include "../../../plugins/loader.h"
#include "../../../redasm_api.h"
#include "rtti_msvc_types.h"
#include <unordered_map>
#include <forward_list>

namespace REDasm {

class DisassemblerAPI;

namespace RTTI {

template<typename T> class RTTIMsvc
{
    private:
        typedef RTTITypeDescriptorT<T> RTTITypeDescriptor;

    private:
        struct RTTICompleteObjectLocatorSearch { u32 signature, offset, cdOffset, pTypeDescriptor; };
        typedef std::forward_list<const Segment*> DataSegmentList;
        typedef std::unordered_map<address_t, const RTTITypeDescriptor*> RTTITypeDescriptorMap;
        typedef std::unordered_map<const RTTICompleteObjectLocator*, address_t> RTTICompleteObjectMap;
        typedef std::unordered_map<const RTTICompleteObjectLocator*, const T*> RTTIVTableMap;

    public:
        RTTIMsvc(DisassemblerAPI* disassembler);
        void search();

    private:
        u32 rttiSignature() const;
        address_t rttiAddress(address_t address) const;
        std::string objectName(const RTTICompleteObjectLocator* rttiobject) const;
        std::string vtableName(const RTTICompleteObjectLocator* rttiobject) const;
        void readHierarchy(document_x_lock& lock, const RTTICompleteObjectLocator* rttiobject) const;
        void searchDataSegments();
        void searchTypeDescriptors();
        void searchCompleteObjects();
        void searchVTables();

    private:
        static std::string objectName(const RTTITypeDescriptor* rttitype);

    private:
        DisassemblerAPI* m_disassembler;
        ListingDocument& m_document;
        const LoaderPlugin* m_loader;

    private:
        RTTIVTableMap m_rttivtables;
        RTTICompleteObjectMap m_rttiobjects;
        RTTITypeDescriptorMap m_rttitypes;
        DataSegmentList m_segments;
};

} // namespace RTTI
} // namespace REDasm

#include "rtti_msvc_impl.h"

#endif // RTTIMSVC_H
