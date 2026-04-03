#include "Command.hpp"
#include "NXModule.hpp"
#include "Offsets.hpp"
#include "Plugin.hpp"

// Probably should refine this at some point, but it will work for now.
struct TogglePatch
{
    uintptr_t *base;            // Module offsets are not loaded at runtime, so defer to ptr.
    int *offset;                // Game offsets are not loaded at runtime, so defer to ptr.
    uint8_t patch_bytes[16];
    size_t patch_size;
    uint8_t orig_bytes[16];     // Adjust max size here if we need to.
    bool bPatched = false;
    bool bOrigSaved = false;

    template<size_t N>
    TogglePatch(uintptr_t *base, int *offset, const uint8_t (&bytes)[N])
        : base(base), offset(offset), patch_size(N)
    {
        memcpy(patch_bytes, bytes, N);
    }

    void Apply()
    {
        if (!bOrigSaved)
        {
            memcpy(orig_bytes, (void *)(*base + *offset), patch_size);
            bOrigSaved = true;
        }

        memcpy((void *)(*base + *offset), patch_bytes, patch_size);
        bPatched = true;
    }

    void Restore()
    {
        if (bOrigSaved)
        {
            memcpy((void *)(*base + *offset), orig_bytes, patch_size);
        }
        bPatched = false;
    }

    void Toggle()
    {
        if (!bPatched)
        {
            Apply();
        }
        else
        {
            Restore();
        }
    }
};

// Autojump
static TogglePatch nx_autojump = { &servernrobase, &Offsets::CPortalGameMovement__CheckJumpButton_autojump, { 0x1F, 0x20, 0x03, 0xD5 } };
CON_COMMAND(nx_toggle_autojump, "Toggles the autojump ability for all players.")
{
    nx_autojump.Toggle();
    Msg("nx_toggle_autojump - %s.\n", nx_autojump.bPatched ? "Patched" : "Unpatched");
}

// Floor reportals
static TogglePatch nx_floor_reportals = { &servernrobase, &Offsets::CPortal_Base2D__NewLocation_reportal, { 0x1F, 0x20, 0x03, 0xD5 } };
CON_COMMAND(nx_toggle_floor_reportals, "Toggles the floor reportal ability for all players.")
{
    if (!g_Plugin.IsGamePortal2())
    {
        Msg("nx_toggle_floor_reportals - Works only on Portal 2 game binaries.\n");
        return;
    }

    nx_floor_reportals.Toggle();
    Msg("nx_toggle_floor_reportals - %s.\n", nx_floor_reportals.bPatched ? "Patched" : "Unpatched");
}

// Coop loading dots
static TogglePatch nx_coop_loading_dots = { &clientnrobase, &Offsets::LoadingProgress__SetupControlStates_dot_patch, { 0x7F, 0xE2, 0x36, 0x39 } };
CON_COMMAND(nx_toggle_coop_loading_dots, "Shows the coop loading progress on map transitions.")
{
    if (!g_Plugin.IsGamePortal2())
    {
        Msg("nx_toggle_coop_loading_dots - Works only on Portal 2 game binaries.\n");
        return;
    }

    nx_coop_loading_dots.Toggle();
    Msg("nx_toggle_coop_loading_dots - %s.\n", nx_coop_loading_dots.bPatched ? "Patched" : "Unpatched");
}

// Loading orange dots
static TogglePatch nx_loading_orange_dots = { &clientnrobase, &Offsets::LoadingProgress__DrawLoadingBar_dot_patch, { 0x68, 0x66, 0x86, 0x52, 0x68, 0xEE, 0xA7, 0x72 } };
CON_COMMAND(nx_toggle_loading_orange_dots, "Fixes the orange dots not showing when fully loaded into maps.")
{
    if (!g_Plugin.IsGamePortal2())
    {
        Msg("nx_toggle_loading_orange_dots - Works only on Portal 2 game binaries.\n");
        return;
    }

    nx_loading_orange_dots.Toggle();
    Msg("nx_toggle_loading_orange_dots - %s.\n", nx_loading_orange_dots.bPatched ? "Patched" : "Unpatched");
}

