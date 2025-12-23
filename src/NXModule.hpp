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
    }

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

        struct NpadGcState
        {
            uint64_t sampling_number;
            uint64_t buttons;                   // Bitfield of nn::hid::NpadButton
            AnalogStickState analog_stick_l;
            AnalogStickState analog_stick_r;
            uint32_t attributes;
            uint32_t trigger_l;                 // Valid range: 0x0-0x7FFF
            uint32_t trigger_r;                 // Valid range: 0x0-0x7FFF
            uint32_t pad;
        };

        enum NpadIdType
        {
            NpadIdType_Player1      = 0,
            NpadIdType_Player2      = 1,
            NpadIdType_Player3      = 2,
            NpadIdType_Player4      = 3,
            NpadIdType_Player5      = 4,
            NpadIdType_Player6      = 5,
            NpadIdType_Player7      = 6,
            NpadIdType_Player8      = 7,
            NpadIdType_Other        = 0x10,
            NpadIdType_Handheld     = 0x20
        };

        enum NpadStyleTag
        {
            ProController = 1 << 0,
            Handheld = 1 << 1,
            JoyconPair = 1 << 2,
            JoyconLeft = 1 << 3,
            JoyconRight = 1 << 4,
            GameCube = 1 << 5
        };

        namespace detail
        {
            void InitializeKeyboard();
            void GetKeyboardState(nn::hid::KeyboardState *);

            void InitializeMouse();
            void GetMouseState(nn::hid::MouseState *);

            void GetSupportedNpadStyleSet(nn::util::BitFlagSet<32,nn::hid::NpadStyleTag> *);
            void SetSupportedNpadStyleSet(nn::util::BitFlagSet<32,nn::hid::NpadStyleTag>);
            void GetNpadStates(int *, nn::hid::NpadGcState *, int, unsigned int const &);
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

    namespace os
    {
        namespace detail
        {
            class InternalCriticalSection
            {
                uint32_t Image;
            };

            class InternalConditionVariable
            {
                uint32_t Image;
            };
        }  // namespace detail

        struct ThreadType
        {
            uint8_t _0[0x40];
            uint32_t State;
            bool _44;
            bool _45;
            uint8_t _46;
            uint32_t PriorityBase;
            void* StackBase;
            void* Stack;
            size_t StackSize;
            void* Arg;
            uint64_t ThreadFunc;
            uint8_t _88[0x100];
            char Name[0x20];
            detail::InternalCriticalSection Crit;
            detail::InternalConditionVariable Condvar;
            uint32_t Handle;
            uint8_t padding[0x18];

            ThreadType(){};
        };

        uint32_t CreateThread(nn::os::ThreadType*, void (*)(void*), void*, void*, uint64_t, int32_t, int32_t);
        void StartThread(nn::os::ThreadType*);
        void DestroyThread(nn::os::ThreadType*);
        void WaitThread(nn::os::ThreadType *);
        void YieldThread();
        void SleepThread(nn::TimeSpan);
    } // namespace os
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
