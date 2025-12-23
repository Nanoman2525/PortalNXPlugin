#include "NXModule.hpp"
#include "Offsets.hpp"

#ifdef __SWITCH__

#include <cstring> // std::memcmp

nn::ro::detail::RoModule** nn::ro::detail::GetModuleListHead()
{
    const uintptr_t insns_base = reinterpret_cast<uintptr_t>(&nn::ro::Finalize) + 0x2C;
    const uint32_t *insns = reinterpret_cast<uint32_t *>(insns_base);

    // Extract the ADRP and LDR instructions to compute the module list pointer
    const uintptr_t adrp_value = (insns_base & ~(0xFFF)) + ((insns[0] >> 5 & 0x7FFFF) << 14) + ((insns[0] >> 29 & 2) << 12);
    const uintptr_t ldr_offset = (insns[1] >> 10 & 0xFFF) << (insns[1] >> 30);

    return *reinterpret_cast<RoModule ***>(adrp_value + ldr_offset);
}

nn::ro::detail::RoModule* nn::ro::detail::FindRoModuleById(std::array<std::uint8_t, 20> moduleId)
{
    RoModule *pHead = *GetModuleListHead();
    for (RoModule* moduleEntry = pHead; moduleEntry; moduleEntry = moduleEntry->next)
    {
		// https://switchbrew.org/wiki/NRO
        // Check against "nn::ro::detail::ModuleId", located 0x40 within the module itself
        if (!std::memcmp(reinterpret_cast<uint8_t *>(moduleEntry->baseAddress + 0x40), moduleId.data(), 20))
        {
            return moduleEntry; // Module with the correct ID found
        }

        if (moduleEntry->next == pHead)
        {
            break; // We went through them all
        }
    }

    // Module not found
    return nullptr;
}

nn::ro::detail::RoModule* nn::ro::detail::FindRoModuleById(const char *id)
{
	// Convert it first into bytes
	std::array<std::uint8_t, 20> moduleId;
	for (size_t i = 0; i < 20; i++)
	{
		moduleId[i] = parseHexByte(&id[i * 2]);
	}

	// Now find the module
    return nn::ro::detail::FindRoModuleById(moduleId);
}

// Modules shared between both games...
// uintptr_t bsppacknrobase = 0;
uintptr_t clientnrobase = 0;
uintptr_t datacachenrobase = 0;
uintptr_t enginenrobase = 0;
uintptr_t filesystem_stdionrobase = 0;
uintptr_t inputsystemnrobase = 0;
uintptr_t launchernrobase = 0;
uintptr_t materialsystemnrobase = 0;
uintptr_t scenefilecachenrobase = 0;
uintptr_t servernrobase = 0;
// uintptr_t shaderapiemptynrobase = 0;
uintptr_t soundemittersystemnrobase = 0;
uintptr_t studiorendernrobase = 0;
uintptr_t tier0nrobase = 0;
uintptr_t vgui2nrobase = 0;
uintptr_t vguimatsurfacenrobase = 0;
uintptr_t vphysicsnrobase = 0;
uintptr_t vstdlibnrobase = 0;

// Portal-specific modules...
// uintptr_t bugreporter_filequeuenrobase = 0;
// uintptr_t bugreporter_publicnrobase = 0;
uintptr_t GameUInrobase = 0;
// uintptr_t ServerBrowsernrobase = 0;
// uintptr_t shaderapidx9nrobase = 0;
// uintptr_t stdshader_dx9nrobase = 0;
// uintptr_t toglnrobase = 0;
uintptr_t video_nxnrobase = 0;
uintptr_t video_servicesnrobase = 0;
// uintptr_t vtex_dllnrobase = 0;

// Portal 2-specific modules...
uintptr_t localizenrobase = 0;
uintptr_t matchmakingnrobase = 0;
uintptr_t vscriptnrobase = 0;

