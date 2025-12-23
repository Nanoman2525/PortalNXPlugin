#include "Offsets.hpp"

// Declare the offsets
#define DECLARE_MODULE_ID(name, portal, portal2)
#define DECLARE_OFFSET(name, portal, portal2) int name;

namespace Offsets
{
    #include "Offsets/1_0_3.hpp"
}

#undef DECLARE_MODULE_ID
#undef DECLARE_OFFSET

// extern the module IDs
#define DECLARE_MODULE_ID(name, portal, portal2) const char *name;
#define DECLARE_OFFSET(name, portal, portal2)

#include "Offsets/1_0_3.hpp"

#undef DECLARE_MODULE_ID
#undef DECLARE_OFFSET

void InitOffsets(bool bIsGamePortal2)
{
    if (bIsGamePortal2)
    {
#define DECLARE_MODULE_ID(name, portal, portal2) name = portal2;
#define DECLARE_OFFSET(name, portal, portal2) Offsets::name = portal2;
        #include "Offsets/1_0_3.hpp"
#undef DECLARE_MODULE_ID
#undef DECLARE_OFFSET
    }
    else
    {
#define DECLARE_MODULE_ID(name, portal, portal2) name = portal;
#define DECLARE_OFFSET(name, portal, portal2) Offsets::name = portal;
        #include "Offsets/1_0_3.hpp"
#undef DECLARE_MODULE_ID
#undef DECLARE_OFFSET
    }
}
