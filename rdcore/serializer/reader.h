#pragma once

#include <unordered_map>
#include <optional>
#include <string>
#include <deque>
#include "common.h"

class SerializerReader: public SerializerCommon
{
    public:
       typedef std::deque<RawData> Result;

    private:
       typedef std::unordered_map<std::string, Result> Map;

    public:
        SerializerReader() = default;
        bool read(const std::string& filepath);
        std::optional<Result> find(const std::string& type) const;

    private:
        static std::string mktype(u32 type);

    private:
        Map m_chunks;
};

