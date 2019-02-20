#ifndef RTTI_MSVC_TYPES_H
#define RTTI_MSVC_TYPES_H

#include "../../../types/base_types.h"

namespace REDasm {
namespace RTTI {

template<typename  T> struct RTTIPMD { T mdisp, pdisp, vdisp; };

template<typename  T> struct RTTIBaseClassDescriptorT
{
    T pTypeDescriptor;
    u32 numContainedBases;
    RTTIPMD<T> pmd;
    u32 attributes;
};

template<typename T> struct RTTIClassHierarchyDescriptorT
{
    u32 signature, attributes, numBaseClasses;
    T pBaseClassArray;
};

template<typename T> struct RTTITypeDescriptorT
{
    T pVFTable, spare;
    char name[1];
};

template<typename T> struct RTTICompleteObjectLocatorT
{
    u32 signature, offset, cdOffset;
    T pTypeDescriptor, pClassHierarchyDescriptor;
};

} // namespace RTTI

} // namespace REDasm

VISITABLE_STRUCT(REDasm::RTTI::RTTIPMD<u32>, mdisp, pdisp, vdisp);
VISITABLE_STRUCT(REDasm::RTTI::RTTIPMD<u64>, mdisp, pdisp, vdisp);

VISITABLE_STRUCT(REDasm::RTTI::RTTIBaseClassDescriptorT<u32>, pTypeDescriptor, numContainedBases, pmd, attributes);
VISITABLE_STRUCT(REDasm::RTTI::RTTIBaseClassDescriptorT<u64>, pTypeDescriptor, numContainedBases, pmd, attributes);

VISITABLE_STRUCT(REDasm::RTTI::RTTIClassHierarchyDescriptorT<u32>, signature, attributes, numBaseClasses, pBaseClassArray);
VISITABLE_STRUCT(REDasm::RTTI::RTTIClassHierarchyDescriptorT<u64>, signature, attributes, numBaseClasses, pBaseClassArray);

VISITABLE_STRUCT(REDasm::RTTI::RTTITypeDescriptorT<u32>, pVFTable, spare, name);
VISITABLE_STRUCT(REDasm::RTTI::RTTITypeDescriptorT<u64>, pVFTable, spare, name);

VISITABLE_STRUCT(REDasm::RTTI::RTTICompleteObjectLocatorT<u32>,
                 signature, offset, cdOffset,
                 pTypeDescriptor, pClassHierarchyDescriptor);

VISITABLE_STRUCT(REDasm::RTTI::RTTICompleteObjectLocatorT<u64>,
                 signature, offset, cdOffset,
                 pTypeDescriptor, pClassHierarchyDescriptor);

#endif // RTTI_MSVC_TYPES_H
