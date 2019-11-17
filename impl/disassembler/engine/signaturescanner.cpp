#include "signaturescanner.h"
#include <redasm/disassembler/listing/document/listingdocument.h>
#include <redasm/disassembler/disassembler.h>
#include <redasm/support/path.h>
#include <redasm/context.h>

namespace REDasm {

bool SignatureScanner::load(const String& signame)
{
    String signaturefile = Path::exists(signame) ? signame : r_ctx->signaturedb(signame);
    if(!signaturefile.endsWith(".json")) signaturefile += ".json";

    m_sigdb = SignatureDB();
    m_count.store(0);

    if(!m_sigdb.load(signaturefile))
    {
        r_ctx->log("Failed to load " + signaturefile.quoted());
        return false;
    }

    if(!m_sigdb.isCompatible())
    {
        r_ctx->log("Signature " + m_sigdb.name().quoted() + " is not compatible");
        return false;
    }

    r_ctx->log("Loading Signature: " + m_sigdb.name().quoted());
    return true;
}

void SignatureScanner::scan(const address_t address)
{
    const Symbol* symbol = r_doc->symbol(address);
    if(!symbol || !symbol->isWeak()) return;

    BufferView view = r_disasm->getFunctionBytes(address);
    auto location = r_ldr->offset(symbol->address);
    if(view.eob() || !location.valid) return;

    m_sigdb.search(view, [&](const nlohmann::json& signature) {
        String signame = signature["name"];
        r_doc->symbol(address, signame, static_cast<SymbolType>(signature["symboltype"]));
        m_count.fetch_add(1);
    });
}

size_t SignatureScanner::count() const { return m_count.load(); }

} // namespace REDasm
