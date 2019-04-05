#ifndef DISASSEMBLERAPI_H
#define DISASSEMBLERAPI_H

#define MIN_STRING       4

#include "../redasm.h"
#include "../support/event.h"
#include "../support/safe_ptr.h"
#include "types/symboltable.h"
#include "types/referencetable.h"

namespace REDasm {

class Printer;
class LoaderPlugin;
class AssemblerPlugin;
class ListingDocumentType;
struct ListingItem;

class DisassemblerAPI
{
    public:
        SimpleEvent busyChanged;

    public:
        DisassemblerAPI() = default;
        virtual ~DisassemblerAPI() = default;
        virtual LoaderPlugin* loader() const = 0;
        virtual AssemblerPlugin* assembler() const = 0;
        virtual const safe_ptr<ListingDocumentType>& document() const = 0;
        virtual safe_ptr<ListingDocumentType>& document() = 0;
        virtual std::deque<ListingItem*> getCalls(address_t address) = 0;
        virtual ReferenceTable* references() = 0;
        virtual Printer* createPrinter() = 0;
        virtual ReferenceVector getReferences(address_t address) const = 0;
        virtual ReferenceSet getTargets(address_t address) const = 0;
        virtual address_location getTarget(address_t address) const = 0;
        virtual u64 getTargetsCount(address_t address) const = 0;
        virtual u64 getReferencesCount(address_t address) const = 0;
        virtual void popTarget(address_t address, address_t pointedby) = 0;
        virtual void pushTarget(address_t address, address_t pointedby) = 0;
        virtual void pushReference(address_t address, address_t refby) = 0;
        virtual void checkLocation(address_t fromaddress, address_t address) = 0;
        virtual bool checkString(address_t fromaddress, address_t address) = 0;
        virtual s64 checkAddressTable(const InstructionPtr& instruction, address_t address) = 0;
        virtual u64 locationIsString(address_t address, bool *wide = nullptr) const = 0;
        virtual std::string readString(const Symbol* symbol, u64 len = std::numeric_limits<u64>::max()) const = 0;
        virtual std::string readString(address_t address, u64 len = std::numeric_limits<u64>::max()) const = 0;
        virtual std::string readWString(const Symbol* symbol, u64 len = std::numeric_limits<u64>::max()) const = 0;
        virtual std::string readWString(address_t address, u64 len = std::numeric_limits<u64>::max()) const = 0;
        virtual std::string getHexDump(address_t address, const Symbol** ressymbol = nullptr) = 0;
        virtual BufferView getFunctionBytes(address_t address) = 0;
        virtual bool readAddress(address_t address, size_t size, u64 *value) const = 0;
        virtual bool readOffset(offset_t offset, size_t size, u64 *value) const = 0;
        virtual bool dereference(address_t address, u64* value) const = 0;
        virtual Symbol* dereferenceSymbol(const Symbol* symbol, u64* value = nullptr) = 0;
        virtual InstructionPtr disassembleInstruction(address_t address) = 0;
        virtual void disassemble(address_t address) = 0;
        virtual bool loadSignature(const std::string& signame) = 0;
        virtual void disassemble() = 0;
        virtual void stop() = 0;
        virtual void pause() = 0;
        virtual void resume() = 0;
        virtual size_t state() const = 0;
        virtual bool busy() const = 0;
};

typedef std::shared_ptr<DisassemblerAPI> DisassemblerPtr;

} // namespace REDasm

#endif // DISASSEMBLERAPI_H
