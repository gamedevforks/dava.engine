#pragma once

#include "Base/BaseTypes.h"

#if defined(__DAVAENGINE_WIN32__)

namespace DAVA
{
class Gamepad;
namespace Private
{
struct MainDispatcherEvent;
class GamepadImpl final
{
public:
    GamepadImpl(Gamepad* gamepad);

    void Update();

    void HandleGamepadMotion(const MainDispatcherEvent&)
    {
    }
    void HandleGamepadButton(const MainDispatcherEvent&)
    {
    }

    bool HandleGamepadAdded(uint32 id);
    bool HandleGamepadRemoved(uint32 id);

    Gamepad* gamepadDevice = nullptr;
};

} // namespace Private
} // namespace DAVA

#endif // __DAVAENGINE_WIN32__
