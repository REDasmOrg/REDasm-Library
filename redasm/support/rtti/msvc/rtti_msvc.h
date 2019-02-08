#ifndef RTTIMSVC_H
#define RTTIMSVC_H

#include "../../../redasm_api.h"
#include "../../../types/buffer/bufferview.h"
#include <forward_list>

namespace REDasm {

class FormatPlugin;
class DisassemblerAPI;

namespace RTTI {

struct RTTIBaseClassDescriptor
{
    u32 pTypeDescriptor;
    u32 numContainedBases;

    struct {
        u32 mdisp, pdisp, vdisp;
    } pmd;

    u32 attributes;
    u32 pClassDescriptor;
};

struct RTTIClassHierarchyDescriptor
{
    u32 signature, attributes, numBaseClasses;
    u32 pBaseClassArray;
};

struct RTTITypeDescriptor
{
    u32 pVFTable, spare;
    char name[1];
};

struct RTTICompleteObjectLocator
{
  u32 signature, offset, cdOffset;
  u32 pTypeDescriptor, pClassHierarchyDescriptor;
};

class RTTIMsvc
{
    private:
        struct RTTICompleteObjectLocatorSearch { u32 signature, offset, cdOffset, pTypeDescriptor; };
        typedef std::forward_list<const Segment*> DataSegmentList;
        typedef std::unordered_map<address_t, const RTTITypeDescriptor*> RTTITypeDescriptorMap;
        typedef std::unordered_map<const RTTICompleteObjectLocator*, address_t> RTTICompleteObjectMap;
        typedef std::unordered_map<const RTTICompleteObjectLocator*, const u32*> RTTIVTableMap;

    public:
        RTTIMsvc() = delete;
        RTTIMsvc(const RTTIMsvc&) = delete;
        static void search(DisassemblerAPI *disassembler);

    private:
        static void searchDataSegments(DisassemblerAPI* disassembler, DataSegmentList& segments);
        static void searchTypeDescriptors(DisassemblerAPI* disassembler, RTTITypeDescriptorMap &rttitypes, const DataSegmentList &segments);
        static void searchCompleteObjects(DisassemblerAPI* disassembler, RTTICompleteObjectMap &rttiobjects,  const RTTITypeDescriptorMap &rttitypes, const DataSegmentList &segments);
        static void searchVTables(DisassemblerAPI* disassembler, RTTIVTableMap& vtables, const RTTICompleteObjectMap &rttiobjects, const DataSegmentList &segments);
};

} // namespace RTTI
} // namespace REDasm

#endif // RTTIMSVC_H
