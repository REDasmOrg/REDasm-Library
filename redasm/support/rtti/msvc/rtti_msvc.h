#ifndef RTTIMSVC_H
#define RTTIMSVC_H

#include "../../../types/buffer/bufferview.h"
#include "../../../plugins/format.h"
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
        typedef RTTICompleteObjectLocatorT<T> RTTICompleteObjectLocator;
        typedef RTTIClassHierarchyDescriptorT<T> RTTIClassHierarchyDescriptor;
        typedef RTTIBaseClassDescriptorT<T> RTTIBaseClassDescriptor;

    private:
        struct RTTICompleteObjectLocatorSearch { u32 signature, offset, cdOffset; T pTypeDescriptor; };
        typedef std::forward_list<const Segment*> DataSegmentList;
        typedef std::unordered_map<address_t, const RTTITypeDescriptor*> RTTITypeDescriptorMap;
        typedef std::unordered_map<const RTTICompleteObjectLocator*, address_t> RTTICompleteObjectMap;
        typedef std::unordered_map<const RTTICompleteObjectLocator*, const T*> RTTIVTableMap;

    public:
        RTTIMsvc() = delete;
        RTTIMsvc(const RTTIMsvc&) = delete;
        static void search(DisassemblerAPI *disassembler);

    private:
        static std::string objectName(const RTTITypeDescriptor* rttitype);
        static std::string objectName(DisassemblerAPI *disassembler, const RTTICompleteObjectLocator* rttiobject);
        static std::string vtableName(DisassemblerAPI *disassembler, const RTTICompleteObjectLocator* rttiobject);
        static void readHierarchy(DisassemblerAPI* disassembler, document_x_lock& lock, const RTTICompleteObjectLocator* rttiobject);
        static void searchDataSegments(DisassemblerAPI* disassembler, DataSegmentList& segments);
        static void searchTypeDescriptors(DisassemblerAPI* disassembler, RTTITypeDescriptorMap &rttitypes, const DataSegmentList &segments);
        static void searchCompleteObjects(DisassemblerAPI* disassembler, RTTICompleteObjectMap &rttiobjects,  const RTTITypeDescriptorMap &rttitypes, const DataSegmentList &segments);
        static void searchVTables(DisassemblerAPI* disassembler, RTTIVTableMap& vtables, const RTTICompleteObjectMap &rttiobjects, const DataSegmentList &segments);
};

} // namespace RTTI
} // namespace REDasm

#include "rtti_msvc_impl.h"

#endif // RTTIMSVC_H