bool InitNXModuleBases(bool bIsPortal2Build)
{
	// Init shared modules
	// bsppacknrobase =				nn::ro::detail::FindRoModuleById(bsppack_nroID)->baseAddress;
	clientnrobase =					nn::ro::detail::FindRoModuleById(client_nroID)->baseAddress;
	datacachenrobase =				nn::ro::detail::FindRoModuleById(datacache_nroID)->baseAddress;
	enginenrobase =					nn::ro::detail::FindRoModuleById(engine_nroID)->baseAddress;
	filesystem_stdionrobase =		nn::ro::detail::FindRoModuleById(filesystem_stdio_nroID)->baseAddress;
	inputsystemnrobase =			nn::ro::detail::FindRoModuleById(inputsystem_nroID)->baseAddress;
	launchernrobase =				nn::ro::detail::FindRoModuleById(launcher_nroID)->baseAddress;
	materialsystemnrobase =			nn::ro::detail::FindRoModuleById(materialsystem_nroID)->baseAddress;
	scenefilecachenrobase =			nn::ro::detail::FindRoModuleById(scenefilecache_nroID)->baseAddress;
	servernrobase =					nn::ro::detail::FindRoModuleById(server_nroID)->baseAddress;
	// shaderapiemptynrobase =			nn::ro::detail::FindRoModuleById(shaderapiempty_nroID)->baseAddress;
	soundemittersystemnrobase =		nn::ro::detail::FindRoModuleById(soundemittersystem_nroID)->baseAddress;
	tier0nrobase =					nn::ro::detail::FindRoModuleById(tier0_nroID)->baseAddress;
	vgui2nrobase =					nn::ro::detail::FindRoModuleById(vgui2_nroID)->baseAddress;
	vguimatsurfacenrobase =			nn::ro::detail::FindRoModuleById(vguimatsurface_nroID)->baseAddress;
	vphysicsnrobase =				nn::ro::detail::FindRoModuleById(vphysics_nroID)->baseAddress;
	vstdlibnrobase =				nn::ro::detail::FindRoModuleById(vstdlib_nroID)->baseAddress;

	// Init modules unique to their games
	if (!bIsPortal2Build)
	{
		// bugreporter_filequeuenrobase =	nn::ro::detail::FindRoModuleById(bugreporter_filequeue_nroID)->baseAddress;
		// bugreporter_publicnrobase =		nn::ro::detail::FindRoModuleById(bugreporter_public_nroID)->baseAddress;
		GameUInrobase =					nn::ro::detail::FindRoModuleById(GameUI_nroID)->baseAddress;
		// ServerBrowsernrobase =			nn::ro::detail::FindRoModuleById(ServerBrowser_nroID)->baseAddress;
		// shaderapidx9nrobase =			nn::ro::detail::FindRoModuleById(shaderapidx9_nroID)->baseAddress;
		// stdshader_dx9nrobase =			nn::ro::detail::FindRoModuleById(stdshader_dx9_nroID)->baseAddress;
		// studiorendernrobase =			nn::ro::detail::FindRoModuleById(studiorender_nroID)->baseAddress;
		// toglnrobase =					nn::ro::detail::FindRoModuleById(togl_nroID)->baseAddress;
		video_nxnrobase =				nn::ro::detail::FindRoModuleById(video_nx_nroID)->baseAddress;
		video_servicesnrobase =			nn::ro::detail::FindRoModuleById(video_services_nroID)->baseAddress;
		// vtex_dllnrobase =				nn::ro::detail::FindRoModuleById(vtex_dll_nroID)->baseAddress;

		if (/*!bsppacknrobase || !bugreporter_filequeuenrobase || !bugreporter_publicnrobase ||*/ !clientnrobase ||
			!datacachenrobase || !enginenrobase || !filesystem_stdionrobase || !GameUInrobase || !inputsystemnrobase ||
			!launchernrobase || !materialsystemnrobase || !scenefilecachenrobase || !servernrobase || /*!ServerBrowsernrobase ||
			!shaderapidx9nrobase || !shaderapiemptynrobase ||*/ !soundemittersystemnrobase || /*!stdshader_dx9nrobase ||
			!studiorendernrobase ||*/ !tier0nrobase || /*!toglnrobase ||*/ !vgui2nrobase || !vguimatsurfacenrobase ||
			!video_nxnrobase || !video_servicesnrobase || !vphysicsnrobase || !vstdlibnrobase /*|| !vtex_dllnrobase*/ )
		{
			return false; // Something failed
		}
	}
	else
	{
		localizenrobase =				nn::ro::detail::FindRoModuleById(localize_nroID)->baseAddress;
		matchmakingnrobase = 			nn::ro::detail::FindRoModuleById(matchmaking_nroID)->baseAddress;
		studiorendernrobase =			nn::ro::detail::FindRoModuleById(studiorender_nroID)->baseAddress;
		vscriptnrobase = 				nn::ro::detail::FindRoModuleById(vscript_nroID)->baseAddress;

		if (/*!bsppacknrobase ||*/ !clientnrobase || !datacachenrobase || !enginenrobase || !filesystem_stdionrobase ||
			!inputsystemnrobase || !launchernrobase || !localizenrobase || !matchmakingnrobase || !materialsystemnrobase ||
			!scenefilecachenrobase || !servernrobase || /*!shaderapiemptynrobase ||*/ !soundemittersystemnrobase ||
			!studiorendernrobase || !tier0nrobase || !vgui2nrobase || !vguimatsurfacenrobase || !vphysicsnrobase ||
			!vscriptnrobase || !vstdlibnrobase)
		{
			return false; // Something failed
		}
	}

	return true;
}

#endif // __SWITCH__
