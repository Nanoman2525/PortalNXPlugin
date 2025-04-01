#ifndef NXMODULE_HPP
#define NXMODULE_HPP

#ifdef __SWITCH__

#include <cstdint>
#include <array>
#include <cstddef>

inline constexpr uint8_t parseHexByte(const char c[2])
{
    uint8_t highNibble = 0;
    if      (c[0] >= '0' && c[0] <= '9') highNibble = static_cast<uint8_t>(c[0] - '0');
    else if (c[0] >= 'A' && c[0] <= 'F') highNibble = static_cast<uint8_t>(c[0] - 'A' + 0xA);
    else if (c[0] >= 'a' && c[0] <= 'f') highNibble = static_cast<uint8_t>(c[0] - 'a' + 0xA);

    uint8_t lowNibble = 0;
    if      (c[1] >= '0' && c[1] <= '9') lowNibble = static_cast<uint8_t>(c[1] - '0');
    else if (c[1] >= 'A' && c[1] <= 'F') lowNibble = static_cast<uint8_t>(c[1] - 'A' + 0xA);
    else if (c[1] >= 'a' && c[1] <= 'f') lowNibble = static_cast<uint8_t>(c[1] - 'a' + 0xA);

    return (highNibble << 4) | lowNibble;
}

namespace nn
{
    namespace ro
    {
        void Finalize();

        namespace detail
        {
            // https://smo.monsterdruide.one/docs/ro_8h.html
            class RoModule
            {
            public:
                char _pad1[8];
                RoModule* next;
                char _pad2[16];
                uintptr_t baseAddress;
            };

            RoModule** GetModuleListHead();
            RoModule* FindRoModuleById(std::array<std::uint8_t, 20> moduleId);

            template<size_t N> inline constexpr RoModule* FindRoModuleById(const char (&moduleIdHex)[N])
            {
                static_assert(N == 41, "moduleIdHex must be 20 hex bytes long");

                // Convert it first into bytes
                std::array<std::uint8_t, 20> moduleId;
                for (size_t i = 0; i < 20; i++)
                {
                    moduleId[i] = parseHexByte(&moduleIdHex[i * 2]);
                }

                // Now find the module
                return FindRoModuleById(moduleId);
            }
        } // namespace detail
    } // namespace ro
} // namespace nn

// Module addresses
#ifdef PORTAL
// extern uintptr_t bsppacknrobase;
// extern uintptr_t bugreporter_filequeuenrobase;
// extern uintptr_t bugreporter_publicnrobase;
extern uintptr_t clientnrobase;
extern uintptr_t datacachenrobase;
extern uintptr_t enginenrobase;
extern uintptr_t filesystem_stdionrobase;
extern uintptr_t GameUInrobase;
extern uintptr_t inputsystemnrobase;
extern uintptr_t launchernrobase;
extern uintptr_t materialsystemnrobase;
extern uintptr_t scenefilecachenrobase;
extern uintptr_t servernrobase;
// extern uintptr_t ServerBrowsernrobase;
// extern uintptr_t shaderapidx9nrobase;
// extern uintptr_t shaderapiemptynrobase;
extern uintptr_t soundemittersystemnrobase;
// extern uintptr_t stdshader_dx9nrobase;
// extern uintptr_t studiorendernrobase;
extern uintptr_t tier0nrobase;
// extern uintptr_t toglnrobase;
extern uintptr_t vgui2nrobase;
extern uintptr_t vguimatsurfacenrobase;
extern uintptr_t video_nxnrobase;
extern uintptr_t video_servicesnrobase;
extern uintptr_t vphysicsnrobase;
extern uintptr_t vstdlibnrobase;
// extern uintptr_t vtex_dllnrobase;
#elifdef PORTAL2
// extern uintptr_t bsppacknrobase;
extern uintptr_t clientnrobase;
extern uintptr_t datacachenrobase;
extern uintptr_t enginenrobase;
extern uintptr_t filesystem_stdionrobase;
extern uintptr_t inputsystemnrobase;
extern uintptr_t launchernrobase;
extern uintptr_t localizenrobase;
extern uintptr_t matchmakingnrobase;
extern uintptr_t materialsystemnrobase;
extern uintptr_t scenefilecachenrobase;
extern uintptr_t servernrobase;
// extern uintptr_t shaderapiemptynrobase;
extern uintptr_t soundemittersystemnrobase;
extern uintptr_t studiorendernrobase;
extern uintptr_t tier0nrobase;
extern uintptr_t vgui2nrobase;
extern uintptr_t vguimatsurfacenrobase;
extern uintptr_t vphysicsnrobase;
extern uintptr_t vscriptnrobase;
extern uintptr_t vstdlibnrobase;
#endif

extern bool InitNXModuleBases();

#endif // __SWITCH__

#endif // NXMODULE_HPP
