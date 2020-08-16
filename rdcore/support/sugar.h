#pragma once

#include <rdapi/types.h>
#include <string>

class Sugar
{
    public:
        Sugar() = delete;
        static int branchDirection(rd_address fromaddress, rd_address address);
};

