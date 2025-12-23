#include "NXModule.hpp"
#include "Offsets.hpp"
#include "Plugin.hpp"
#include <string.h> // memset

#include "Variable.hpp"

Variable nx_enable_keyboard_support("nx_enable_keyboard_support", "0", "On Switch, enables keyboard input reading.", FCVAR_NONE);
Variable nx_enable_mouse_support("nx_enable_mouse_support", "0", "On Switch, enables mouse input reading.", FCVAR_NONE);

struct InputEvent_t
{
	int m_nType;				// Type of the event (see InputEventType_t)
	int m_nTick;				// Tick on which the event occurred
	int m_nData;				// Generic 32-bit data, what it contains depends on the event
	int m_nData2;				// Generic 32-bit data, what it contains depends on the event
	int m_nData3;				// Generic 32-bit data, what it contains depends on the event
};

extern void *g_pInputSystem;

const int IE_ButtonPressed = 0;
const int IE_ButtonReleased = 1;
const int IE_ButtonDoubleClicked = 2;
const int IE_AnalogValueChanged = 3;

// Note: These aren't in the enum like in Portal 2. Instead they are offset from IE_FirstVguiEvent
int IE_LocateMouseClick = 1001;
int IE_KeyTyped = 1003;
int IE_KeyCodeTyped = 1004;

enum ButtonCode_t
{
	BUTTON_CODE_INVALID = -1,
	BUTTON_CODE_NONE = 0,

	KEY_FIRST = 0,

	KEY_NONE = KEY_FIRST,
	KEY_0,
	KEY_1,
	KEY_2,
	KEY_3,
	KEY_4,
	KEY_5,
	KEY_6,
	KEY_7,
	KEY_8,
	KEY_9,
	KEY_A,
	KEY_B,
	KEY_C,
	KEY_D,
	KEY_E,
	KEY_F,
	KEY_G,
	KEY_H,
	KEY_I,
	KEY_J,
	KEY_K,
	KEY_L,
	KEY_M,
	KEY_N,
	KEY_O,
	KEY_P,
	KEY_Q,
	KEY_R,
	KEY_S,
	KEY_T,
	KEY_U,
	KEY_V,
	KEY_W,
	KEY_X,
	KEY_Y,
	KEY_Z,
	KEY_PAD_0,
	KEY_PAD_1,
	KEY_PAD_2,
	KEY_PAD_3,
	KEY_PAD_4,
	KEY_PAD_5,
	KEY_PAD_6,
	KEY_PAD_7,
	KEY_PAD_8,
	KEY_PAD_9,
	KEY_PAD_DIVIDE,
	KEY_PAD_MULTIPLY,
	KEY_PAD_MINUS,
	KEY_PAD_PLUS,
	KEY_PAD_ENTER,
	KEY_PAD_DECIMAL,
	KEY_LBRACKET,
	KEY_RBRACKET,
	KEY_SEMICOLON,
	KEY_APOSTROPHE,
	KEY_BACKQUOTE,
	KEY_COMMA,
	KEY_PERIOD,
	KEY_SLASH,
	KEY_BACKSLASH,
	KEY_MINUS,
	KEY_EQUAL,
	KEY_ENTER,
	KEY_SPACE,
	KEY_BACKSPACE,
	KEY_TAB,
	KEY_CAPSLOCK,
	KEY_NUMLOCK,
	KEY_ESCAPE,
	KEY_SCROLLLOCK,
	KEY_INSERT,
	KEY_DELETE,
	KEY_HOME,
	KEY_END,
	KEY_PAGEUP,
	KEY_PAGEDOWN,
	KEY_BREAK,
	KEY_LSHIFT,
	KEY_RSHIFT,
	KEY_LALT,
	KEY_RALT,
	KEY_LCONTROL,
	KEY_RCONTROL,
	KEY_LWIN,
	KEY_RWIN,
	KEY_APP,
	KEY_UP,
	KEY_LEFT,
	KEY_DOWN,
	KEY_RIGHT,
	KEY_F1,
	KEY_F2,
	KEY_F3,
	KEY_F4,
	KEY_F5,
	KEY_F6,
	KEY_F7,
	KEY_F8,
	KEY_F9,
	KEY_F10,
	KEY_F11,
	KEY_F12,
	KEY_CAPSLOCKTOGGLE,
	KEY_NUMLOCKTOGGLE,
	KEY_SCROLLLOCKTOGGLE,

	KEY_LAST = KEY_SCROLLLOCKTOGGLE,
	KEY_COUNT = KEY_LAST - KEY_FIRST + 1,

	// Mouse
	MOUSE_FIRST = KEY_LAST + 1,

