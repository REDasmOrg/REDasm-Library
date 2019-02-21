#include "rtti_msvc.h"
#include "../../../disassembler/disassemblerapi.h"
#include "../../../formats/pe/pe.h"
#include "../../symbolize.h"

#define RTTI_MSVC_CLASS_DESCRIPTOR_PREFIX ".?AV"
#define RTTI_MSVC_FIXUP (sizeof(T) * 2)
#define RTTI_MSVC_TYPE_DESCRIPTOR(typedescriptorname) FormatPlugin::relpointer<RTTITypeDescriptor>(typedescriptorname, -RTTI_MSVC_FIXUP)

namespace REDasm {
namespace RTTI {

template<typename T> void RTTIMsvc<T>::search(DisassemblerAPI *disassembler)
{
    RTTIVTableMap rttivtables;
    RTTICompleteObjectMap rttiobjects;
    RTTITypeDescriptorMap rttitypes;
    DataSegmentList segments;

    RTTIMsvc::searchDataSegments(disassembler, segments);
    RTTIMsvc::searchTypeDescriptors(disassembler, rttitypes, segments);
    RTTIMsvc::searchCompleteObjects(disassembler, rttiobjects, rttitypes, segments);
    RTTIMsvc::searchVTables(disassembler, rttivtables, rttiobjects, segments);

    auto lock = x_lock_safe_ptr(disassembler->document());
    FormatPlugin* format = disassembler->format();

    for(auto& rttivtableitem : rttivtables)
    {
        const RTTICompleteObjectLocator* rttiobject = rttivtableitem.first;
        auto it = rttiobjects.find(rttiobject);

        if(it == rttiobjects.end())
            continue;

        std::string objectname = RTTIMsvc::objectName(disassembler, rttiobject);
        std::string vtablename = RTTIMsvc::vtableName(disassembler, rttiobject);
        const T* pobjectdata = rttivtableitem.second;
        address_t address = format->addressof(pobjectdata), rttiobjectaddress = format->addressof(rttiobject);

        REDasm::status("Reading " + objectname + "'s VTable");

        lock->type(address, vtablename);
        lock->lock(address, objectname + "::ptr_rtti_object", SymbolTypes::Data | SymbolTypes::Pointer);

        REDasm::symbolize<RTTICompleteObjectLocator>(disassembler, rttiobjectaddress, objectname + "::rtti_complete_object_locator");
        REDasm::symbolize<RTTIClassHierarchyDescriptor>(disassembler, rttiobject->pClassHierarchyDescriptor, objectname + "::rtti_class_hierarchy");
        disassembler->pushReference(rttiobjectaddress, address);
        pobjectdata++; // Skip RTTICompleteObjectLocator

        const Segment* segment = lock->segment(static_cast<T>(*pobjectdata));

        for(u64 i = 0; segment && segment->is(SegmentTypes::Code); i++) // Walk vtable
        {
            address = format->addressof(pobjectdata);
            disassembler->disassemble(*pobjectdata);

            lock->lock(address, objectname + "::vftable_" + std::to_string(i), SymbolTypes::Data | SymbolTypes::Pointer);
            lock->function(*pobjectdata, objectname + "::sub_" + REDasm::hex(*pobjectdata));

            disassembler->pushReference(*pobjectdata, address);

            pobjectdata++;
            segment = lock->segment(*pobjectdata);
        }

        RTTIMsvc<T>::readHierarchy(disassembler, lock, rttiobject);
    }
}

template<typename T> u32 RTTIMsvc<T>::rttiSignature(DisassemblerAPI* disassembler)
{
    auto* peformat = static_cast<const PeFormat64*>(disassembler->format());

    if(peformat->bits() == 64)
        return RTTISignatureType::x64;

    return RTTISignatureType::x86;
}

template<typename T> address_t RTTIMsvc<T>::rttiAddress(DisassemblerAPI* disassembler, address_t address)
{
    auto* peformat = static_cast<const PeFormat64*>(disassembler->format());

    if(peformat->bits() == 64)
        return peformat->rvaToVa(address);

    return address;
}

template<typename T> std::string RTTIMsvc<T>::objectName(const RTTIMsvc::RTTITypeDescriptor *rttitype)
{
    std::string rttitypename = reinterpret_cast<const char*>(&rttitype->name);
    return Demangler::demangled("?" + rttitypename.substr(4) + "6A@Z");
}

template<typename T> std::string RTTIMsvc<T>::objectName(DisassemblerAPI* disassembler, const RTTICompleteObjectLocator *rttiobject)
{
    const RTTITypeDescriptor* rttitype = disassembler->format()->addrpointer<RTTITypeDescriptor>(RTTIMsvc<T>::rttiAddress(disassembler, rttiobject->pTypeDescriptor));
    return RTTIMsvc::objectName(rttitype);
}

template<typename T> std::string RTTIMsvc<T>::vtableName(DisassemblerAPI *disassembler, const RTTICompleteObjectLocator *rttiobject)
{
    const RTTITypeDescriptor* rttitype = disassembler->format()->addrpointer<RTTITypeDescriptor>(RTTIMsvc<T>::rttiAddress(disassembler, rttiobject->pTypeDescriptor));
    std::string rttitypename = reinterpret_cast<const char*>(&rttitype->name);
    return Demangler::demangled("??_7" + rttitypename.substr(4) + "6B@Z");
}

template<typename T> void RTTIMsvc<T>::readHierarchy(DisassemblerAPI* disassembler, document_x_lock& lock, const RTTICompleteObjectLocator* rttiobject)
{
    FormatPlugin* format = disassembler->format();
    std::string objectname = RTTIMsvc<T>::objectName(disassembler, rttiobject);
    RTTIClassHierarchyDescriptor* pclasshierarchy = format->addrpointer<RTTIClassHierarchyDescriptor>(RTTIMsvc<T>::rttiAddress(disassembler, rttiobject->pClassHierarchyDescriptor));
    u32* pbcdescriptor = format->addrpointer<u32>(RTTIMsvc<T>::rttiAddress(disassembler, pclasshierarchy->pBaseClassArray));

    for(u64 i = 0; i < pclasshierarchy->numBaseClasses; i++, pbcdescriptor++) // Walk class hierarchy
    {
        address_t bcaddress = format->addressof(pbcdescriptor);
        RTTIBaseClassDescriptor* pbaseclass = format->addrpointer<RTTIBaseClassDescriptor>(RTTIMsvc<T>::rttiAddress(disassembler, *pbcdescriptor));

        lock->pointer(RTTIMsvc<T>::rttiAddress(disassembler, pclasshierarchy->pBaseClassArray), SymbolTypes::Data);
        REDasm::symbolize<RTTIBaseClassDescriptor>(disassembler, format->addressof(pbaseclass), objectname + "::rtti_base_class");

        RTTITypeDescriptor* rttitype = format->addrpointer<RTTITypeDescriptor>(RTTIMsvc<T>::rttiAddress(disassembler, pbaseclass->pTypeDescriptor));
        lock->lock(bcaddress, objectname + "::ptr_base_" + RTTIMsvc<T>::objectName(rttitype) + "_" + REDasm::hex(bcaddress), SymbolTypes::Data | SymbolTypes::Pointer);
    }
}

template<typename T> void RTTIMsvc<T>::searchDataSegments(DisassemblerAPI *disassembler, DataSegmentList &segments)
{
    const ListingDocument& document = disassembler->document();

    for(size_t i = 0; i < document->segmentsCount(); i++)
    {
        const Segment* segment = document->segmentAt(i);

        if(segment->empty() || segment->is(SegmentTypes::Bss) || segment->is(SegmentTypes::Code) || (segment->name.find("data") == std::string::npos))
            continue;

        REDasm::status("Checking segment '" + segment->name + "'");
        segments.push_front(segment);
    }
}

template<typename T> void RTTIMsvc<T>::searchTypeDescriptors(DisassemblerAPI *disassembler, RTTITypeDescriptorMap &rttitypes, const DataSegmentList &segments)
{
    const FormatPlugin* format = disassembler->format();
    ListingDocument& document = disassembler->document();

    for(const Segment* segment : segments)
    {
        BufferView view = format->viewSegment(segment);

        if(view.eob())
            continue;

        auto res = view.find<char>(RTTI_MSVC_CLASS_DESCRIPTOR_PREFIX);

        while(res.hasNext())
        {
            const RTTITypeDescriptor* rttitype = RTTI_MSVC_TYPE_DESCRIPTOR(res.result);
            address_t rttiaddress = format->addressof(rttitype);
            REDasm::statusAddress("Searching RTTITypeDescriptors in " + REDasm::quoted(segment->name), rttiaddress);

            if(document->segment(rttitype->pVFTable))
            {
                REDasm::symbolize<RTTITypeDescriptor>(disassembler, rttiaddress, RTTIMsvc<T>::objectName(rttitype) + "::rtti_type_descriptor");
                rttitypes.emplace(segment->address + res.position - RTTI_MSVC_FIXUP, rttitype);
            }

            res = res.next();
        }
    }
}

template<typename T> void RTTIMsvc<T>::searchCompleteObjects(DisassemblerAPI *disassembler, RTTICompleteObjectMap& rttiobjects, const RTTITypeDescriptorMap &rttitypes, const DataSegmentList &segments)
{
    const FormatPlugin* format = disassembler->format();
    RTTICompleteObjectLocatorSearch searchobj = { RTTIMsvc<T>::rttiSignature(disassembler), 0, 0, 0 };

    for(const auto& item : rttitypes)
    {
        auto* peformat = static_cast<const PeFormat64*>(disassembler->format());

        if(peformat->bits() == 64)
            searchobj.pTypeDescriptor = peformat->vaToRva(item.first);
        else
            searchobj.pTypeDescriptor = item.first;

        for(const Segment* segment : segments)
        {
            BufferView view = format->viewSegment(segment);
            auto res = view.find<RTTICompleteObjectLocatorSearch>(&searchobj);

            if(!res.hasNext())
                continue;

            REDasm::statusProgress("Searching RTTICompleteObjectLocators in " + REDasm::quoted(segment->name), format->address(res.position));
            rttiobjects.emplace(reinterpret_cast<const RTTICompleteObjectLocator*>(res.result), segment->address + res.position);
            break;
        }
    }
}

template<typename T> void RTTIMsvc<T>::searchVTables(DisassemblerAPI *disassembler, RTTIMsvc::RTTIVTableMap &vtables, const RTTIMsvc::RTTICompleteObjectMap &rttiobjects, const RTTIMsvc::DataSegmentList &segments)
{
    const FormatPlugin* format = disassembler->format();

    for(const auto& item : rttiobjects)
    {
        const RTTICompleteObjectLocator* rttiobject = item.first;
        REDasm::status("Searching VTables for " + REDasm::quoted(RTTIMsvc::objectName(disassembler, rttiobject)));

        for(const Segment* segment : segments)
        {
            BufferView view = format->viewSegment(segment);
            auto res = view.find(reinterpret_cast<const T*>(&item.second));
            bool found = false;

            while(res.hasNext())
            {
                const RTTICompleteObjectLocator* foundrttiobject = format->addrpointer<RTTICompleteObjectLocator>(*res.result);

                if(rttiobject == foundrttiobject)
                {
                    found = true;
                    break;
                }

                res = res.next();
            }

            if(!found)
                continue;

            vtables.emplace(item.first, format->pointer<T>(segment->offset + res.position));
            break;
        }
    }
}

} // namespace RTTI
} // namespace REDasm