// Force max FPS
static TogglePatch nx_force_max_fps = { &enginenrobase, &Offsets::ApplySplitscreenPerformanceConfig__fps_patch, { 0x00, 0x00, 0x80, 0x52, 0x1F, 0x20, 0x03, 0xD5 } };
CON_COMMAND(nx_toggle_force_max_fps, "Forces r_dynres_enable and nvn_swap_interval to 1. (Won't be 30 FPS in splitscreen)")
{
    if (!g_Plugin.IsGamePortal2())
    {
        Msg("nx_toggle_force_max_fps - Works only on Portal 2 game binaries.\n");
        return;
    }

    // This checks against "engine->IsSplitScreenActive()" to see if we should run at 30 fps, otherwise, run at 60.
    // It may be a good idea to hook this up to ConVar fps_max again, but unfortunately as of current, we are limited to 60 fps as the max.
    nx_force_max_fps.Toggle();
    Msg("nx_toggle_force_max_fps - %s.\n", nx_force_max_fps.bPatched ? "Patched" : "Unpatched");
}

// Make challenge mode menu navigation work without internet/NSO
static TogglePatch nx_CSinglePlayer__OnCommand_check1 = { &clientnrobase, &Offsets::CSinglePlayer__OnCommand_cm_check1, { 0x1F, 0x20, 0x03, 0xD5 } };
static TogglePatch nx_CSinglePlayer__OnCommand_check2 = { &clientnrobase, &Offsets::CSinglePlayer__OnCommand_cm_check2, { 0x1F, 0x20, 0x03, 0xD5 } };
static TogglePatch nx_CStartCoopGame__OnCommand_check1 = { &clientnrobase, &Offsets::CStartCoopGame__OnCommand_cm_check1, { 0x1F, 0x20, 0x03, 0xD5 } };
static TogglePatch nx_CStartCoopGame__OnCommand_check2 = { &clientnrobase, &Offsets::CStartCoopGame__OnCommand_cm_check2, { 0x1F, 0x20, 0x03, 0xD5 } };
static TogglePatch nx_CPortalLeaderboardPanel__OnThink_check1 = { &clientnrobase, &Offsets::CPortalLeaderboardPanel__OnThink_cm_check1, { 0x1F, 0x20, 0x03, 0xD5 } };
static TogglePatch nx_CPortalLeaderboardPanel__OnThink_check2 = { &clientnrobase, &Offsets::CPortalLeaderboardPanel__OnThink_cm_check2, { 0x1F, 0x20, 0x03, 0xD5 } };
CON_COMMAND(nx_toggle_ui_challenge_mode_menu_access, "Restores the ability to enter these menus without internet/NSO.")
{
    if (!g_Plugin.IsGamePortal2())
    {
        Msg("nx_toggle_ui_challenge_mode_menu_access - Works only on Portal 2 game binaries.\n");
        return;
    }

    // For the "PLAY SINGLE PLAYER -> CHALLENGE MODE" button
    nx_CSinglePlayer__OnCommand_check1.Toggle();
    nx_CSinglePlayer__OnCommand_check2.Toggle();

    // For the "PLAY COOPERATIVE GAME -> PLAY ONLINE" button
    nx_CStartCoopGame__OnCommand_check1.Toggle();
    nx_CStartCoopGame__OnCommand_check2.Toggle();

    // While we are in any of the challenge mode menus at all, it will constantly run a check (For if you lose internet/NSO connection)
    nx_CPortalLeaderboardPanel__OnThink_check1.Toggle();
    nx_CPortalLeaderboardPanel__OnThink_check2.Toggle();

    Msg("nx_toggle_ui_challenge_mode_menu_access - %s.\n", nx_CSinglePlayer__OnCommand_check1.bPatched ? "Patched" : "Unpatched");
}