	MOUSE_LEFT = MOUSE_FIRST,
	MOUSE_RIGHT,
	MOUSE_MIDDLE,
	MOUSE_4,
	MOUSE_5,
	MOUSE_WHEEL_UP,		// A fake button which is 'pressed' and 'released' when the wheel is moved up 
	MOUSE_WHEEL_DOWN,	// A fake button which is 'pressed' and 'released' when the wheel is moved down

	MOUSE_LAST = MOUSE_WHEEL_DOWN,
	MOUSE_COUNT = MOUSE_LAST - MOUSE_FIRST + 1
};

// Convert ButtonCode_t enum value to HID bit position
uint8_t ButtonCodeToBitPosition(ButtonCode_t buttonCode)
{
    // Only handle keyboard keys
    if (buttonCode < KEY_FIRST || buttonCode > KEY_LAST)
    {
        return 0xFF; // Invalid/unsupported code
    }

    // Define a mapping between enum values and bit positions
    // https://switchbrew.org/wiki/HID_services#KeyboardKey
    static const uint8_t keyToBitPosition[KEY_COUNT] = {
        0,   // KEY_NONE
        39,  // KEY_0
        30,  // KEY_1
        31,  // KEY_2
        32,  // KEY_3
        33,  // KEY_4
        34,  // KEY_5
        35,  // KEY_6
        36,  // KEY_7
        37,  // KEY_8
        38,  // KEY_9
        4,   // KEY_A
        5,   // KEY_B
        6,   // KEY_C
        7,   // KEY_D
        8,   // KEY_E
        9,   // KEY_F
        10,  // KEY_G
        11,  // KEY_H
        12,  // KEY_I
        13,  // KEY_J
        14,  // KEY_K
        15,  // KEY_L
        16,  // KEY_M
        17,  // KEY_N
        18,  // KEY_O
        19,  // KEY_P
        20,  // KEY_Q
        21,  // KEY_R
        22,  // KEY_S
        23,  // KEY_T
        24,  // KEY_U
        25,  // KEY_V
        26,  // KEY_W
        27,  // KEY_X
        28,  // KEY_Y
        29,  // KEY_Z
        98,  // KEY_PAD_0
        89,  // KEY_PAD_1
        90,  // KEY_PAD_2
        91,  // KEY_PAD_3
        92,  // KEY_PAD_4
        93,  // KEY_PAD_5
        94,  // KEY_PAD_6
        95,  // KEY_PAD_7
        96,  // KEY_PAD_8
        97,  // KEY_PAD_9
        84,  // KEY_PAD_DIVIDE
        85,  // KEY_PAD_MULTIPLY
        86,  // KEY_PAD_MINUS
        87,  // KEY_PAD_PLUS
        88,  // KEY_PAD_ENTER
        99,  // KEY_PAD_DECIMAL
        47,  // KEY_LBRACKET
        48,  // KEY_RBRACKET
        51,  // KEY_SEMICOLON
        52,  // KEY_APOSTROPHE
        53,  // KEY_BACKQUOTE
        54,  // KEY_COMMA
        55,  // KEY_PERIOD
        56,  // KEY_SLASH
        100, // KEY_BACKSLASH
        45,  // KEY_MINUS
        46,  // KEY_EQUAL
        40,  // KEY_ENTER
        44,  // KEY_SPACE
        42,  // KEY_BACKSPACE
        43,  // KEY_TAB
        57,  // KEY_CAPSLOCK
        83,  // KEY_NUMLOCK
        41,  // KEY_ESCAPE
        71,  // KEY_SCROLLLOCK
        73,  // KEY_INSERT
        76,  // KEY_DELETE
        74,  // KEY_HOME
        77,  // KEY_END
        75,  // KEY_PAGEUP
        78,  // KEY_PAGEDOWN
        72,  // KEY_BREAK
        225, // KEY_LSHIFT
        229, // KEY_RSHIFT
        226, // KEY_LALT
        230, // KEY_RALT
        224, // KEY_LCONTROL
        228, // KEY_RCONTROL
        227, // KEY_LWIN
        231, // KEY_RWIN
        101, // KEY_APP
        82,  // KEY_UP
        80,  // KEY_LEFT
        81,  // KEY_DOWN
        79,  // KEY_RIGHT
        58,  // KEY_F1
        59,  // KEY_F2
        60,  // KEY_F3
        61,  // KEY_F4
        62,  // KEY_F5
        63,  // KEY_F6
        64,  // KEY_F7
        65,  // KEY_F8
        66,  // KEY_F9
        67,  // KEY_F10
        68,  // KEY_F11
        69,  // KEY_F12
        57,  // KEY_CAPSLOCKTOGGLE
        83,  // KEY_NUMLOCKTOGGLE
        71,  // KEY_SCROLLLOCKTOGGLE
    };

    return keyToBitPosition[buttonCode];
}

