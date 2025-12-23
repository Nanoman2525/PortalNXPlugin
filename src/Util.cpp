#include "Command.hpp"
#include "NXModule.hpp"
#include "Offsets.hpp"
#include "Plugin.hpp"

extern "C" void Msg( const char* pMsg, ... );

CON_COMMAND(nx_toggle_autojump, "Toggles the autojump ability for all players.")
{
    static uint8_t orig_bytes[4] = { 0 };
    static bool bPatched = false;

    if (!bPatched)
    {
        bPatched = true;

        if (!orig_bytes[0])
        {
            memcpy(orig_bytes, (void *)(servernrobase + Offsets::CPortalGameMovement__CheckJumpButton_autojump), sizeof(orig_bytes));
        }

        uint8_t patch[4] = { 0x1F, 0x20, 0x03, 0xD5 }; // Nop
        memcpy((void *)(servernrobase + Offsets::CPortalGameMovement__CheckJumpButton_autojump), patch, sizeof(patch));

        Msg("nx_toggle_autojump - Patched.\n");
    }
    else
    {
        bPatched = false;

        memcpy((void *)(servernrobase + Offsets::CPortalGameMovement__CheckJumpButton_autojump), orig_bytes, sizeof(orig_bytes));

        Msg("nx_toggle_autojump - Unpatched.\n");
    }
}

CON_COMMAND(nx_toggle_reportals, "Toggles the reportal ability for all players.")
{
    if (!g_Plugin.IsGamePortal2())
    {
        Msg("nx_toggle_reportals - Works only on Portal 2 game binaries.\n");
        return;
    }

    static uint8_t orig_bytes[4] = { 0 };
    static bool bPatched = false;

    if (!bPatched)
    {
        bPatched = true;

        if (!orig_bytes[0])
        {
            memcpy(orig_bytes, (void *)(servernrobase + Offsets::CPortal_Base2D__NewLocation_reportal), sizeof(orig_bytes));
        }

        uint8_t patch[4] = { 0x1F, 0x20, 0x03, 0xD5 }; // Nop
        memcpy((void *)(servernrobase + Offsets::CPortal_Base2D__NewLocation_reportal), patch, sizeof(patch));

        Msg("nx_toggle_reportals - Patched.\n");
    }
    else
    {
        bPatched = false;

        memcpy((void *)(servernrobase + Offsets::CPortal_Base2D__NewLocation_reportal), orig_bytes, sizeof(orig_bytes));

        Msg("nx_toggle_reportals - Unpatched.\n");
    }
}

CON_COMMAND(nx_toggle_menu_controller_button_visibility, "Toggles the visibility of the menu game console helpers on screen.")
{
    if (!g_Plugin.IsGamePortal2())
    {
        Msg("nx_toggle_menu_controller_button_visibility - Works only on Portal 2 game binaries.\n");
        return;
    }

    static uint8_t orig_bytes[4] = { 0 };
    static bool bPatched = false;

    if (!bPatched)
    {
        bPatched = true;

        if (!orig_bytes[0])
        {
            memcpy(orig_bytes, (void *)(clientnrobase + Offsets::CBaseModFooterPanel__DrawButtonAndText_stub), sizeof(orig_bytes));
        }

        uint8_t patch[4] = { 0xE3, 0x00, 0x00, 0x14 }; // Jump to end of function (stub it out)
        memcpy((void *)(clientnrobase + Offsets::CBaseModFooterPanel__DrawButtonAndText_stub), patch, sizeof(patch));

        Msg("nx_toggle_menu_controller_button_visibility - Patched.\n");
    }
    else
    {
        bPatched = false;

        memcpy((void *)(clientnrobase + Offsets::CBaseModFooterPanel__DrawButtonAndText_stub), orig_bytes, sizeof(orig_bytes));

        Msg("nx_toggle_menu_controller_button_visibility - Unpatched.\n");
    }
}

