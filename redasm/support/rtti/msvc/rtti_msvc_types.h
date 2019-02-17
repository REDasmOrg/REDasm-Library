#ifndef RTTI_MSVC_TYPES_H
#define RTTI_MSVC_TYPES_H

#include "../../../types/base_types.h"

namespace REDasm {
namespace RTTI {

template<typename  T> struct RTTIBaseClassDescriptorT
{
    T pTypeDescriptor;
    u32 numContainedBases;

    struct {
        T mdisp, pdisp, vdisp;
    } pmd;

    u32 attributes;
    T pClassDescriptor;
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


#endif // RTTI_MSVC_TYPES_H
