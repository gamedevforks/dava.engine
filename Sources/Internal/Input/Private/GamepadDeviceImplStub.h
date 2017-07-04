#pragma once

#include "Base/BaseTypes.h"

namespace DAVA
{
class GamepadDevice;
namespace Private
{
struct MainDispatcherEvent;

// Dummy GamepadDeviceImpl implementation for platforms that do not support gamepads for now.
class GamepadDeviceImpl final
{
public:
    GamepadDeviceImpl(GamepadDevice*)
    {
    }

    void Update()
    {
    }

    void HandleGamepadMotion(const MainDispatcherEvent&)
    {
    }
    void HandleGamepadButton(const MainDispatcherEvent&)
    {
    }

    bool HandleGamepadAdded(uint32)
    {
        return false;
    }
    bool HandleGamepadRemoved(uint32)
    {
        return false;
    }
};

} // namespace Private
} // namespace DAVA
