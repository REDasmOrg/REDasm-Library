#include "rtti_msvc.h"
#include "../../../disassembler/disassemblerapi.h"
#include "../../../formats/pe/pe.h"
#include "../../symbolize.h"

#define RTTI_MSVC_CLASS_DESCRIPTOR_PREFIX ".?AV"
#define RTTI_MSVC_FIXUP (sizeof(T) * 2)
#define RTTI_MSVC_TYPE_DESCRIPTOR(typedescriptorname) FormatPlugin::relpointer<RTTITypeDescriptor>(typedescriptorname, -RTTI_MSVC_FIXUP)

namespace REDasm {
namespace RTTI {

template<typename T> RTTIMsvc<T>::RTTIMsvc(DisassemblerAPI *disassembler): m_disassembler(disassembler), m_document(disassembler->document()), m_format(disassembler->format()) { }

template<typename T> void RTTIMsvc<T>::search()
{
    this->searchDataSegments();
    this->searchTypeDescriptors();
    this->searchCompleteObjects();
    this->searchVTables();

    auto lock = x_lock_safe_ptr(m_document);

    for(auto& rttivtableitem : m_rttivtables)
    {
        const RTTICompleteObjectLocator* rttiobject = rttivtableitem.first;
        auto it = m_rttiobjects.find(rttiobject);

        if(it == m_rttiobjects.end())
            continue;

        std::string objectname = this->objectName(rttiobject);
        std::string vtablename = this->vtableName(rttiobject);
        const T* pobjectdata = rttivtableitem.second;
        address_t address = m_format->addressof(pobjectdata), rttiobjectaddress = m_format->addressof(rttiobject);

        REDasm::status("Reading " + objectname + "'s VTable");

        lock->type(address, vtablename);
        lock->lock(address, objectname + "::ptr_rtti_object", SymbolTypes::Data | SymbolTypes::Pointer);

        REDasm::symbolize<RTTICompleteObjectLocator>(m_disassembler, rttiobjectaddress, objectname + "::rtti_complete_object_locator");
        REDasm::symbolize<RTTIClassHierarchyDescriptor>(m_disassembler, rttiAddress(rttiobject->pClassHierarchyDescriptor), objectname + "::rtti_class_hierarchy");
        m_disassembler->pushReference(rttiobjectaddress, address);
        pobjectdata++; // Skip RTTICompleteObjectLocator

        const Segment* segment = lock->segment(static_cast<T>(*pobjectdata));

        for(u64 i = 0; segment && segment->is(SegmentTypes::Code); i++) // Walk vtable
        {
            address = m_format->addressof(pobjectdata);
            m_disassembler->disassemble(*pobjectdata);

            lock->lock(address, objectname + "::vftable_" + std::to_string(i), SymbolTypes::Data | SymbolTypes::Pointer);
            lock->function(*pobjectdata, objectname + "::sub_" + REDasm::hex(*pobjectdata));

            m_disassembler->pushReference(*pobjectdata, address);

            pobjectdata++;
            segment = lock->segment(*pobjectdata);
        }

        this->readHierarchy(lock, rttiobject);
    }

    if(m_rttiobjects.size())
        REDasm::log("Found " + std::to_string(m_rttiobjects.size()) + " RTTI objects");
    else
        REDasm::log("No RTTI Objects found");
}

template<typename T> u32 RTTIMsvc<T>::rttiSignature() const
{
    auto* peformat = static_cast<const PeFormat64*>(m_format);

    if(peformat->bits() == 64)
        return RTTISignatureType::x64;

    return RTTISignatureType::x86;
}

template<typename T> address_t RTTIMsvc<T>::rttiAddress(address_t address) const
{
    auto* peformat = static_cast<const PeFormat64*>(m_format);

    if(peformat->bits() == 64)
        return peformat->rvaToVa(address);

    return address;
}

template<typename T> std::string RTTIMsvc<T>::objectName(const RTTIMsvc::RTTITypeDescriptor *rttitype)
{
    std::string rttitypename = reinterpret_cast<const char*>(&rttitype->name);
    return Demangler::demangled("?" + rttitypename.substr(4) + "6A@Z");
}

template<typename T> std::string RTTIMsvc<T>::objectName(const RTTICompleteObjectLocator *rttiobject) const
{
    const RTTITypeDescriptor* rttitype = m_format->addrpointer<RTTITypeDescriptor>(this->rttiAddress(rttiobject->pTypeDescriptor));
    return objectName(rttitype);
}

template<typename T> std::string RTTIMsvc<T>::vtableName(const RTTICompleteObjectLocator *rttiobject) const
{
    const RTTITypeDescriptor* rttitype = m_format->addrpointer<RTTITypeDescriptor>(this->rttiAddress(rttiobject->pTypeDescriptor));
    std::string rttitypename = reinterpret_cast<const char*>(&rttitype->name);
    return Demangler::demangled("??_7" + rttitypename.substr(4) + "6B@Z");
}

template<typename T> void RTTIMsvc<T>::readHierarchy(document_x_lock& lock, const RTTICompleteObjectLocator* rttiobject) const
{
    std::string objectname = objectName(rttiobject);
    RTTIClassHierarchyDescriptor* pclasshierarchy = m_format->addrpointer<RTTIClassHierarchyDescriptor>(this->rttiAddress(rttiobject->pClassHierarchyDescriptor));
    u32* pbcdescriptor = m_format->addrpointer<u32>(this->rttiAddress(pclasshierarchy->pBaseClassArray));

    for(u64 i = 0; i < pclasshierarchy->numBaseClasses; i++, pbcdescriptor++) // Walk class hierarchy
    {
        address_t bcaddress = m_format->addressof(pbcdescriptor);
        RTTIBaseClassDescriptor* pbaseclass = m_format->addrpointer<RTTIBaseClassDescriptor>(this->rttiAddress(*pbcdescriptor));

        lock->pointer(this->rttiAddress(pclasshierarchy->pBaseClassArray), SymbolTypes::Data);
        REDasm::symbolize<RTTIBaseClassDescriptor>(m_disassembler, m_format->addressof(pbaseclass), objectname + "::rtti_base_class");

        RTTITypeDescriptor* rttitype = m_format->addrpointer<RTTITypeDescriptor>(this->rttiAddress(pbaseclass->pTypeDescriptor));
        lock->lock(bcaddress, objectname + "::ptr_base_" + objectName(rttitype) + "_" + REDasm::hex(bcaddress), SymbolTypes::Data | SymbolTypes::Pointer);
    }
}

template<typename T> void RTTIMsvc<T>::searchDataSegments()
{
    for(size_t i = 0; i < m_document->segmentsCount(); i++)
    {
        const Segment* segment = m_document->segmentAt(i);

        if(segment->empty() || segment->is(SegmentTypes::Bss) || segment->is(SegmentTypes::Code) || (segment->name.find("data") == std::string::npos))
            continue;

        REDasm::status("Checking segment '" + segment->name + "'");
        m_segments.push_front(segment);
    }
}

template<typename T> void RTTIMsvc<T>::searchTypeDescriptors()
{
    for(const Segment* segment : m_segments)
    {
        BufferView view = m_format->viewSegment(segment);

        if(view.eob())
            continue;

        auto res = view.find<char>(RTTI_MSVC_CLASS_DESCRIPTOR_PREFIX);

        while(res.hasNext())
        {
            const RTTITypeDescriptor* rttitype = RTTI_MSVC_TYPE_DESCRIPTOR(res.result);
            address_t rttiaddress = m_format->addressof(rttitype);
            REDasm::statusAddress("Searching RTTITypeDescriptors in " + REDasm::quoted(segment->name), rttiaddress);

            if(m_document->segment(rttitype->pVFTable))
            {
                REDasm::symbolize<RTTITypeDescriptor>(m_disassembler, rttiaddress, objectName(rttitype) + "::rtti_type_descriptor");
                m_rttitypes.emplace(segment->address + res.position - RTTI_MSVC_FIXUP, rttitype);
            }

            res = res.next();
        }
    }
}

template<typename T> void RTTIMsvc<T>::searchCompleteObjects()
{
    RTTICompleteObjectLocatorSearch searchobj = { this->rttiSignature(), 0, 0, 0 };

    for(const auto& item : m_rttitypes)
    {
        const auto* peformat = static_cast<const PeFormat64*>(m_format);

        if(peformat->bits() == 64)
            searchobj.pTypeDescriptor = peformat->vaToRva(item.first);
        else
            searchobj.pTypeDescriptor = item.first;

        for(const Segment* segment : m_segments)
        {
            BufferView view = m_format->viewSegment(segment);
            auto res = view.find<RTTICompleteObjectLocatorSearch>(&searchobj);

            if(!res.hasNext())
                continue;

            REDasm::statusProgress("Searching RTTICompleteObjectLocators in " + REDasm::quoted(segment->name), m_format->address(res.position));
            m_rttiobjects.emplace(reinterpret_cast<const RTTICompleteObjectLocator*>(res.result), segment->address + res.position);
            break;
        }
    }
}

template<typename T> void RTTIMsvc<T>::searchVTables()
{
    for(const auto& item : m_rttiobjects)
    {
        const RTTICompleteObjectLocator* rttiobject = item.first;
        REDasm::status("Searching VTables for " + REDasm::quoted(objectName(rttiobject)));

        for(const Segment* segment : m_segments)
        {
            BufferView view = m_format->viewSegment(segment);
            auto res = view.find(reinterpret_cast<const T*>(&item.second));
            bool found = false;

            while(res.hasNext())
            {
                const RTTICompleteObjectLocator* foundrttiobject = m_format->addrpointer<RTTICompleteObjectLocator>(*res.result);

                if(rttiobject != foundrttiobject)
                    res = res.next();

                found = true;
                break;
            }

            if(!found)
                continue;

            m_rttivtables.emplace(item.first, m_format->pointer<T>(segment->offset + res.position));
            break;
        }
    }
}

} // namespace RTTI
} // namespace REDasm
