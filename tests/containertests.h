#pragma once

#include "../rdcore/containers/blockcontainer.h"
#include "doctest.h"

class ContainerTests
{
    public:
        ContainerTests() = delete;
        static void testBlockContainer();

    private:
        static void testBlockContainerMerge();
        static void testBlockContainerOperations();

    private:
        template<typename BC> static void checkSorting(const BC& bc);
        template<typename B, typename Value> static void checkBlock(const B& b, bool free, Value start, Value size);
};

template<typename BC>
void ContainerTests::checkSorting(const BC& bc) {
    typename BC::Block prevblock{ };

    for(size_t i = 0; i < bc.size(); i++) {
        typename BC::Block block;
        REQUIRE(bc.at(i, &block));
        if(i) REQUIRE_LT(prevblock.start, block.start);
        prevblock = block;
    }
}

template<typename B, typename Value>
void ContainerTests::checkBlock(const B& b, bool free, Value start, Value size) {
    REQUIRE_EQ(b.free, free);
    REQUIRE_EQ(b.start, start);
    REQUIRE_EQ(b.size, size);
}