// Convert HID bit position to ButtonCode_t enum value
ButtonCode_t BitPositionToButtonCode(uint8_t bitPosition)
{
    // Define the reverse lookup map - maps bit positions to enum values
    static const ButtonCode_t bitPositionToKey[] = {
        KEY_NONE,       // 0
        BUTTON_CODE_INVALID, // 1
        BUTTON_CODE_INVALID, // 2
        BUTTON_CODE_INVALID, // 3
        KEY_A,          // 4
        KEY_B,          // 5
        KEY_C,          // 6
        KEY_D,          // 7
        KEY_E,          // 8
        KEY_F,          // 9
        KEY_G,          // 10
        KEY_H,          // 11
        KEY_I,          // 12
        KEY_J,          // 13
        KEY_K,          // 14
        KEY_L,          // 15
        KEY_M,          // 16
        KEY_N,          // 17
        KEY_O,          // 18
        KEY_P,          // 19
        KEY_Q,          // 20
        KEY_R,          // 21
        KEY_S,          // 22
        KEY_T,          // 23
        KEY_U,          // 24
        KEY_V,          // 25
        KEY_W,          // 26
        KEY_X,          // 27
        KEY_Y,          // 28
        KEY_Z,          // 29
        KEY_1,          // 30
        KEY_2,          // 31
        KEY_3,          // 32
        KEY_4,          // 33
        KEY_5,          // 34
        KEY_6,          // 35
        KEY_7,          // 36
        KEY_8,          // 37
        KEY_9,          // 38
        KEY_0,          // 39
        KEY_ENTER,      // 40
        KEY_ESCAPE,     // 41
        KEY_BACKSPACE,  // 42
        KEY_TAB,        // 43
        KEY_SPACE,      // 44
        KEY_MINUS,      // 45
        KEY_EQUAL,      // 46
        KEY_LBRACKET,   // 47
        KEY_RBRACKET,   // 48
        BUTTON_CODE_INVALID, // 49
        BUTTON_CODE_INVALID, // 50
        KEY_SEMICOLON,  // 51
        KEY_APOSTROPHE, // 52
        KEY_BACKQUOTE,  // 53
        KEY_COMMA,      // 54
        KEY_PERIOD,     // 55
        KEY_SLASH,      // 56
        KEY_CAPSLOCK,   // 57
        KEY_F1,         // 58
        KEY_F2,         // 59
        KEY_F3,         // 60
        KEY_F4,         // 61
        KEY_F5,         // 62
        KEY_F6,         // 63
        KEY_F7,         // 64
        KEY_F8,         // 65
        KEY_F9,         // 66
        KEY_F10,        // 67
        KEY_F11,        // 68
        KEY_F12,        // 69
        BUTTON_CODE_INVALID, // 70
        KEY_SCROLLLOCK, // 71
        KEY_BREAK,      // 72
        KEY_INSERT,     // 73
        KEY_HOME,       // 74
        KEY_PAGEUP,     // 75
        KEY_DELETE,     // 76
        KEY_END,        // 77
        KEY_PAGEDOWN,   // 78
        KEY_RIGHT,      // 79
        KEY_LEFT,       // 80
        KEY_DOWN,       // 81
        KEY_UP,         // 82
        KEY_NUMLOCK,    // 83
        KEY_PAD_DIVIDE, // 84
        KEY_PAD_MULTIPLY, // 85
        KEY_PAD_MINUS,  // 86
        KEY_PAD_PLUS,   // 87
        KEY_PAD_ENTER,  // 88
        KEY_PAD_1,      // 89
        KEY_PAD_2,      // 90
        KEY_PAD_3,      // 91
        KEY_PAD_4,      // 92
        KEY_PAD_5,      // 93
        KEY_PAD_6,      // 94
        KEY_PAD_7,      // 95
        KEY_PAD_8,      // 96
        KEY_PAD_9,      // 97
        KEY_PAD_0,      // 98
        KEY_PAD_DECIMAL, // 99
        KEY_BACKSLASH,  // 100
        KEY_APP,        // 101
    };

    // Check if the bit position is within our mapping range
    const size_t mapSize = sizeof(bitPositionToKey) / sizeof(bitPositionToKey[0]);
    if (bitPosition < mapSize)
    {
        return bitPositionToKey[bitPosition];
    }

    return BUTTON_CODE_INVALID;
}

