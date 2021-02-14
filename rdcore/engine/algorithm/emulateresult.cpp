#include "emulateresult.h"

EmulateResult::EmulateResult(rd_address address, const RDBufferView* view): m_address(address), m_view(view) { }
bool EmulateResult::invalid() const { return m_invalid; }
bool EmulateResult::canFlow() const { return m_canflow; }
const EmulateResult::Results& EmulateResult::results() const { return m_results; }
const RDBufferView* EmulateResult::view() const { return m_view; }
rd_address EmulateResult::address() const { return m_address; }
size_t EmulateResult::size() const { return m_size; }
void EmulateResult::setSize(size_t size) { m_size = size; }
size_t EmulateResult::delaySlot() const { return m_delayslot; }
void EmulateResult::setDelaySlot(size_t ds) { m_delayslot = ds; }
void EmulateResult::addReturn() { m_canflow = false; m_results.push_back({Return, {{0}, RD_NVAL}}); }
void EmulateResult::addBranchUnresolved() { m_canflow = false; m_results.push_back({BranchUnresolved, {{0}, RD_NVAL}});  }
void EmulateResult::addBranchIndirect() { m_canflow = false; m_results.push_back({BranchIndirect, {{0}, RD_NVAL}}); }
void EmulateResult::addBranch(rd_address address) { m_canflow = false; m_results.push_back({Branch, {{address} ,RD_NVAL}}); }
void EmulateResult::addBranchTrue(rd_address address) { m_canflow = false; m_results.push_back({BranchTrue, {{address}, RD_NVAL}}); }
void EmulateResult::addBranchFalse(rd_address address) { m_canflow = false; m_results.push_back({BranchFalse, {{address}, RD_NVAL}}); }
void EmulateResult::addBranchTable(rd_address address, size_t size) { m_results.push_back({BranchTable, {{address}, size }}); }
void EmulateResult::addSysCall(u64 n) { m_results.push_back({SysCall, {{n}, RD_NVAL}}); }
void EmulateResult::addCall(rd_address address) { m_results.push_back({Call, {{address}, RD_NVAL}}); }
void EmulateResult::addCallIndirect() { m_results.push_back({CallIndirect, {{0}, RD_NVAL}}); }
void EmulateResult::addCallUnresolved() { m_results.push_back({CallUnresolved, {{0}, RD_NVAL}}); }
void EmulateResult::addCallTable(rd_address address, size_t size) { m_results.push_back({CallTable, {{address}, size }}); }
void EmulateResult::addReferenceSize(rd_address address, size_t size) { m_results.push_back({Ref, {{address}, size}}); }
void EmulateResult::addTable(rd_address address, size_t size) { m_results.push_back({Table, {{address}, size}}); }
void EmulateResult::addReference(rd_address address) { this->addReferenceSize(address, RD_NVAL); }
void EmulateResult::addInvalid(size_t size) { m_invalid = true; m_size = size; }
