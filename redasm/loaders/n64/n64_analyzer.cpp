#include "n64_analyzer.h"

namespace REDasm {

N64Analyzer::N64Analyzer(DisassemblerAPI* disassembler, const SignatureFiles& signaturefiles): Analyzer(disassembler, signaturefiles)
{

}

void N64Analyzer::analyze()
{
    Analyzer::analyze();
}

} // namespace REDasm