void PostUserEvent(int m_nType, int m_nData = 0, int m_nData2 = 0, int m_nData3 = 0, int m_nTick = 0)
{
    InputEvent_t event;
    memset(&event, 0, sizeof(event));

    event.m_nType = m_nType;

    if (m_nData)
    {
        event.m_nData = m_nData;
    }
    if (m_nData2)
    {
        event.m_nData2 = m_nData2;
    }
    if (m_nData3)
    {
        event.m_nData3 = m_nData3;
    }

    if (m_nTick)
    {
        event.m_nTick = m_nTick;
    }
    else
    {
        auto CInputSystem__GetPollTick = (*(int (**)(void *))(*(uintptr_t *)g_pInputSystem + Offsets::CInputSystem__GetPollTick));
        event.m_nTick = CInputSystem__GetPollTick(g_pInputSystem);
    }

    // Post the event
    auto CInputSystem__PostUserEvent = *reinterpret_cast<void (**)(void *, const InputEvent_t &)>(*reinterpret_cast<uintptr_t *>(g_pInputSystem) + Offsets::CInputSystem__PostUserEvent);
    CInputSystem__PostUserEvent(g_pInputSystem, event);
}

bool IsKeyBitSet(const nn::hid::KeyboardState& kbState, uint8_t bitPosition)
{
    int arrayIndex = bitPosition / 64;          // Calculate which 64-bit block the bit is in
    int bitInArray = bitPosition % 64;          // Calculate the bit position within that block
    uint64_t keyBitmask = 1ULL << bitInArray;   // Create the bit mask for that position

    // Check if the bit is set in the appropriate array element
    return (kbState.keys[arrayIndex] & keyBitmask) != 0;
}

bool IsKeyNewlyPressed(const nn::hid::KeyboardState& kbState, const nn::hid::KeyboardState& prevKBKeys, uint8_t bitPosition)
{
    int arrayIndex = bitPosition / 64;          // Calculate which 64-bit block the bit is in
    int bitInArray = bitPosition % 64;          // Calculate the bit position within that block
    uint64_t keyBitmask = 1ULL << bitInArray;   // Create the bit mask for that position

    // Check if the bit is set in current state but not in previous state
    return ((kbState.keys[arrayIndex] & keyBitmask) != 0) &&
           ((prevKBKeys.keys[arrayIndex] & keyBitmask) == 0);
}

void ProcessKeyboardState(const nn::hid::KeyboardState& kbState, nn::hid::KeyboardState& prevKBKeys)
{
    // Map modifier bits to key codes
    static const struct
    {
        uint64_t modifierBit;
        ButtonCode_t keyCode;
    } modifierMap[] = {
        { (1 << 0), KEY_LCONTROL },
        { (1 << 1), KEY_LSHIFT },
        { (1 << 2), KEY_LALT },
        { (1 << 3), KEY_LWIN },
        // { (1 << 4), ??? }, // TODO: See what this missing value is
        { (1 << 5), KEY_RSHIFT },
        { (1 << 6), KEY_RALT },
        { (1 << 7), KEY_RWIN },
    };

    // Create a modifiable copy of the current state
    nn::hid::KeyboardState patchedKBState = kbState;

    // Manually patch in the modifier key states into the bitfield
    for (const auto& mod : modifierMap)
    {
        if (kbState.modifiers & mod.modifierBit)
        {
            uint8_t bit = ButtonCodeToBitPosition(mod.keyCode);
            if (bit != 0xFF)
            {
                patchedKBState.keys[bit / 8] |= (1 << (bit % 8));
            }
        }
    }

    // Loop through all keys
    for (int keyCode = KEY_FIRST; keyCode <= KEY_LAST; keyCode++)
    {
        uint8_t bitPosition = ButtonCodeToBitPosition((ButtonCode_t)keyCode);
        if (bitPosition == 0xFF) continue;

        bool wasDown = IsKeyBitSet(prevKBKeys, bitPosition);
        bool isDown  = IsKeyBitSet(patchedKBState, bitPosition);

        // TODO: Pressing KEY_ENTER will cause KEY_PAD_ENTER to press as well...
        if (isDown && !wasDown)
        {
            PostUserEvent(IE_ButtonPressed, keyCode, keyCode); // m_nData2 is what handles the vgui
            PostUserEvent(IE_KeyCodeTyped, keyCode);
        }
        else if (!isDown && wasDown)
        {
            PostUserEvent(IE_ButtonReleased, keyCode, keyCode); // m_nData2 is what handles the vgui
        }
    }

    // Update previous keyboard state
    prevKBKeys = patchedKBState;
}

