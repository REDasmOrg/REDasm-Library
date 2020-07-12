#include "containertests.h"
#include "../rdcore/containers/blockcontainer.h"
#include "../rdapi/rdapi.h"
#include <string>

void ContainerTests::testBlockContainer()
{
    ContainerTests::testBlockContainerMerge();
    ContainerTests::testBlockContainerOperations();
}

void ContainerTests::testBlockContainerMerge()
{
    BlockContainer<std::string, rd_address> c;

    c.free(1000, 1000);
    c.free(2000, 1000);
    c.free(3000, 1000);

    REQUIRE_EQ(c.size(), 1);
    ContainerTests::checkSorting(c);

    decltype(c)::Block b;

    for(auto start : { 1000, 1500, 2000, 2500, 3000, 2999}) {
        REQUIRE(c.get(start, &b));
        ContainerTests::checkBlock(b, true, 1000, 3000);
    }

    REQUIRE(!c.get(4000, &b));
}

void ContainerTests::testBlockContainerOperations()
{
    BlockContainer<std::string, rd_address> c;

    c.free(1000, 1000);
    c.free(3000, 1000);
    c.free(5000, 1000);

    REQUIRE_EQ(c.size(), 3);
    ContainerTests::checkSorting(c);

    decltype(c)::Block b;

    for(auto start : { 1000, 3000, 5000 }) {
        REQUIRE(c.get(start, &b));
        ContainerTests::checkBlock(b, true, start, 1000);
    }

    c.alloc(3500, 500, "alloc1");
    REQUIRE_EQ(c.size(), 4);
    ContainerTests::checkSorting(c);

    c.alloc(4500, 1000, "alloc2");
    REQUIRE_EQ(c.size(), 5);
    ContainerTests::checkSorting(c);

    REQUIRE(c.get(3499, &b));
    ContainerTests::checkBlock(b, true, 3000, 500);

    REQUIRE(c.get(3999, &b));
    ContainerTests::checkBlock(b, false, 3500, 500);
    REQUIRE_EQ(b.data, "alloc1");

    c.free(0, 6000);
    REQUIRE_EQ(c.size(), 1);
    REQUIRE(c.get(0, &b));
    ContainerTests::checkBlock(b, true, 0, 6000);
}
