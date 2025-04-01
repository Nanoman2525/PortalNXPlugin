#include "NXModule.hpp"

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

// Module addresses
#ifdef PORTAL
// uintptr_t bsppacknrobase = 0;
// uintptr_t bugreporter_filequeuenrobase = 0;
// uintptr_t bugreporter_publicnrobase = 0;
uintptr_t clientnrobase = 0;
uintptr_t datacachenrobase = 0;
uintptr_t enginenrobase = 0;
uintptr_t filesystem_stdionrobase = 0;
uintptr_t GameUInrobase = 0;
uintptr_t inputsystemnrobase = 0;
uintptr_t launchernrobase = 0;
uintptr_t materialsystemnrobase = 0;
uintptr_t scenefilecachenrobase = 0;
uintptr_t servernrobase = 0;
// uintptr_t ServerBrowsernrobase = 0;
// uintptr_t shaderapidx9nrobase = 0;
// uintptr_t shaderapiemptynrobase = 0;
uintptr_t soundemittersystemnrobase = 0;
// uintptr_t stdshader_dx9nrobase = 0;
// uintptr_t studiorendernrobase = 0;
uintptr_t tier0nrobase = 0;
// uintptr_t toglnrobase = 0;
uintptr_t vgui2nrobase = 0;
uintptr_t vguimatsurfacenrobase = 0;
uintptr_t video_nxnrobase = 0;
uintptr_t video_servicesnrobase = 0;
uintptr_t vphysicsnrobase = 0;
uintptr_t vstdlibnrobase = 0;
// uintptr_t vtex_dllnrobase = 0;
#elifdef PORTAL2
// uintptr_t bsppacknrobase = 0;
uintptr_t clientnrobase = 0;
uintptr_t datacachenrobase = 0;
uintptr_t enginenrobase = 0;
uintptr_t filesystem_stdionrobase = 0;
uintptr_t inputsystemnrobase = 0;
uintptr_t launchernrobase = 0;
uintptr_t localizenrobase = 0;
uintptr_t matchmakingnrobase = 0;
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
uintptr_t vscriptnrobase = 0;
uintptr_t vstdlibnrobase = 0;
#endif