void ProcessKeyboardTypingInput(const nn::hid::KeyboardState& kbState, uint64_t& prevKBTypingInput)
{
    // Detect newly pressed keys by comparing with previous state
    uint64_t newPresses = kbState.keys[0] & ~prevKBTypingInput;
    // uint64_t newReleases = ~kbState.keys[0] & prevKBTypingInput; // Uncomment if needed

    // Check if either Left Shift or Right Shift is held
    bool shiftHeld = (kbState.modifiers & ((1 << 1) | (1 << 5))) != 0;

    // TODO: Also check if either Right Control is held
    bool controlHeld = (kbState.modifiers & (1 << 0)) != 0;

    // Define key mappings for Unicode input
    static const struct
    {
        uint8_t bitPos;
        int lowerChar;  // Character when shift is not pressed
        int upperChar;  // Character when shift is pressed
    } unicodeKeyMap[] = {
        // Letters
        { 4, 'a', 'A' },
        { 5, 'b', 'B' },
        { 6, 'c', 'C' },
        { 7, 'd', 'D' },
        { 8, 'e', 'E' },
        { 9, 'f', 'F' },
        { 10, 'g', 'G' },
        { 11, 'h', 'H' },
        { 12, 'i', 'I' },
        { 13, 'j', 'J' },
        { 14, 'k', 'K' },
        { 15, 'l', 'L' },
        { 16, 'm', 'M' },
        { 17, 'n', 'N' },
        { 18, 'o', 'O' },
        { 19, 'p', 'P' },
        { 20, 'q', 'Q' },
        { 21, 'r', 'R' },
        { 22, 's', 'S' },
        { 23, 't', 'T' },
        { 24, 'u', 'U' },
        { 25, 'v', 'V' },
        { 26, 'w', 'W' },
        { 27, 'x', 'X' },
        { 28, 'y', 'Y' },
        { 29, 'z', 'Z' },

        // Numbers
        { 30, '1', '!' },
        { 31, '2', '@' },
        { 32, '3', '#' },
        { 33, '4', '$' },
        { 34, '5', '%' },
        { 35, '6', '^' },
        { 36, '7', '&' },
        { 37, '8', '*' },
        { 38, '9', '(' },
        { 39, '0', ')' },

        // Special characters
        { 44, ' ', ' ' },   // Space
        { 45, '-', '_' },   // Minus/Underscore
        { 46, '=', '+' },   // Equal/Plus
        { 47, '[', '{' },   // Left Bracket/Brace
        { 48, ']', '}' },   // Right Bracket/Brace
        { 51, ';', ':' },   // Semicolon/Colon
        { 52, '\'', '"' },  // Apostrophe/Quote
        { 53, '`', '~' },   // Backtick/Tilde
        { 54, ',', '<' },   // Comma/Less Than
        { 55, '.', '>' },   // Period/Greater Than
        { 56, '/', '?' },   // Slash/Question Mark
        // { 100, '\\', '|' }, // Backslash/Pipe (TODO: From my testing, the backslash key does absolutely nothing. If this is reimplemented, increase the size of uint64_t mask)
    };

    const int unicodeKeyMapSize = sizeof(unicodeKeyMap) / sizeof(unicodeKeyMap[0]);

    // Process newly pressed keys for Unicode input
    // Allows us to actually input text in text fields (console, chat)
    for (int i = 0; i < unicodeKeyMapSize; i++)
    {
        uint8_t bitPos = unicodeKeyMap[i].bitPos;
        uint64_t mask = 1ULL << bitPos;

        // Check if this bit is newly pressed
        if ((newPresses & mask) != 0)
        {
            ButtonCode_t buttonCode = BitPositionToButtonCode(bitPos);
            int unicodeChar = shiftHeld ? unicodeKeyMap[i].upperChar : unicodeKeyMap[i].lowerChar;

            // Try to mimic the CocoaEvent_KeyDown structure towards the bottom
            if (!controlHeld)
            {
                PostUserEvent(IE_KeyTyped, unicodeChar);
            }
            else
            {
                if (buttonCode == KEY_A || buttonCode == KEY_C || buttonCode == KEY_V || buttonCode == KEY_X)
                {
                    PostUserEvent(IE_KeyTyped, unicodeChar - 96); // Allow these to go through
                }
            }
        }
    }

    // Update previous state
    prevKBTypingInput = kbState.keys[0];
}

