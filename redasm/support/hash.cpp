#include "hash.h"
#include <impl/support/hash_impl.h>

namespace REDasm {

u16 Hash::crc16(const BufferView *br) { return Hash::crc16(br->data(), br->size()); }
u16 Hash::crc16(const BufferView *br, size_t length) { return Hash::crc16(br->data(), std::min(br->size(), length)); }
u16 Hash::crc16(const std::string &s) { return Hash::crc16(reinterpret_cast<const u8*>(s.data()), s.size()); }
u16 Hash::crc16(const u8 *data, size_t length) { return HashImpl::crc16(data, length); }

u32 Hash::crc32(const BufferView *br) { return Hash::crc32(br->data(), br->size()); }
u32 Hash::crc32(const BufferView *br, size_t length) { return Hash::crc32(br->data(), std::min(br->size(), length));  }
u32 Hash::crc32(const std::string &s) { return Hash::crc32(reinterpret_cast<const u8*>(s.data()), s.size()); }
u32 Hash::crc32(const u8 *data, size_t length) { return HashImpl::crc32(data, length); }

} // namespace REDasm
