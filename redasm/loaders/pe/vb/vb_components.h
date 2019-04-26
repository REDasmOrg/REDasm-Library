#pragma once

#include "vb_header.h"
#include <string>
#include <vector>
#include <unordered_map>

namespace REDasm {

class VBComponents
{
    public:
        typedef std::deque<std::string> EventList;
        struct Component { std::string name; EventList events; };
        typedef std::unordered_map<std::string, Component> Components;

    public:
        VBComponents() = delete;
        static const Component* get(GUID* guid);

    private:
        static void initComponents();
        static std::string guidString(GUID* guid);

    private:
        static Components m_components;
};

} // namespace REDasm
