#include "sugar.h"
#include <type_traits>

int Sugar::branchDirection(rd_address fromaddress, rd_address address)
{
    return static_cast<int>(static_cast<std::make_signed<decltype(address)>::type>(address) -
                            static_cast<std::make_signed<decltype(fromaddress)>::type>(fromaddress));
}