// A remake of CInputSystem::UpdateMousePositionState
void MouseMove(const nn::hid::MouseState &mouseState)
{
    short x = mouseState.x;
    short y = mouseState.y;

    // Constants
    const int MOUSE_X = 0;
    const int MOUSE_Y = 1;
    const int MOUSE_XY = 2;

    // ---- Questionable start

    // Calculate offsets based on InputState_t structure (some lazy guesswork)
    const uintptr_t inputStateOffset = 8264;

    // More lazy guesswork
    const uintptr_t buttonStateSize = (104 + 31) / 32 * 4; // 104 = BUTTON_CODE_LAST, round up to nearest 32
    uintptr_t buttonTickArraySize = (g_Plugin.IsGamePortal2()) ? (208 * sizeof(int)) : 630 * sizeof(int); // For both pressed and released ticks
    const uintptr_t analogDeltaOffset = inputStateOffset + buttonStateSize + buttonTickArraySize;
    const uintptr_t analogValueOffset = (g_Plugin.IsGamePortal2()) ? (analogDeltaOffset + (32 * sizeof(int))) : (analogDeltaOffset + (28 * sizeof(int))); // 32/28 = ANALOG_CODE_LAST

    // ---- Questionable end

    // Get pointers to the arrays
    int* pAnalogDelta = reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(g_pInputSystem) + analogDeltaOffset);
    int* pAnalogValue = reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(g_pInputSystem) + analogValueOffset);

    // Store old values
    int nOldX = pAnalogValue[MOUSE_X];
    int nOldY = pAnalogValue[MOUSE_Y];

    // Update values and calculate deltas
    pAnalogValue[MOUSE_X] = x;
    pAnalogValue[MOUSE_Y] = y;
    pAnalogDelta[MOUSE_X] = pAnalogValue[MOUSE_X] - nOldX;
    pAnalogDelta[MOUSE_Y] = pAnalogValue[MOUSE_Y] - nOldY;

    // Get last sample tick
    int CInputSystem__m_nLastSampleTick = *(int *)((uintptr_t)g_pInputSystem + Offsets::CInputSystem__m_nLastSampleTick);

    if (pAnalogDelta[MOUSE_X] != 0)
    {
        PostUserEvent(IE_AnalogValueChanged, MOUSE_X, pAnalogValue[MOUSE_X], pAnalogDelta[MOUSE_X], CInputSystem__m_nLastSampleTick);
    }

    if (pAnalogDelta[MOUSE_Y] != 0)
    {
        PostUserEvent(IE_AnalogValueChanged, MOUSE_Y, pAnalogValue[MOUSE_Y], pAnalogDelta[MOUSE_Y], CInputSystem__m_nLastSampleTick);
    }

    if (pAnalogDelta[MOUSE_X] != 0 || pAnalogDelta[MOUSE_Y] != 0)
    {
        PostUserEvent(IE_AnalogValueChanged, MOUSE_XY, pAnalogValue[MOUSE_X], pAnalogValue[MOUSE_Y], CInputSystem__m_nLastSampleTick);
    }
}

void ProcessMouseState(const nn::hid::MouseState& mouseState)
{
    // https://switchbrew.org/wiki/HID_services#MouseButton
    static const struct { int buttonCode; uint32_t bitMask; } mouseButtonMap[] = {
        { MOUSE_LEFT, 1 }, { MOUSE_MIDDLE, 2 }, { MOUSE_RIGHT, 4 }, { MOUSE_5, 8 }, { MOUSE_4, 16 }
    };

    static uint32_t prevMouseState = 0;
    static bool wasMouseButtonPressed[MOUSE_COUNT] = { false };

    // Process mouse buttons
    for (size_t i = 0; i < sizeof(mouseButtonMap) / sizeof(mouseButtonMap[0]); i++)
    {
        int buttonCode = mouseButtonMap[i].buttonCode;
        uint32_t buttonBitMask = mouseButtonMap[i].bitMask;

        // Check if the button state has changed
        bool currentState = (mouseState.Buttons & buttonBitMask) != 0;
        bool previousState = (prevMouseState & buttonBitMask) != 0;

        if (currentState != previousState)
        {
            // Update the bit in the previous state
            if (currentState)
                prevMouseState |= buttonBitMask;
            else
                prevMouseState &= ~buttonBitMask;

            if (currentState)
            {
                if (!wasMouseButtonPressed[buttonCode - MOUSE_FIRST])
                {
                    wasMouseButtonPressed[buttonCode - MOUSE_FIRST] = true;

                    PostUserEvent(IE_ButtonPressed, buttonCode, buttonCode); // m_nData2 is what allows this to interact with vgui
                }

                // Note: When it comes to double clicking, it straight up does not fire the function even when I tried setting it up correctly
                // It will be activated through a detour
            }
            else
            {
                if (wasMouseButtonPressed[buttonCode - MOUSE_FIRST])
                {
                    wasMouseButtonPressed[buttonCode - MOUSE_FIRST] = false;

                    PostUserEvent(IE_ButtonReleased, buttonCode, buttonCode); // m_nData2 is what allows this to interact with vgui
                }
            }
        }
    }

    // Handle mouse wheel events
    if (mouseState.wheelDeltaY != 0)
    {
        if (mouseState.wheelDeltaY > 0)
        {
            PostUserEvent(IE_ButtonPressed, MOUSE_WHEEL_UP);
            PostUserEvent(IE_ButtonReleased, MOUSE_WHEEL_UP);
        }
        else if (mouseState.wheelDeltaY < 0)
        {
            PostUserEvent(IE_ButtonPressed, MOUSE_WHEEL_DOWN);
            PostUserEvent(IE_ButtonReleased, MOUSE_WHEEL_DOWN);
        }

        // Note: There is no member var that gives us the analog value of the mouse wheel itself, so will make a var that does it
        // See CocoaEvent_MouseScroll in CInputSystem::PollInputState_Linux
        const int MOUSE_WHEEL = 3;
        static int prevMouseWheelAnalog = 0; // Just set it to 0 by default
        prevMouseWheelAnalog += mouseState.wheelDeltaY;
        PostUserEvent(IE_AnalogValueChanged, MOUSE_WHEEL, prevMouseWheelAnalog, mouseState.wheelDeltaY); // This is what allows us to interact with vgui
    }

    // Process mouse movement (Taken from CInputSystem::UpdateMousePositionState)
    // Update: Turns out they optimized a check for m_rawinput in CInput::GetAccumulatedMouseDeltasAndResetAccumulators, so it's impossible to hook it up with inputsystem lol
    // The solution: Hook CEngineClient::GetMouseDelta and return the actual delta values of the mouse since that will be called from a bit earlier in CInput::AccumulateMouse
    MouseMove(mouseState);
    PostUserEvent(IE_LocateMouseClick, mouseState.x, mouseState.y); // Updates the in-game cursor position for menus
}

