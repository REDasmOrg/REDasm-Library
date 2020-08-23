#include "emulateresult.h"

EmulateResult::EmulateResult(rd_address address, const RDBufferView* view, Disassembler* disassembler): m_address(address), m_disassembler(disassembler), m_view(view) { }
bool EmulateResult::canFlow() const { return m_canflow; }
const EmulateResult::Results& EmulateResult::results() const { return m_results; }
Disassembler* EmulateResult::disassembler() const { return m_disassembler; }
const RDBufferView* EmulateResult::view() const { return m_view; }
rd_address EmulateResult::address() const { return m_address; }
size_t EmulateResult::size() const { return m_size; }
void EmulateResult::setSize(size_t size) { m_size = size; }
size_t EmulateResult::delaySlot() const { return m_delayslot; }
void EmulateResult::setDelaySlot(size_t ds) { m_delayslot = ds; }
void EmulateResult::addReturn() { m_canflow = false; m_results.push_back({Return, {0}}); }
void EmulateResult::addBranchUnresolved() { m_canflow = false; m_results.push_back({BranchUnresolved, {0}});  }
void EmulateResult::addBranchIndirect() { m_canflow = false; m_results.push_back({BranchIndirect, {0}}); }
void EmulateResult::addBranch(rd_address address) { m_canflow = false; m_results.push_back({Branch, {address}}); }
void EmulateResult::addBranchTrue(rd_address address) { m_canflow = false; m_results.push_back({BranchTrue, {address}}); }
void EmulateResult::addBranchFalse(rd_address address) { m_canflow = false; m_results.push_back({BranchFalse, {address}}); }
void EmulateResult::addSysCall(u64 n) { m_results.push_back({SysCall, {n}}); }
void EmulateResult::addCall(rd_address address) { m_results.push_back({Call, {address}}); }
void EmulateResult::addReference(rd_address address) { m_results.push_back({Ref, {address}}); }
