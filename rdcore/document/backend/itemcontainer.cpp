#include "itemcontainer.h"

size_t ItemContainer::instructionIndex(address_t address) const { return this->indexOf({ address, DocumentItemType_Instruction, 0}); }
size_t ItemContainer::functionIndex(address_t address) const { return this->indexOf({ address, DocumentItemType_Function, 0 }); }
size_t ItemContainer::symbolIndex(address_t address) const { return this->indexOf({ address, DocumentItemType_Symbol, 0 }); }
