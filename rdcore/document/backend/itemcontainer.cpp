#include "itemcontainer.h"

size_t ItemContainer::instructionIndex(rd_address address) const { return this->indexOf({ address, DocumentItemType_Instruction, 0}); }
size_t ItemContainer::functionIndex(rd_address address) const { return this->indexOf({ address, DocumentItemType_Function, 0 }); }
size_t ItemContainer::symbolIndex(rd_address address) const { return this->indexOf({ address, DocumentItemType_Symbol, 0 }); }