extern void *engineClient;
void ScaleMouse(nn::hid::MouseState &mouseState)
{
    // Docked:      x: 1920, y: 1080
    // Handheld:    x: 1280, y: 720
    int screenWidth = 0;
    int screenHeight = 0;

    // Make it modular in case of new updates for bigger resolutions or something
    auto CEngineClient__GetScreenSize = *reinterpret_cast<void (**)(void *, int &, int &)>(*reinterpret_cast<uintptr_t *>(engineClient) + (sizeof(uintptr_t) * 5));
    CEngineClient__GetScreenSize(engineClient, screenWidth, screenHeight);

    // Base resolution for the mouse state is 1280x720 (handheld)
    constexpr float baseWidth = 1280.0f;
    constexpr float baseHeight = 720.0f;

    if (screenWidth > 0 && screenHeight > 0)
    {
        float widthScale = static_cast<float>(screenWidth) / baseWidth;
        float heightScale = static_cast<float>(screenHeight) / baseHeight;

        mouseState.x = static_cast<int>(mouseState.x * widthScale);
        mouseState.y = static_cast<int>(mouseState.y * heightScale);
    }
}

bool bShouldExitThread = false;
void InputThreadMain(void* arg)
{
    while (!bShouldExitThread)
    {
        if (g_pInputSystem)
        {
            if (nx_enable_keyboard_support.GetBool())
            {
                nn::hid::KeyboardState kbState;
                nn::hid::detail::GetKeyboardState(&kbState);

                // Translate button keys to binds and such for in-game use
                static nn::hid::KeyboardState prevKBKeys;
                ProcessKeyboardState(kbState, prevKBKeys);

                // Translate button keys into actual characters that can be used to type in menus
                static uint64_t prevKBTypingInput = 0;
                ProcessKeyboardTypingInput(kbState, prevKBTypingInput);
            }

            if (nx_enable_mouse_support.GetBool())
            {
                nn::hid::MouseState mouseState;
                nn::hid::detail::GetMouseState(&mouseState);

                // Note: These mouse values are only ever consistent with the handheld screen size resolution
                // Make it work properly when docked
                ScaleMouse(mouseState);

                // Translate button keys to binds and such for in-game use
                ProcessMouseState(mouseState);

                // TODO: Something stemming from Menu::OnCursorEnteredMenuItem crashes for some reason when hovering over menu options
                // Updated note: Fixed through the detours

                // The last mouse process is found in Detours.cpp
            }

            // Check out UserRequestingMovieSkip in engine.rno 0x710035BC00
            // auto CInputSystem__IsButtonDown = (*(bool (**)(void *, ButtonCode_t))(*(uintptr_t *)g_pInputSystem + 112));
        }

        nn::os::YieldThread();
        nn::os::SleepThread(nn::TimeSpan::FromNanoSeconds(10'000'000)); // Run every 0.01 second
    }
}

