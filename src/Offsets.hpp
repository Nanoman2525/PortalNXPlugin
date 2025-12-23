#pragma once

#define EMPTY_OFFSET 0 // So that it is easier to tell if the actual index is 0 or nonexistent in the build

// extern the offsets
#define DECLARE_MODULE_ID(name, portal, portal2)
#define DECLARE_OFFSET(name, portal, portal2) extern int name;

namespace Offsets
{
    #include "Offsets/1_0_3.hpp"
}

#undef DECLARE_MODULE_ID
#undef DECLARE_OFFSET

// extern the module IDs
#define DECLARE_MODULE_ID(name, portal, portal2) extern const char *name;
#define DECLARE_OFFSET(name, portal, portal2)

#include "Offsets/1_0_3.hpp"

#undef DECLARE_MODULE_ID
#undef DECLARE_OFFSET


void InitOffsets(bool bIsGamePortal2);
