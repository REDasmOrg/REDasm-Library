#pragma once

#include "../document/listingitem.h"
#include "../../../macros.h"
#include "../../../pimpl.h"

namespace REDasm {

class FunctionGraph;
class FunctionBasicBlock;
class ListingFunctionsImpl;

class LIBREDASM_API ListingFunctions
{
    PIMPL_DECLARE_P(ListingFunctions)
    PIMPL_DECLARE_PRIVATE(ListingFunctions)

    public:
        ListingFunctions();
        address_t at(size_t idx) const;
        bool contains(address_t address);
        bool insert(address_t address);
        size_t size() const;
        void erase(address_t address);
        void eraseAt(size_t idx);

    public:
        address_location functionFromAddress(address_t address) const;
        const FunctionBasicBlock* basicBlockFromAddress(address_t address) const;
        const FunctionGraph* graph(address_t address) const;
        FunctionGraph* graph(address_t address);
        void graph(address_t address, FunctionGraph* graph);
        void remove(address_t address);
        void invalidateGraphs();
};

} // namespace REDasm