nn::os::ThreadType g_InputThreadType;

extern "C" void *memalign(size_t alignment, size_t size);
void InitNXInput(bool bIsPortal2Build)
{
    if (bIsPortal2Build)
    {
        IE_LocateMouseClick = 200;
        IE_KeyTyped = 202;
        IE_KeyCodeTyped = 203;
    }

    const size_t stackSize = 0x3000;
    void *threadStack = memalign(0x1000, stackSize);

    bShouldExitThread = false;
    nn::os::CreateThread(&g_InputThreadType, InputThreadMain, nullptr, threadStack, stackSize, 16, 0);
    nn::os::StartThread(&g_InputThreadType);
}

void ShutdownNXInput()
{
    bShouldExitThread = true;
    nn::os::WaitThread(&g_InputThreadType);
    nn::os::DestroyThread(&g_InputThreadType);
}

extern "C" void Msg( const char* pMsg, ... );

// Add a copy of Saul's cvar unhiding functionality
#include "Command.hpp"
extern void *g_pCVar;
CON_COMMAND(cvar_unhide_all, "Unhide all FCVAR_HIDDEN and FCVAR_DEVELOPMENTONLY convars")
{
	// Unhide dev cvars
    FOR_ALL_CONSOLE_COMMANDS( pCommand )
    {
        if ( pCommand->IsFlagSet( FCVAR_DEVELOPMENTONLY ) || pCommand->IsFlagSet( FCVAR_HIDDEN ) )
        {
            pCommand->RemoveFlags( FCVAR_DEVELOPMENTONLY | FCVAR_HIDDEN );
        }
    }
}

DECL_DECLARE_AUTOCOMPLETION_FUNCTION(nx_gamecube_input_support)
{
	const char *pszCMD = "nx_gamecube_input_support ";
	const char *pszPresets[] = { "add", "remove" };

	const char *pSubstring = partial + strlen( pszCMD );
	int iSlots = 0;
	for ( const char *pzsPreset : pszPresets )
	{
		if ( strncasecmp( pzsPreset, pSubstring, strlen( pSubstring ) ) )
		{
			continue;
		}

		snprintf( commands[ iSlots ], sizeof( commands[ iSlots ] ), "%s%s", pszCMD, pzsPreset );

		// Make sure we don't go over the max allowed
		++iSlots;
		if ( iSlots >= COMMAND_COMPLETION_MAXITEMS )
		{
			break;
		}
	}
	return iSlots;
}
CON_COMMAND_F_COMPLETION(nx_gamecube_input_support, "Modifies whether polling for GameCube controllers is done.", FCVAR_NONE, AUTOCOMPLETION_FUNCTION(nx_gamecube_input_support))
{
    if (args.ArgC() < 2)
    {
        Msg("Not enough args!\n");
        return;
    }

    if (strcmp("add", args[1]) && strcmp("remove", args[1]))
    {
        Msg("Wrong args!\n");
        return;
    }

    nn::util::BitFlagSet<32, nn::hid::NpadStyleTag> gameFlags;
    nn::hid::detail::GetSupportedNpadStyleSet(&gameFlags);

    if (!strcmp("add", args[1]))
    {
        gameFlags.flags = (uint32_t)(gameFlags.flags | nn::hid::NpadStyleTag::GameCube);
    }
    else if (!strcmp("remove", args[1]))
    {
        gameFlags.flags = (uint32_t)(gameFlags.flags & ~nn::hid::NpadStyleTag::GameCube);
    }

    nn::hid::detail::SetSupportedNpadStyleSet(gameFlags);
}

CON_COMMAND(nx_get_gamecube_controller_state, "Test command for GameCube controllers.")
{
    int outputCount;
    nn::hid::NpadGcState GcState;
    int maxStates = 1;
    unsigned int controllerIdType = nn::hid::NpadIdType_Player1;
    nn::hid::detail::GetNpadStates(&outputCount, &GcState, maxStates, controllerIdType);

    Msg("GcState.sampling_number: %d\n", GcState.sampling_number);
    Msg("GcState.buttons: %d\n", GcState.buttons);
    Msg("GcState.analog_stick_l: x: %d, y: %d\n", GcState.analog_stick_l.x, GcState.analog_stick_l.y);
    Msg("GcState.analog_stick_r: x: %d, y: %d\n", GcState.analog_stick_r.x, GcState.analog_stick_r.y);
    Msg("GcState.trigger_l: %d\n", GcState.trigger_l);
    Msg("GcState.trigger_r: %d\n", GcState.trigger_r);
}
