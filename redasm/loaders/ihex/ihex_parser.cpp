#include "ihex_parser.h"

namespace REDasm {

IHexParser::IHexParser(const BufferView &view): m_view(view), m_maxrecord(0), m_test(false) { }

u32 IHexParser::bits()
{
    if(m_maxrecord <= 1)
        return 8;

    if(m_maxrecord <= 3)
        return 16;

    return 32;
}

bool IHexParser::test()
{
    m_test = true;
    m_maxrecord = 0;

    if(*m_view.data() != ':')
        return false;

    std::string hexstring = m_view.toString();
    u64 i = 0;

    while(i < hexstring.size())
    {
        IHexLine line;

        if(!this->readLine(hexstring, i, line))
            return false;

        if(!m_test)
            m_lines.push_back(line);
    }

    return true;
}

bool IHexParser::readLine(const std::string& hexstring, u64& i, IHexLine& line)
{
    if(hexstring[i] != ':')
        return false;

    i++; // Skip ':'

    if(!this->readHex(hexstring, &line.length, i))
        return false;

    if(!this->readHex(hexstring, &line.address, i))
        return false;

    if(!this->readHex(hexstring, &line.record, i))
        return false;

    if(line.record > IHexRecordType::Last)
        return false;

    m_maxrecord = std::max(line.record, m_maxrecord);

    if(line.length)
    {
        line.bytes.resize(line.length);

        for(u8 j = 0; j < line.length; j++)
        {
            if(!this->readHex(hexstring, &line.bytes[j], i))
                return false;
        }
    }

    if(!this->readHex(hexstring, &line.checksum, i))
        return false;

    while(::isspace(hexstring[i]))
    {
        if(i >= hexstring.size())
            return false;

        i++;
    }

    return true;
}

} // namespace REDasm
