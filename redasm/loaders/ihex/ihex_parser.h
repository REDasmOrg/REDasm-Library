#ifndef IHEX_PARSER_H
#define IHEX_PARSER_H

#include <vector>
#include <deque>
#include "../../types/buffer/bufferview.h"
#include "../../support/utils.h"

namespace REDasm {

enum IHexRecordType: u8 {
    First = 0x00,
    Data = First,
    Eof, ExtSeg, StartSeg, ExtLineAddr, StartLineAddr,
    Last = StartLineAddr
};

struct IHexLine {
    u8 length;
    u16 address;
    u8 record;
    std::vector<u8> bytes;
    u8 checksum;
};

class IHexParser
{
    public:
        IHexParser(const BufferView &view);
        u32 bits();
        bool test();

    private:
        bool readLine(const std::string &hexstring, u64 &i, IHexLine &line);
        template<typename T> bool readHex(const std::string& hexstring, T* res, u64& i);

    private:
        std::deque<IHexLine> m_lines;
        const BufferView& m_view;
        u8 m_maxrecord;
        bool m_test;
};

template<typename T> bool IHexParser::readHex(const std::string& hexstring, T* res, u64& i)
{
    u64 hexlen = sizeof(T) * 2;
    MemoryBuffer mb = REDasm::bytes(hexstring, i, hexlen);

    if(mb.empty())
        return false;

    std::copy_n(mb.data(), mb.size(), res);
    i += hexlen;
    return true;
}

} // namespace REDasm

#endif // IHEX_PARSER_H
