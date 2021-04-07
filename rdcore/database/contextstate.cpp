#include "contextstate.h"
#include "../document/document.h"
#include "../serializer/writer.h"
#include "../support/endian.h"
#include "../context.h"
#include <rdapi/level.h>

#define CHUNK_CONTEXTSTATE "RDS"
#define CHUNK_SEGMENTS     "SEG"
#define CHUNK_SPACES       "SPACE"
#define CHUNK_BLOCKS       "BLOCK"

#pragma pack(push, 1)
struct ContextStateHeader
{
    apilevel_t apilevel;
    u32 nsegments;
};
#pragma pack(pop)

ContextState::ContextState(Context* ctx): Object(ctx) { }

bool ContextState::load(const std::string& filepath)
{
    if(filepath.empty()) return false;

    return false;
}

bool ContextState::save(const std::string& filepath)
{
    if(filepath.empty()) return false;

    const auto& doc = this->context()->document();

    SerializerWriter sw;
    this->serializeHeader(doc, sw);
    this->serializeSegments(doc, sw);

    if(!sw.save(filepath))
    {
        this->log(sw.lastError());
        return false;
    }

    return true;
}

void ContextState::serializeHeader(const SafeDocument& doc, SerializerWriter& writer) const
{
    ContextStateHeader csh;
    csh.apilevel = Endian::tolittleendian32(RDAPI_LEVEL);
    csh.nsegments = Endian::tolittleendian32(doc->getSegments(nullptr));
    writer.push(CHUNK_CONTEXTSTATE, &csh);
}

void ContextState::serializeSegments(const SafeDocument& doc, SerializerWriter& writer) const
{
    auto* aspace = doc->addressSpace();

    const rd_address* addresses = nullptr;
    size_t c = doc->getSegments(&addresses);

    for(size_t i = 0; i < c; i++)
    {
        RawData chunkdata(sizeof(RDSegment));
        doc->addressToSegment(addresses[i], reinterpret_cast<RDSegment*>(chunkdata.data()));
        writer.push(CHUNK_SEGMENTS, chunkdata);

        auto* buffer = aspace->getBuffer(addresses[i]);
        writer.push(CHUNK_SPACES, buffer->internalData());

        auto* blocks = aspace->getBlocks(addresses[i]);

        writer.push(CHUNK_BLOCKS, blocks->begin(), blocks->end(), [](const RDBlock* inb, RDBlock* outb) {
            outb->type = Endian::tolittleendian32(inb->type);
            outb->start = Endian::tolittleendian32(inb->start);
            outb->end = Endian::tolittleendian32(inb->end);
        });
    }
}