bool InitNXModuleBases()
{
#ifdef PORTAL
	// bsppacknrobase =				nn::ro::detail::FindRoModuleById("F62C71FD880039241B72BC01BB244CF3A47D8D63")->baseAddress;
	// bugreporter_filequeuenrobase =	nn::ro::detail::FindRoModuleById("2E45E923CBDC74A2256CB9A5BA029B2117289FD1")->baseAddress;
	// bugreporter_publicnrobase =		nn::ro::detail::FindRoModuleById("1CF400F4DED967183924C1571C3E56ECB7C8FFB8")->baseAddress;
	clientnrobase =					nn::ro::detail::FindRoModuleById("A3CD70076187F6FFBD2FC717F31EA6BDF0399030")->baseAddress;
	datacachenrobase =				nn::ro::detail::FindRoModuleById("C307565F3A22FE01111A35FD2640B47F4A38349C")->baseAddress;
	enginenrobase =					nn::ro::detail::FindRoModuleById("DE2C23C74EF3B25D042EF3552F3ADF2E67C056CE")->baseAddress;
	filesystem_stdionrobase =		nn::ro::detail::FindRoModuleById("3CC4A49851522FB6FB4F52FA59FBDD7965BE5170")->baseAddress;
	GameUInrobase =					nn::ro::detail::FindRoModuleById("D9B827AEA1993139DD84156232013E04A3B010DE")->baseAddress;
	inputsystemnrobase =			nn::ro::detail::FindRoModuleById("5568A4F593ECAF4B59045747BC491C90A88CB155")->baseAddress;
	launchernrobase =				nn::ro::detail::FindRoModuleById("F280FC4D9888240EEF6076D43C83730C609C32D4")->baseAddress;
	materialsystemnrobase =			nn::ro::detail::FindRoModuleById("4CF824FA3C83CE46CA5110A4BEB300757FC57EA2")->baseAddress;
	scenefilecachenrobase =			nn::ro::detail::FindRoModuleById("920B2F41D260247874367E976FB478E33DC0AA45")->baseAddress;
	servernrobase =					nn::ro::detail::FindRoModuleById("A74E2D73E62E187F0F0422482BD37ED8F0FA0777")->baseAddress;
	// ServerBrowsernrobase =			nn::ro::detail::FindRoModuleById("DC24B479B2088435FCC934996CBE74DF2F0B5D69")->baseAddress;
	// shaderapidx9nrobase =			nn::ro::detail::FindRoModuleById("3CCFA5C57F131DDBCB8DCB5E9524286D1F08CB28")->baseAddress;
	// shaderapiemptynrobase =			nn::ro::detail::FindRoModuleById("158A4A226B7A54F1D04BF5EFBE8DDC25FFCF9E29")->baseAddress;
	soundemittersystemnrobase =		nn::ro::detail::FindRoModuleById("ECCF0202B32DB838C3D2549C552CC701C4772C8D")->baseAddress;
	// stdshader_dx9nrobase =			nn::ro::detail::FindRoModuleById("F0410B77CCEA2ACD5BB9E22CDA80FD5467F6E799")->baseAddress;
	// studiorendernrobase =			nn::ro::detail::FindRoModuleById("22C55354951FBDB140EDCAEB1A168752C808DB16")->baseAddress;
	tier0nrobase =					nn::ro::detail::FindRoModuleById("9F9DEA234465B275792AF3BE6322755980EF4990")->baseAddress;
	// toglnrobase =					nn::ro::detail::FindRoModuleById("2B8E9FDC3CC430CEC0140780FBDCEED4CE0C6FD9")->baseAddress;
	vgui2nrobase =					nn::ro::detail::FindRoModuleById("E16C9DD5C6B7793D639C41D27B5626C7129FA627")->baseAddress;
	vguimatsurfacenrobase =			nn::ro::detail::FindRoModuleById("7E5807163B3217FF519FD4FA8C40EBA5A50C52DF")->baseAddress;
	video_nxnrobase =				nn::ro::detail::FindRoModuleById("AA89C985D773468DA9B94C215EB55BE7F9B59C95")->baseAddress;
	video_servicesnrobase =			nn::ro::detail::FindRoModuleById("3E78EEB40E14D5D1DE88BC4D7CC2EF79D6172D6F")->baseAddress;
	vphysicsnrobase =				nn::ro::detail::FindRoModuleById("684588BA8FF297A1003EFA4CBD236E1F40D068B1")->baseAddress;
	vstdlibnrobase =				nn::ro::detail::FindRoModuleById("011821AAC93B862D1361F4CFB399EFA97BC5E10F")->baseAddress;
	// vtex_dllnrobase =				nn::ro::detail::FindRoModuleById("DE8584EDAEBE6010663E0BAE85DB59DAF118442F")->baseAddress;
#elifdef PORTAL2
	// bsppacknrobase =				nn::ro::detail::FindRoModuleById("A7B9C386738256724BACEED44CBB1D6854BECF33")->baseAddress;
	clientnrobase =					nn::ro::detail::FindRoModuleById("ABC37131A88D191BE4CB813EE84E11B21B94FC2C")->baseAddress;
	datacachenrobase = 				nn::ro::detail::FindRoModuleById("4027DD610E3D38EE1EDA9905B01B832FCEF0D766")->baseAddress;
	enginenrobase =					nn::ro::detail::FindRoModuleById("12820975A58BAE4A9EAE7AA82E5C1DCCAE260A19")->baseAddress;
	filesystem_stdionrobase = 		nn::ro::detail::FindRoModuleById("60A9E75E3662E6A48A24F7C958257F2639D516BC")->baseAddress;
	inputsystemnrobase = 			nn::ro::detail::FindRoModuleById("7A53B88C390C142866ADDFE5F45B9DBFDFBCB1E3")->baseAddress;
	launchernrobase = 				nn::ro::detail::FindRoModuleById("B857C14E84176F5BD6C518640DD9AA2908D6CFFA")->baseAddress;
	localizenrobase =				nn::ro::detail::FindRoModuleById("04EB58AF45B255EC0C7CFAE0BC012BC104058862")->baseAddress;
	matchmakingnrobase = 			nn::ro::detail::FindRoModuleById("6CF7AD9EFCD6308002DAF158C91653F26087C6A4")->baseAddress;
	materialsystemnrobase =			nn::ro::detail::FindRoModuleById("69BD850200ACBBFF7D6B6EF312D64E53FA73680B")->baseAddress;
	scenefilecachenrobase =			nn::ro::detail::FindRoModuleById("D44A4378FFF04D7938CE29CBB40252B29C1F411C")->baseAddress;
	servernrobase =					nn::ro::detail::FindRoModuleById("42D59B8BF6F02B1FE75ABF5FA3A1435713047DEA")->baseAddress;
	// shaderapiemptynrobase =			nn::ro::detail::FindRoModuleById("97E6F3B867D20CFA477A363A97D80E658FC50157")->baseAddress;
	soundemittersystemnrobase =		nn::ro::detail::FindRoModuleById("1F08714AC92B1BB2ACA56A1C32E08754D460EA77")->baseAddress;
	studiorendernrobase =			nn::ro::detail::FindRoModuleById("E4B6C3C75DCD40F2EBFEFC973207617C89D89690")->baseAddress;
	tier0nrobase = 					nn::ro::detail::FindRoModuleById("41C209763789790E429719CE754C71DB78124048")->baseAddress;
	vgui2nrobase =					nn::ro::detail::FindRoModuleById("A83F79040F84193C2F21A6C39D3A6BAB1D5219BD")->baseAddress;
	vguimatsurfacenrobase =			nn::ro::detail::FindRoModuleById("4E6E411761F7287DAE9F6AA90217924EE28D4F44")->baseAddress;
	vphysicsnrobase = 				nn::ro::detail::FindRoModuleById("C01B27E514E9AEB01E6AA982C106C685EBD7D91A")->baseAddress;
	vscriptnrobase = 				nn::ro::detail::FindRoModuleById("75010F2CCA80434E3C5800D929F3E4067241728D")->baseAddress;
	vstdlibnrobase = 				nn::ro::detail::FindRoModuleById("98DCFE5D1F736DC1971DF35429AB5265F590ABC4")->baseAddress;
#endif

#ifdef PORTAL
	if (/*!bsppacknrobase || !bugreporter_filequeuenrobase || !bugreporter_publicnrobase ||*/ !clientnrobase ||
		!datacachenrobase || !enginenrobase || !filesystem_stdionrobase || !GameUInrobase || !inputsystemnrobase ||
		!launchernrobase || !materialsystemnrobase || !scenefilecachenrobase || !servernrobase || /*!ServerBrowsernrobase ||
		!shaderapidx9nrobase || !shaderapiemptynrobase ||*/ !soundemittersystemnrobase || /*!stdshader_dx9nrobase ||
		!studiorendernrobase ||*/ !tier0nrobase || /*!toglnrobase ||*/ !vgui2nrobase || !vguimatsurfacenrobase ||
		!video_nxnrobase || !video_servicesnrobase || !vphysicsnrobase || !vstdlibnrobase /*|| !vtex_dllnrobase*/ )
#elifdef PORTAL2
	if (/*!bsppacknrobase ||*/ !clientnrobase || !datacachenrobase || !enginenrobase || !filesystem_stdionrobase ||
		!inputsystemnrobase || !launchernrobase || !localizenrobase || !matchmakingnrobase || !materialsystemnrobase ||
		!scenefilecachenrobase || !servernrobase || /*!shaderapiemptynrobase ||*/ !soundemittersystemnrobase ||
		!studiorendernrobase || !tier0nrobase || !vgui2nrobase || !vguimatsurfacenrobase || !vphysicsnrobase ||
		!vscriptnrobase || !vstdlibnrobase)
#endif
	{
		return false; // Something failed
	}

	return true;
}

#endif // __SWITCH__
