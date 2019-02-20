#ifndef SYMBOLIZE_H
#define SYMBOLIZE_H

#include <type_traits>
#include "../../redasm/disassembler/listing/listingdocument.h"
#include "../../redasm/disassembler/disassemblerapi.h"

namespace REDasm {

struct StructVisitor
{
    StructVisitor(DisassemblerAPI* disassembler, address_t address, const std::string& basename): address(address), disassembler(disassembler), document(disassembler->document()), basename(basename) { }

    template<typename T> void operator()(const char* name, const visit_struct::type_c<T>& tag) {
        if(std::is_array<T>::value && std::is_convertible<T, std::string>::value)
            document->lock(address, basename + "." + std::string(name), SymbolTypes::String);
        else {
            document->lock(address, basename + "." + std::string(name), SymbolTypes::Data);
        }

        address += sizeof(T);
    }

    address_t address;
    DisassemblerAPI* disassembler;
    ListingDocument& document;
    const std::string& basename;
};

template<typename T> void symbolize(DisassemblerAPI* disassembler, address_t address, const std::string& name)
{
    if(!std::is_pod<T>::value)
    {
        REDasm::log("Type " + REDasm::quoted(Demangler::typeName<T>()) + "is not POD");
        return;
    }

    auto document = disassembler->document();
    std::string symbolname = name + "_" + REDasm::hex(address); // Generate an unique name
    StructVisitor visitor(disassembler, address, symbolname);
    visit_struct::visit_types<T>(visitor);
    document->info(address, "struct " + symbolname); // Add later, it may be removed from ListingDocument
}

} // namespace REDasm

#endif // SYMBOLIZE_H
