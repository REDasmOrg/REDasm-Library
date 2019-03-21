#include "n64_analyzer.h"

namespace REDasm {

N64Analyzer::N64Analyzer(DisassemblerAPI* disassembler): Analyzer(disassembler) { }
void N64Analyzer::analyze() { Analyzer::analyze(); }

} // namespace REDasm
