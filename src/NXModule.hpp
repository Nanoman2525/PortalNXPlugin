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
            RoModule *FindRoModuleById(const char *id);
        } // namespace detail
    } // namespace ro

    namespace util
    {
        template <int BitCount, typename Tag>
        class BitFlagSet
        {
        public:
            uint32_t flags;
        };
    } // namespace util

    struct Result
    {
        uint32_t value;
    };

    namespace hid
    {
        struct KeyboardState
        {
            int64_t samplingNumber;
            uint64_t modifiers;
            uint64_t keys[4];
        };

        struct MouseState
        {
            uint64_t samplingNumber;
            int32_t x;
            int32_t y;
            int32_t deltaX;
            int32_t deltaY;
            int32_t wheelDeltaX;
            int32_t wheelDeltaY;
            uint32_t Buttons;
        };

        struct AnalogStickState
        {
            int32_t x;
            int32_t y;
        };

        // 0x38 bytes per touch confirmed
        struct TouchState
        {
            int64_t    samplingNumber;  // +0x00
            int32_t    count;           // +0x08
            uint8_t    _pad[4];         // +0x0C
            int64_t    unk10;           // +0x10
            uint32_t   unk18;           // +0x18
            uint32_t   unk1C;           // +0x1C
            int32_t    x;               // +0x20
            int32_t    y;               // +0x24
            int32_t    unk28;           // +0x28
            int32_t    unk2C;           // +0x2C
            int32_t    unk30;           // +0x30
            uint8_t    _pad2[4];        // +0x34
        };
        static_assert(sizeof(TouchState) == 0x38, "bad size");

        template<size_t N>
        struct TouchScreenState
        {
            TouchState touches[N];  // array starts at offset 0
        };

        namespace detail
        {
            void GetKeyboardState(nn::hid::KeyboardState *);

            void GetMouseState(nn::hid::MouseState *);

            template <size_t N>
            nn::Result GetTouchScreenState(nn::hid::TouchScreenState<N>* state);
        } // namespace detail
    } // namespace hid

    class TimeSpan
    {
    public:
        uint64_t nanoseconds;

        static TimeSpan FromNanoSeconds(uint64_t nanoSeconds)
        {
            TimeSpan ret;
            ret.nanoseconds = nanoSeconds;
            return ret;
        }
    };

    namespace swkbd
    {
        enum Trigger : uint32_t
        {
            Trigger_Default = 0,
        };

        struct KeyboardConfig
        {
            uint8_t data[0x4D0]; // MakePresetDefault clears 1232 (0x4D0) bytes
        };

        // From assembly: offsets 0x4D0=workBufPtr, 0x4D8=workBufSize, 0x4F0=dictPtr, 0x4F8=dictSize
        struct ShowKeyboardArg
        {
            KeyboardConfig config;          // 0x000, size 0x4D0
            void*          workBufPtr;      // 0x4D0
            uint64_t       workBufSize;     // 0x4D8
            uint8_t        pad[0x10];       // 0x4E0..0x4EF
            void*          dictPtr;         // 0x4F0
            uint64_t       dictSize;        // 0x4F8
        };

        struct String
        {
            char*    ptr;
            uint64_t capacity; // bytes available, used as read size
            uint64_t length;   // written back after read, may be char count
        };

        void MakePresetDefault(KeyboardConfig*);
        void ShowKeyboard(String*, const ShowKeyboardArg&, Trigger);

        void SetHeaderTextUtf8(KeyboardConfig*, const char*);  // title at top
        void SetGuideTextUtf8(KeyboardConfig*, const char*);   // placeholder text in input field
    } // namespace swkbd
} // namespace nn

// Modules shared between both games...
// extern uintptr_t bsppacknrobase;
extern uintptr_t clientnrobase;
extern uintptr_t datacachenrobase;
extern uintptr_t enginenrobase;
extern uintptr_t filesystem_stdionrobase;
extern uintptr_t inputsystemnrobase;
extern uintptr_t launchernrobase;
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

// Portal-specific modules...
// extern uintptr_t bugreporter_filequeuenrobase;
// extern uintptr_t bugreporter_publicnrobase;
extern uintptr_t GameUInrobase;
// extern uintptr_t ServerBrowsernrobase;
// extern uintptr_t shaderapidx9nrobase;
// extern uintptr_t stdshader_dx9nrobase;
// extern uintptr_t toglnrobase;
extern uintptr_t video_nxnrobase;
extern uintptr_t video_servicesnrobase;
// extern uintptr_t vtex_dllnrobase;

// Portal 2-specific modules...
extern uintptr_t localizenrobase;
extern uintptr_t matchmakingnrobase;

extern bool InitNXModuleBases(bool bIsPortal2Build);

#endif // __SWITCH__

#endif // NXMODULE_HPP