CON_COMMAND(nx_toggle_coop_loading_dots, "Shows the coop loading progress on map transitions.")
{
    if (!g_Plugin.IsGamePortal2())
    {
        Msg("nx_toggle_coop_loading_dots - Works only on Portal 2 game binaries.\n");
        return;
    }

    static uint8_t orig_bytes[4] = { 0 };
    static bool bPatched = false;

    if (!bPatched)
    {
        bPatched = true;

        if (!orig_bytes[0])
        {
            memcpy(orig_bytes, (void *)(clientnrobase + Offsets::LoadingProgress__SetupControlStates_dot_patch), sizeof(orig_bytes));
        }

        uint8_t patch[4] = { 0x7F, 0xE2, 0x36, 0x39 };
        memcpy((void *)(clientnrobase + Offsets::LoadingProgress__SetupControlStates_dot_patch), patch, sizeof(patch));

        Msg("nx_toggle_coop_loading_dots - Patched.\n");
    }
    else
    {
        bPatched = false;

        memcpy((void *)(clientnrobase + Offsets::LoadingProgress__SetupControlStates_dot_patch), orig_bytes, sizeof(orig_bytes));

        Msg("nx_toggle_coop_loading_dots - Unpatched.\n");
    }
}

extern void *engineClient;
void UpdateFooterButtons() // TODO: This crashes if there is no button present on the menu, and this is called
{
    uintptr_t CBaseModPanel__m_CFactoryBasePanel = *(uintptr_t *)(clientnrobase + 0x1614958); // CBaseModPanel* type
    if (!CBaseModPanel__m_CFactoryBasePanel)
    {
        Msg("No CBaseModPanel::m_CFactoryBasePanel!\n");
        return;
    }

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

    if (!pFooter)
    {
        Msg("No pFooter!\n");
        return;
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

    char uilanguage[64];
    (*(void (**)(void *, char *, int))(*(uintptr_t *)engineClient + 736))(engineClient, uilanguage, sizeof(uilanguage));
    bool bIsEnglish = ( uilanguage[0] == 0 ) || !strcmp( uilanguage, "english" );

    int m_nFooterType = *reinterpret_cast<int *>((uintptr_t)pFooter + 3492);
    void *pFooterData = (void *)(((uintptr_t)pFooter) + 168 * m_nFooterType); // m_FooterData[m_nFooterType]

    int x = *(int *)((uintptr_t)pFooterData + 3652); // pFooterData->m_nX;

    // CBaseModFrame *pFrame = BASEMODPANEL_SINGLETON.GetWindow( BASEMODPANEL_SINGLETON.GetActiveWindowType() );
    // m_bUsesAlternateTiles = ( pFrame && pFrame->UsesAlternateTiles() );
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

        // Note: Deviating from the normal code by adding this check to not display if the first button is just "Select", so that it matches up with PC
        int offset = 3496 + (i * 24); // pFooterData->m_AButtonText.Get() actual value to start with
        const char *buttonText = *(const char **)((uintptr_t)pFooterData + offset); // Offset each text by adding 24 to get to the next one in this order: A, B, X, Y, DPad, LShoulder
        if ((nButton == 1) && !strcmp("#L4D360UI_Select", buttonText))
        {
            // pButton->SetVisible( false );
            (*(void (**)(void *, bool))(*(uintptr_t *)pButton + 272))(pButton, false); // pFooterData::m_Buttons
            continue;
        }

        // pButton->SetVisible( ( nButton & pFooterData->m_Buttons ) != 0 );
        (*(void (**)(void *, bool))(*(uintptr_t *)pButton + 272))(pButton, (nButton & (*(unsigned int *)((uintptr_t)pFooterData + 3640))) != 0); // pFooterData::m_Buttons

        // pButton->SetUsesAlternateTiles( m_bUsesAlternateTiles );
        *(bool *)((uintptr_t)pButton + 772) = m_bUsesAlternateTiles;

        if ( !(nButton & (*(unsigned int *)((uintptr_t)pFooterData + 3640))) ) // pFooterData::m_Buttons
            continue;

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
            *((uintptr_t *)pButton + 638) = true; // m_bAllCaps = true;
            uintptr_t textImage = *((uintptr_t *)pButton + 67); // thisptr[67] from disassembly
            *(unsigned char *)(textImage + 80) = (*(unsigned char *)(textImage + 80) & 0xDF) | 32; // _textImage->m_bAllCaps = true;
            (*(void (**)(void *, bool, bool))(*(uintptr_t *)pButton + 536))(pButton, false, false); // InvalidateLayout( false, false );
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

CON_COMMAND(nx_update_footer_buttons, "Run init logic missing from the game if needed and update the menu footer.")
{
    if (!g_Plugin.IsGamePortal2())
    {
        Msg("nx_update_footer_buttons - Works only on Portal 2 game binaries.\n");
        return;
    }

    UpdateFooterButtons();
}
