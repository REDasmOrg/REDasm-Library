#pragma once

#include <type_traits>
#include "../libs/visit_struct/visit_struct.hpp"
#include "../disassembler/listing/document/listingdocument.h"
#include "../disassembler/disassembler.h"
#include "../context.h"
#include "demangler.h"
#include "utils.h"

#define REDASM_SYMBOLIZE(T, address) REDasm::symbolize<T>(address, #T)

namespace REDasm {

struct StructVisitor {
    template<typename T, typename = void> struct VisitorImpl {
        static bool visit(address_t address, const String& basename, const char* name) { // Basic Types
            u64 value = 0;

            if(r_disasm->dereference(address, &value) && r_doc->segment(value)) r_doc->pointer(address, basename + "." + name);
            else r_doc->data(address, sizeof(T), basename + "." + name);
            return true;
        }
    };

    template<typename T> struct VisitorImpl<T, typename std::enable_if<std::is_array<T>::value && std::is_convertible<T, String>::value>::type> { // Arrays
        static bool visit(address_t address, const String& basename, const char* name) {
            r_doc->rename(address, basename + "." + name);
            return true;
        }
    };

    template<typename T> struct VisitorImpl<T, typename std::enable_if<std::is_class<T>::value>::type> { // Classes
        static bool visit(address_t address, const String& basename, const char* name) {
            if(!StructVisitor::symbolize<T>(address, basename + "." + name)) return false;
            r_doc->data(address, sizeof(T), basename + "." + name);
            return true;
        }
    };

    StructVisitor(address_t address, const String& basename): address(address), basename(basename) { }

    template<typename T> void operator()(const char* name, const visit_struct::type_c<T>& tag) {
        if(failed) return;
        if(!VisitorImpl<T>::visit(address, basename, name)) failed = true;
        address += sizeof(T);
    }

    template<typename T> static bool symbolize(address_t address, const String& name) {
        if(!std::is_trivial<T>::value) {
            r_ctx->problem("Type " + Demangler::typeName<T>().quoted() + "is not Trivial");
            return false;
        }

        String symbolname = name + "_" + String::hex(address); // Generate an unique name
        StructVisitor visitor(address, symbolname);
        visit_struct::visit_types<T>(visitor);
        r_doc->type(address, symbolname);
        return true;
    }

    address_t address;
    const String& basename;
    bool failed{false};
};

template<typename T> static inline void symbolize(address_t address, const String& name) { StructVisitor::symbolize<T>(address, name); }

} // namespace REDasm
