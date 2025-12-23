#include "NXModule.hpp"
#include "Variable.hpp"
#include "Offsets.hpp"
#include "Plugin.hpp"
#include "Command.hpp"

// Simple defines to make it easier to create a hook a function.
#define DECLARE_HOOK_FUNC( returnType, FuncName, ... ) \
static returnType(*FuncName##_Original)(__VA_ARGS__) = nullptr; \
returnType FuncName##_Hook(__VA_ARGS__) \

#define DECLARE_MEMBER_HOOK_FUNC( returnType, ClassName, FuncName, ... ) \
static returnType(*ClassName##__##FuncName##_Original)(__VA_ARGS__) = nullptr; \
returnType ClassName##__##FuncName##_Hook(__VA_ARGS__) \

//---------------------------------------------------------------------------------
// Purpose: Define a template for replacing functions within the code.
//---------------------------------------------------------------------------------
template <typename T>
void ToggleDetour(void* targetAddress, T& original, T detour, bool enable)
{
    if (enable)
    {
        if (original == nullptr)
        {
            // Save the original function pointer
            original = *(T*)targetAddress;
        }

        // Apply the detour (overwrite target address)
        *(T*)targetAddress = detour;
    }
    else
    {
        if (original != nullptr)
        {
            // Restore the original function pointer
            *(T*)targetAddress = original;
            original = nullptr;
        }
    }
}

//---------------------------------------------------------------------------------
// Purpose: Enable in-game mouse movement reading from the Switch. (Excludes menus)
//---------------------------------------------------------------------------------
extern Variable nx_enable_mouse_support;
extern void ScaleMouse(nn::hid::MouseState &mouseState);
DECLARE_MEMBER_HOOK_FUNC( void, CEngineClient, GetMouseDelta, void *thisptr, int &x, int &y, bool bIgnoreNextMouseDelta )
{
    if (nx_enable_mouse_support.GetBool())
    {
        nn::hid::MouseState mouseState;
        nn::hid::detail::GetMouseState(&mouseState);

        // Note: These mouse values are only ever consistent with the handheld screen size resolution
        // Make it work properly when docked
        ScaleMouse(mouseState);

        static int32_t oldx = 0;
        static int32_t oldy = 0;

        x = (oldx - mouseState.x) * -1;
        y = (oldy - mouseState.y) * -1;

        oldx = mouseState.x;
        oldy = mouseState.y;
    }
    else
    {
        CEngineClient__GetMouseDelta_Original(thisptr, x, y, bIgnoreNextMouseDelta);
    }
}

//---------------------------------------------------------------------------------
// Purpose: Hook to fix fire double mouse click events.
// TODO:    This double press code should apply to all mouse buttons, not just MOUSE_LEFT.
//---------------------------------------------------------------------------------
DECLARE_MEMBER_HOOK_FUNC(bool, CInputSystem, InternalMousePressed, void *thisptr, int code)
{
    // The thisptr is the same as g_pInput:
    // Portal 2: *(void **)(clientnrobase + 0x1641320);

    // We do this here, since it does not work through normal input event means.
    const int MOUSE_LEFT = 107;
    if (code == MOUSE_LEFT)
    {
        static auto Plat_FloatTime = (double (*)())(enginenrobase + Offsets::Plat_FloatTime);
        static double prevDoublePressTime = 0;

        double currentTime = Plat_FloatTime();
        double timeDiff = currentTime - prevDoublePressTime;
        prevDoublePressTime = currentTime;

        if (timeDiff > 0 && timeDiff <= 0.3)
        {
            // Code is most similar to CInputOSX::InternalMouseDoublePressed
            auto CInputSystem__InternalMouseDoublePressed = reinterpret_cast<bool (*)(void *, int)>(reinterpret_cast<uintptr_t **>(vgui2nrobase + Offsets::CInputSystem__vtable)[Offsets::CInputSystem__InternalMouseDoublePressed_vtable_index]);
            CInputSystem__InternalMouseDoublePressed(thisptr, code);
        }
    }

    return CInputSystem__InternalMousePressed_Original(thisptr, code);
}

//---------------------------------------------------------------------------------
// Purpose: Hook to fix cursor crash on menu option hover
// TODO:    Re-add the implementation to select it
//---------------------------------------------------------------------------------
DECLARE_MEMBER_HOOK_FUNC(void, MenuItem, OnCursorEntered, void *thisptr)
{
    // Note: Forwarding straight to the target func doesn't crash the game, but also doesn't do anything
    // TODO: Remove the defines to make compatible with both builds
//     auto Panel__GetVParent = (*(void *(**)(void *))(*(uintptr_t **)thisptr + Offsets::Panel__GetVParent));
// #ifdef PORTAL2
//     auto Menu__OnCursorEnteredMenuItem = (void (*)(void *thisptr, void *VPanel))(clientnrobase + 0x715AA4);
// #elifdef PORTAL
//     auto Menu__OnCursorEnteredMenuItem = (void (*)(void *thisptr, void *VPanel))(GameUInrobase + 0xE8CB0);
// #endif
//     auto GetVPanel = (**(void *(***)(void *))thisptr);

//     Menu__OnCursorEnteredMenuItem(Panel__GetVParent(thisptr), GetVPanel(thisptr));

    //---------------

    // Instead, we will remake Menu::OnCursorEnteredMenuItem...

    // MenuItem::ArmItem
    auto MenuItem__ArmItem = *reinterpret_cast<void(**)(void *thisptr)>(*reinterpret_cast<uintptr_t*>(thisptr) + Offsets::MenuItem__ArmItem);
    MenuItem__ArmItem(thisptr);

    // MenuItem::OpenCascadeMenu
    void *m_pCascadeMenu = *(void **)(reinterpret_cast<uintptr_t>(thisptr) + Offsets::MenuItem__m_pCascadeMenu);
    if (m_pCascadeMenu)
    {
        (*(void (**)(void *))(*reinterpret_cast<uintptr_t *>(m_pCascadeMenu) + Offsets::Menu__PerformLayout))(m_pCascadeMenu); // m_pCascadeMenu->PerformLayout();
        (*(void (**)(void *, bool))(*reinterpret_cast<uintptr_t *>(m_pCascadeMenu) + 272))(m_pCascadeMenu, true); // m_pCascadeMenu->SetVisible(true);
        MenuItem__ArmItem(thisptr);
    }

    // Menu::SetCurrentlySelectedItem
    // TODO
}

DECLARE_MEMBER_HOOK_FUNC(void, MenuItem, OnCursorExited, void *thisptr)
{
    // Note: Forwarding straight to the target func doesn't crash the game, but also doesn't do anything
    // auto Panel__GetVParent = (*(void *(**)(void *))(*(uintptr_t **)thisptr + Offsets::Panel__GetVParent));

    // uintptr_t baseModule = g_Plugin.IsGamePortal2() ? clientnrobase : GameUInrobase;
    // auto Menu__OnCursorExitedMenuItem = (void (*)(void *thisptr, void *VPanel))(baseModule + Offsets::Menu__OnCursorExitedMenuItem);
    // auto GetVPanel = (**(void *(***)(void *))thisptr);

    // Menu__OnCursorExitedMenuItem(Panel__GetVParent(thisptr), GetVPanel(thisptr));

    //---------------

    // Instead, we will remake Menu::OnCursorExitedMenuItem...

    auto MenuItem__DisarmItem = *reinterpret_cast<void(**)(void *thisptr)>(*reinterpret_cast<uintptr_t*>(thisptr) + Offsets::MenuItem__DisarmItem);
    MenuItem__DisarmItem(thisptr);
}

//---------------------------------------------------------------------------------
// Purpose: Hook to fix console printing in Portal.
// TODO:    Re-add the implementation for color and dev-only printing support.
//---------------------------------------------------------------------------------
DECLARE_HOOK_FUNC(int, vsnprintf, char *s, size_t maxlen, const char *format, __gnuc_va_list arg)
{
    int iOriginal = vsnprintf_Original(s, maxlen, format, arg);

    // Actually make the info print to the console
    uintptr_t *thisptr = reinterpret_cast<uintptr_t **>(*reinterpret_cast<uintptr_t **>(GameUInrobase + Offsets::CConsolePanel__globalptr))[Offsets::CConsolePanel__ptr_index];
    auto CConsolePanel__Print = reinterpret_cast<void (*)(uintptr_t *, const char *)>(reinterpret_cast<void **>(*thisptr)[Offsets::CConsolePanel__Print_vtable_index]);
    CConsolePanel__Print(thisptr, s);

    return iOriginal;
}

//---------------------------------------------------------------------------------
// Purpose: Hook to allow using ladders for all players.
//---------------------------------------------------------------------------------
extern void *g_GameMovement;
Variable nx_enable_ladders("nx_enable_ladders", "0", "Enables ladder usage for all players.");
DECLARE_MEMBER_HOOK_FUNC(bool, CPortalGameMovement, GameHasLadders)
{
    return nx_enable_ladders.GetBool();
}

//---------------------------------------------------------------------------------
// Purpose: Allow us to control the max player limit when starting a server.
//---------------------------------------------------------------------------------
extern "C" void Warning( const char* pMsg, ... );
Variable nx_max_players_override("nx_max_players_override", "0", "1-33: Max player count when starting a MP server. Otherwise: Game decides.");
DECLARE_MEMBER_HOOK_FUNC(void, CGameServer, SetMaxClients, void *thisptr, int number)
{
	int iNewMaxPlayerLimit = nx_max_players_override.GetInt();
	if ( iNewMaxPlayerLimit && iNewMaxPlayerLimit >= 1 && iNewMaxPlayerLimit <= 33 )
	{
		// We must check if this is the coop game mode, which is determined by a ConVar
		Variable sv_portal_players( "sv_portal_players" );
		if ( sv_portal_players.GetInt() > 1 )
		{
			// ConVar override value was changed to something valid, so let's use it.
            *(unsigned int*)(enginenrobase + Offsets::CGameServer__m_nMaxClientsLimit_static_offset) = iNewMaxPlayerLimit; // Set CGameServer::m_nMaxClientsLimit (Switch uses a static offset)
            number = iNewMaxPlayerLimit;
		}
		else
		{
			Warning( "nx_max_players_override - Refusing max player override, due to a singleplayer gamemode load!\n" );
		}
	}

    // Now that the game knows of this upper bound, set it before we load into our server
    CGameServer__SetMaxClients_Original( thisptr, number );
}

//---------------------------------------------------------------------------------
// Purpose: Restore menu footer button functionality.
//---------------------------------------------------------------------------------
DECLARE_MEMBER_HOOK_FUNC(void, CBaseModFooterPanel, OnCommand, void *thisptr, const char *pCommand)
{
    uintptr_t CBaseModPanel__m_CFactoryBasePanel = *(uintptr_t *)(clientnrobase + 0x1614958); // CBaseModPanel* type
    if (!CBaseModPanel__m_CFactoryBasePanel)
    {
        Warning("No CBaseModPanel__m_CFactoryBasePanel in CBaseModFooterPanel::OnCommand hook!\n");
        return;
    }

    // CBaseModFrame *pFrame = BASEMODPANEL_SINGLETON.GetWindow( BASEMODPANEL_SINGLETON.GetActiveWindowType() );
    // m_bUsesAlternateTiles = ( pFrame && pFrame->UsesAlternateTiles() );
    // See CBaseModPanel::GetBackgroundMovieName for useful offsets
    auto GetActiveWindowType = (int (*)(void *))(clientnrobase + 0x4990F8);
    int activeWindowType = GetActiveWindowType((void *)CBaseModPanel__m_CFactoryBasePanel);
    void *g_pVGui = *(void **)(clientnrobase + 0x1641318);
    void *g_pVGuiPanel = *(void **)(clientnrobase + 0x1641328);
    void *pWindow = nullptr;
    void *m_iPanelID = *(void **)(CBaseModPanel__m_CFactoryBasePanel + 8 * activeWindowType + 688); // m_Frames[activeWindowType].m_iPanelID
	if ((uintptr_t)m_iPanelID != 0xffffffff)
	{
		void *panel = (*(void *(**)(void *, void *))(*(uintptr_t *)g_pVGui + 152))(g_pVGui, m_iPanelID); // ivgui()->HandleToPanel(m_iPanelID);
		if (panel)
		{
			pWindow = (*(void *(**)(void *, void *vguiPanel, const char *moduleName))(*(uintptr_t *)g_pVGuiPanel + 448))(g_pVGuiPanel, panel, (const char *)(clientnrobase + 0x171C105)); // ipanel()->GetPanel(panel, GetControlsModuleName());
		}
	}

    if (pWindow)
    {
        if (!strncmp(pCommand, "Btn", 3))
        {
            int m_nFooterType = *reinterpret_cast<int *>((uintptr_t)thisptr + 3492);
            void *pFooterData = (void *)(((uintptr_t)thisptr) + 168 * m_nFooterType); // m_FooterData[m_nFooterType]

            // GetButtonOrder
            int buttonOrder[6] = { 0 };
            int m_Format = *(int *)((uintptr_t)pFooterData + 3644);
            switch ( m_Format )
            {
            case 2:
                buttonOrder[0] = 1;
                buttonOrder[1] = 2;
                buttonOrder[2] = 8;
                buttonOrder[3] = 4;
                buttonOrder[4] = 16;
                buttonOrder[5] = 32;
                break;

            default:
                buttonOrder[0] = 1;
                buttonOrder[1] = 2;
                buttonOrder[2] = 4;
                buttonOrder[3] = 8;
                buttonOrder[4] = 16;
                buttonOrder[5] = 32;
                break;
            }

            int nWhich = atoi( pCommand + 3 );
			int nButton = buttonOrder[nWhich];

			int keyCode = 0; // KEY_NONE
			switch ( nButton )
			{
			case 0x01: // FB_ABUTTON
				keyCode = 114; // KEY_XBUTTON_A
				break;
			case 0x02: // FB_BBUTTON
				keyCode = 115; // KE_XBUTTON_B
				break;
			case 0x04: // FB_XBUTTON
				keyCode = 116; // KEY_XBUTTON_X
				break;
			case 0x08: // FB_YBUTTON
				keyCode = 117; // KEY_XBUTTON_Y
				break;
			case 0x20: // FB_LSHOULDER
				keyCode = 118; // KEY_XBUTTON_LEFT_SHOULDER
				break;
			default:
				return;
			}

            // TODO: Reimplement keyCode arg here correctly
            // pWindow->OnKeyCodePressed( ButtonCodeToJoystickButtonCode( keyCode, CBaseModPanel::GetSingleton().GetLastActiveUserId() ) );
            (*(void (**)(void *, int code))(*(uintptr_t *)pWindow + (8 * 122)))(pWindow, keyCode);
        }
    }
}

//-----------------------------------------------------------------------------------------

#define VTABLE_FUNC_ADDRESS(nrobase, offset, index) &((void**)(nrobase + offset))[index]

void ToggleVTableDetours( bool bPatching )
{
	// Enables actual in-game mouse input tracking
    // In Portal 2, you need to remove the -nomouse launch parm and set cl_mouseenable to 1
    ToggleDetour(VTABLE_FUNC_ADDRESS(enginenrobase, Offsets::CEngineClient__vtable, Offsets::CEngineClient__GetMouseDelta_vtable_index), CEngineClient__GetMouseDelta_Original, CEngineClient__GetMouseDelta_Hook, bPatching);

    // We need this detour to actually fire double mouse click events since they normally don't work for some reason
    ToggleDetour(VTABLE_FUNC_ADDRESS(vgui2nrobase, Offsets::CInputSystem__vtable, Offsets::CInputSystem__InternalMousePressed_vtable_index), CInputSystem__InternalMousePressed_Original, CInputSystem__InternalMousePressed_Hook, bPatching);

    // Hook to fix cursor crash on menu option hover
    if (!g_Plugin.IsGamePortal2())
    {
        // Portal has multiple vtables in reference to these functions
        ToggleDetour(VTABLE_FUNC_ADDRESS(GameUInrobase, Offsets::MenuItem__vtable, Offsets::MenuItem__OnCursorEntered_vtable_index), MenuItem__OnCursorEntered_Original, MenuItem__OnCursorEntered_Hook, bPatching);
        ToggleDetour(VTABLE_FUNC_ADDRESS(GameUInrobase, Offsets::MenuItem__vtable, Offsets::MenuItem__OnCursorExited_vtable_index), MenuItem__OnCursorExited_Original, MenuItem__OnCursorExited_Hook, bPatching);

        ToggleDetour(VTABLE_FUNC_ADDRESS(GameUInrobase, Offsets::MenuItem__vtable2, Offsets::MenuItem__OnCursorEntered_vtable_index), MenuItem__OnCursorEntered_Original, MenuItem__OnCursorEntered_Hook, bPatching);
        ToggleDetour(VTABLE_FUNC_ADDRESS(GameUInrobase, Offsets::MenuItem__vtable2, Offsets::MenuItem__OnCursorExited_vtable_index), MenuItem__OnCursorExited_Original, MenuItem__OnCursorExited_Hook, bPatching);

        ToggleDetour(VTABLE_FUNC_ADDRESS(GameUInrobase, Offsets::CGameMenuItem__vtable, Offsets::MenuItem__OnCursorEntered_vtable_index), MenuItem__OnCursorEntered_Original, MenuItem__OnCursorEntered_Hook, bPatching);
        ToggleDetour(VTABLE_FUNC_ADDRESS(GameUInrobase, Offsets::CGameMenuItem__vtable, Offsets::MenuItem__OnCursorExited_vtable_index), MenuItem__OnCursorExited_Original, MenuItem__OnCursorExited_Hook, bPatching);
    }
    else
    {
        ToggleDetour(VTABLE_FUNC_ADDRESS(clientnrobase, Offsets::MenuItem__vtable, Offsets::MenuItem__OnCursorEntered_vtable_index), MenuItem__OnCursorEntered_Original, MenuItem__OnCursorEntered_Hook, bPatching);
        ToggleDetour(VTABLE_FUNC_ADDRESS(clientnrobase, Offsets::MenuItem__vtable, Offsets::MenuItem__OnCursorExited_vtable_index), MenuItem__OnCursorExited_Original, MenuItem__OnCursorExited_Hook, bPatching);
    }

    // Hook to fix console printing in Portal (No color support yet)
    if (!g_Plugin.IsGamePortal2())
    {
        // Patch the GOT entry for the PLT stub
        uintptr_t got_entry_addr = tier0nrobase + Offsets::vsnprintf__got_entry;
        vsnprintf_Original = *(int (**)(char *s, size_t maxlen, const char *format, __gnuc_va_list arg))got_entry_addr;
        *(void **)got_entry_addr = (bPatching) ? (void *)vsnprintf_Hook : (void *)vsnprintf_Original;
    }

    if (g_Plugin.IsGamePortal2())
    {
        // Hook to allow using ladders for all players
        // Point the target func to the base class's
        ToggleDetour(&(*reinterpret_cast<void ***>(g_GameMovement))[Offsets::CPortalGameMovement__GameHasLadders_vtable_index], CPortalGameMovement__GameHasLadders_Original, CPortalGameMovement__GameHasLadders_Hook, bPatching);

        // Allow us to control the max player limit when starting a server.
        ToggleDetour(VTABLE_FUNC_ADDRESS(enginenrobase, Offsets::CGameServer__vtable, Offsets::CGameServer__SetMaxClients_vtable_index), CGameServer__SetMaxClients_Original, CGameServer__SetMaxClients_Hook, bPatching);

        // Restore the menu footer button functionality.
        ToggleDetour(VTABLE_FUNC_ADDRESS(clientnrobase, 0x1353048, 97), CBaseModFooterPanel__OnCommand_Original, CBaseModFooterPanel__OnCommand_Hook, bPatching);
    }

    //-----------------------------------------------------------------------------------------

    // TODO: Look into CInputStackSystem::SetCursorPosition for the below...
    // Allows us to not reach the borders of the screen when moving the mouse while in-game (Won't work when using Ryujinx since no locked mouse capture supported)

    // TODO: Finish this to manually reset the mouse
    /*nn::hid::MouseState mouseState;
    nn::hid::detail::GetMouseState(&mouseState);
    Msg("BEFORE: x: %d, y: %d\n", mouseState.x, mouseState.y)   ;

    uintptr_t *sharedMemoryPtr = (uintptr_t *)((uintptr_t)&nn::hid::detail::GetMouseState + 0xAFA0D8); // Portal 2
    uintptr_t sharedMemoryBase = *sharedMemoryPtr;

    int currentIndex = *(int *)(sharedMemoryBase + 16);
    int offset = ((currentIndex - 0 + 17) % 17); // Most recent entry
    uintptr_t mouseStateAddr = sharedMemoryBase + 48 * offset;

    *(int32_t *)(mouseStateAddr + 48) = x; // Set X coordinate
    *(int32_t *)(mouseStateAddr + 52) = y; // Set Y coordinate

    nn::hid::detail::GetMouseState(&mouseState);
    Msg("AFTER: x: %d, y: %d\n", mouseState.x, mouseState.y);*/
}