void CBaseModFooterPanel__FixLayout_Restored(bool bHideAllButtons)
{
    uintptr_t CBaseModPanel__m_CFactoryBasePanel = *(uintptr_t *)(clientnrobase + 0x1614958); // CBaseModPanel* type

    // CBaseModFooterPanel *pFooter = BaseModUI::CBaseModPanel::GetSingleton().GetFooterPanel();
    void *g_pVGui = *(void **)(clientnrobase + 0x1641318);
    void *g_pVGuiPanel = *(void **)(clientnrobase + 0x1641328);
    void *pFooter = nullptr;
    void *m_FooterPanel = *(void **)(CBaseModPanel__m_CFactoryBasePanel + 1216);
	if ((uintptr_t)m_FooterPanel != 0xffffffff) // m_FooterPanel.Get()
	{
		void *panel = (*(void *(**)(void *, void *))(*(uintptr_t *)g_pVGui + 152))(g_pVGui, m_FooterPanel); // ivgui()->HandleToPanel(m_FooterPanel);
		if (panel)
		{
			pFooter = (*(void *(**)(void *, void *vguiPanel, const char *moduleName))(*(uintptr_t *)g_pVGuiPanel + 448))(g_pVGuiPanel, panel, (const char *)(clientnrobase + 0x171C105)); // ipanel()->GetPanel(panel, GetControlsModuleName());
		}
	}

    // Member variables of m_FooterPanel
    void **m_pButtons = reinterpret_cast<void **>((uintptr_t)pFooter + 4008);
    void *m_hButtonTextFont = *reinterpret_cast<void **>((uintptr_t)pFooter + 4080);
    int m_nTextOffsetX = *reinterpret_cast<int *>((uintptr_t)pFooter + 4104);
    int m_nTextOffsetY = *reinterpret_cast<int *>((uintptr_t)pFooter + 4108);

    // Re-add the missing logic from CBaseModFooterPanel::ApplySchemeSettings
    static bool bAlreadyInitiated = false;
    if (!bAlreadyInitiated)
    {
        for ( int i = 0; i < 6; i++ )
        {
            // m_pButtons[i]->SetFont( m_hButtonTextFont );
            (*(void (**)(void *, void *))(*(uintptr_t *)m_pButtons[i] + 1800))(m_pButtons[i], m_hButtonTextFont);

            // m_pButtons[i]->SetArmedSound( "UI/menu_focus.wav" );
            unsigned short shortVar;
            auto SetArmedSound = (void (*)(unsigned short *a1, void *, const char *))(clientnrobase + 0x630F54);
            SetArmedSound(&shortVar, (void *)(clientnrobase + 0x171B8D0), "UI/menu_focus.wav");
            *(unsigned short *)((uintptr_t)m_pButtons[i] + 738) = shortVar;

            // m_pButtons[i]->SetCommand( buffer );
            char buffer[8];
            sprintf(buffer, "Btn%d", i);
            (*(void (**)(void *, const char *))(*(uintptr_t *)m_pButtons[i] + 2248))(m_pButtons[i], buffer);

            if ( m_nTextOffsetX || m_nTextOffsetY )
            {
                // m_pButtons[i]->SetTextInset( m_nTextOffsetX, m_nTextOffsetY );
                (*(void (**)(void *, int, int))(*(uintptr_t *)m_pButtons[i] + 1744))(m_pButtons[i], m_nTextOffsetX, m_nTextOffsetY);
            }
        }

        bAlreadyInitiated = true;
    }

    // Re-add logic missing from CBaseModFooterPanel::FixLayout
    if ( !*(bool *)((uintptr_t)pFooter + 4144) ) // if ( !m_bInitialized )
        return;

    // Note: Deviating from the normal code by adding this check to see if we want to hide the buttons for cleanup
    if (bHideAllButtons)
    {
        for ( int i = 0; i < 6; i++ )
        {
            // m_pButtons[i]->SetVisible( false );
            (*(void (**)(void *, bool))(*(uintptr_t *)m_pButtons[i] + 272))(m_pButtons[i], false);
        }
        return;
    }

    char uilanguage[64];
    (*(void (**)(void *, char *, int))(*(uintptr_t *)engineClient + 736))(engineClient, uilanguage, sizeof(uilanguage)); // CEngineClient::GetUILanguage
    bool bIsEnglish = ( uilanguage[0] == 0 ) || !strcmp( uilanguage, "english" );

    int m_nFooterType = *reinterpret_cast<int *>((uintptr_t)pFooter + 3492);
    void *pFooterData = (void *)(((uintptr_t)pFooter) + 168 * m_nFooterType); // m_FooterData[m_nFooterType]

    int x = *(int *)((uintptr_t)pFooterData + 3652); // pFooterData->m_nX;

    // CBaseModFrame *pFrame = BASEMODPANEL_SINGLETON.GetWindow( BASEMODPANEL_SINGLETON.GetActiveWindowType() );
    // See CBaseModPanel::GetBackgroundMovieName for useful offsets
    auto GetActiveWindowType = (int (*)(void *))(clientnrobase + 0x4990F8);
    int activeWindowType = GetActiveWindowType((void *)CBaseModPanel__m_CFactoryBasePanel);
    void *vguiPanel = nullptr;
    void *m_iPanelID = *(void **)(CBaseModPanel__m_CFactoryBasePanel + 8 * activeWindowType + 688); // m_Frames[activeWindowType].m_iPanelID
	if ((uintptr_t)m_iPanelID != 0xffffffff)
	{
		void *panel = (*(void *(**)(void *, void *))(*(uintptr_t *)g_pVGui + 152))(g_pVGui, m_iPanelID); // ivgui()->HandleToPanel(m_iPanelID);
		if (panel)
		{
			vguiPanel = (*(void *(**)(void *, void *vguiPanel, const char *moduleName))(*(uintptr_t *)g_pVGuiPanel + 448))(g_pVGuiPanel, panel, (const char *)(clientnrobase + 0x171C105)); // ipanel()->GetPanel(panel, GetControlsModuleName());
		}
	}

    // m_bUsesAlternateTiles = ( pFrame && pFrame->UsesAlternateTiles() );
    bool m_bUsesAlternateTiles = *(bool *)((uintptr_t)pFooter + 4000);
    m_bUsesAlternateTiles = ( vguiPanel && (*(bool *)((uintptr_t)vguiPanel + 3152)) );

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

    for ( int i = 0; i < 6; i++ )
    {
        void *pButton = m_pButtons[i];
        if ( !pButton )
            continue;

        int nButton = buttonOrder[i];

        // pButton->SetVisible( ( nButton & pFooterData->m_Buttons ) != 0 );
        (*(void (**)(void *, bool))(*(uintptr_t *)pButton + 272))(pButton, (nButton & (*(unsigned int *)((uintptr_t)pFooterData + 3640))) != 0); // pFooterData::m_Buttons

        // pButton->SetUsesAlternateTiles( m_bUsesAlternateTiles );
        *(bool *)((uintptr_t)pButton + 772) = m_bUsesAlternateTiles;

        if ( !(nButton & (*(unsigned int *)((uintptr_t)pFooterData + 3640))) ) // pFooterData::m_Buttons
            continue;

        // Note: Simplified version of the text logic
        int offset = 3496 + (i * 24); // pFooterData->m_AButtonText.Get() actual value to start with
        const char *buttonText = *(const char **)((uintptr_t)pFooterData + offset); // Offset each text by adding 24 to get to the next one in this order: A, B, X, Y, DPad, LShoulder
        if (m_Format == 2)
        {
            if (nButton == 8)
            {
                buttonText = *(const char **)((uintptr_t)pFooterData + offset + 24);
            }
            else if (nButton == 4)
            {
                buttonText = *(const char **)((uintptr_t)pFooterData + offset - 24);
            }
        }

        // pButton->SetText
        (*(void (**)(void *, const char *))(*(uintptr_t*)pButton + 1696))(pButton, buttonText);

        // pButton->GetContentSize( nButtonWide, nButtonTall );
        int nButtonWide, nButtonTall;
        (*(void (**)(void *, int &, int &))(*(uintptr_t *)pButton + 1728))(pButton, nButtonWide, nButtonTall);

        // pButton->SetBounds( x, pFooterData->m_nY, nButtonWide + m_nButtonPaddingX, nButtonTall );
        int y = *(int *)((uintptr_t)pFooterData + 3656); // pFooterData->m_nY;
        int m_nButtonPaddingX = *reinterpret_cast<int *>((uintptr_t)pFooter + 4124);
        (*(void (**)(void *, void *, int, int))(*(uintptr_t *)g_pVGuiPanel + 24))(g_pVGuiPanel, (**(void *(***)(void *))pButton)(pButton), x, y); // SetPos(x, pFooterData->m_nY);
        (*(void (**)(void *, void *, int, int))(*(uintptr_t *)g_pVGuiPanel + 40))(g_pVGuiPanel, (**(void *(***)(void *))pButton)(pButton), nButtonWide + m_nButtonPaddingX, nButtonTall); // SetSize(nButtonWide + m_nButtonPaddingX, nButtonTall);

        // pButton->SetContentAlignment( vgui::Label::a_center );
        (*(void (**)(void *, int))(*(uintptr_t *)pButton + 1736))(pButton, 4);

        if (bIsEnglish)
        {
            // pButton->SetAllCaps( true );
            // On Switch, it looks much better without caps...
            // *((uintptr_t *)pButton + 638) = true; // m_bAllCaps = true;
            // uintptr_t textImage = *((uintptr_t *)pButton + 67); // thisptr[67] from disassembly
            // *(unsigned char *)(textImage + 80) = (*(unsigned char *)(textImage + 80) & 0xDF) | 32; // _textImage->m_bAllCaps = true;
            // (*(void (**)(void *, bool, bool))(*(uintptr_t *)pButton + 536))(pButton, false, false); // InvalidateLayout( false, false );
        }

        uint32_t m_TextColor = *reinterpret_cast<uint32_t *>((uintptr_t)pFooter + 4128);
        uint32_t m_TextColorAlt = *reinterpret_cast<uint32_t *>((uintptr_t)pFooter + 4132);
        uint32_t textColor = ( m_bUsesAlternateTiles ) ? m_TextColorAlt : m_TextColor;
        (*(void (**)(void *, uint32_t, uint32_t))(*(uintptr_t *)pButton + 2176))(pButton, textColor, 0); // pButton->SetDefaultColor( textColor, Color(0,0,0,0) );
        (*(void (**)(void *, uint32_t, uint32_t))(*(uintptr_t *)pButton + 2192))(pButton, textColor, 0); // pButton->SetDepressedColor( textColor, Color(0,0,0,0) );
        (*(void (**)(void *, uint32_t, uint32_t))(*(uintptr_t *)pButton + 2184))(pButton, textColor, 0); // pButton->SetArmedColor( textColor, Color(0,0,0,0) );

        int m_nButtonGapX = *reinterpret_cast<int *>((uintptr_t)pFooter + 4112);

        x += nButtonWide + m_nButtonPaddingX + m_nButtonGapX;
    }
}

// Footer buttons aren't compiled out, but they have been severely stripped.
CON_COMMAND(nx_update_footer_buttons, "Run init logic missing from the game if needed and update the menu footer.")
{
    if (!g_Plugin.IsGamePortal2())
    {
        Msg("nx_update_footer_buttons - Works only on Portal 2 game binaries.\n");
        return;
    }

    CBaseModFooterPanel__FixLayout_Restored(false);
}

// Add cvar unhiding functionality like in Saul's plugin
// https://github.com/saul/cvar-unhide/blob/master/serverplugin.cpp#L192-L209
CON_COMMAND(nx_cvar_unhide_all, "Unhide all FCVAR_HIDDEN and FCVAR_DEVELOPMENTONLY convars")
{
    FOR_ALL_CONSOLE_COMMANDS( pCommand )
    {
        if ( pCommand->IsFlagSet( FCVAR_DEVELOPMENTONLY ) || pCommand->IsFlagSet( FCVAR_HIDDEN ) )
        {
            pCommand->RemoveFlags( FCVAR_DEVELOPMENTONLY | FCVAR_HIDDEN );
        }
    }
}
