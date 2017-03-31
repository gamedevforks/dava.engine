#include "Input/KeyboardInputDevice.h"

#if defined(__DAVAENGINE_WIN32__)
#include "Input/Private/Win32/KeyboardDeviceImplWin32.h"
#else
#error "DeviceManager: unknown platform"
#endif

#include "Engine/Engine.h"
#include "Input/InputSystem.h"
#include "Time/SystemTimer.h"

namespace DAVA
{
KeyboardInputDevice::KeyboardInputDevice(uint32 id)
    : InputDevice(id)
    , inputSystem(GetEngineContext()->inputSystem)
    , impl(new Private::KeyboardDeviceImpl())
{
    Engine* engine = Engine::Instance();
    endFrameConnectionToken = engine->endFrame.Connect(this, &KeyboardInputDevice::OnEndFrame);
    primaryWindowFocusChangedToken = engine->PrimaryWindow()->focusChanged.Connect(this, &KeyboardInputDevice::OnWindowFocusChanged); // TODO: handle all the windows

    Private::EngineBackend::Instance()->InstallEventFilter(this, MakeFunction(this, &KeyboardInputDevice::HandleEvent));
}

KeyboardInputDevice::~KeyboardInputDevice()
{
    Engine* engine = Engine::Instance();
    engine->endFrame.Disconnect(endFrameConnectionToken);
    engine->PrimaryWindow()->focusChanged.Disconnect(primaryWindowFocusChangedToken);

    Private::EngineBackend::Instance()->UninstallEventFilter(this);

    if (impl != nullptr)
    {
        delete impl;
        impl = nullptr;
    }
}

bool KeyboardInputDevice::SupportsElement(eInputElements elementId) const
{
    return (elementId >= eInputElements::KB_FIRST) && (elementId <= eInputElements::KB_LAST);
}

eDigitalElementStates KeyboardInputDevice::GetDigitalElementState(eInputElements elementId) const
{
    DVASSERT(SupportsElement(elementId));

    if (elementId >= eInputElements::KB_FIRST_VIRTUAL && elementId <= eInputElements::KB_LAST_VIRTUAL)
    {
        elementId = ConvertVirtualToScancode(elementId);
    }

    return keys[elementId - eInputElements::KB_FIRST_SCANCODE].GetState();
}

AnalogElementState KeyboardInputDevice::GetAnalogElementState(eInputElements elementId) const
{
    DVASSERT(false, "KeyboardInputDevice does not support analog element");
    return {};
}

eInputElements KeyboardInputDevice::ConvertScancodeToVirtual(eInputElements scancodeElement) const
{
    return impl->ConvertDavaScancodeToDavaVirtual(scancodeElement);
}

eInputElements KeyboardInputDevice::ConvertVirtualToScancode(eInputElements virtualElement) const
{
    return impl->ConvertDavaVirtualToDavaScancode(virtualElement);
}

void KeyboardInputDevice::CreateAndSendInputEvent(eInputElements elementId, const Private::DigitalElement& element, Window* window, int64 timestamp)
{
    InputEvent inputEvent;
    inputEvent.window = window;
    inputEvent.timestamp = timestamp / 1000.0f;
    inputEvent.deviceType = eInputDeviceTypes::KEYBOARD;
    inputEvent.deviceId = GetId();
    inputEvent.digitalState = element.GetState();
    inputEvent.elementId = elementId;

    inputSystem->DispatchInputEvent(inputEvent);
}

bool KeyboardInputDevice::HandleEvent(const Private::MainDispatcherEvent& e)
{
    using Private::MainDispatcherEvent;

    if (e.type == MainDispatcherEvent::KEY_DOWN || e.type == MainDispatcherEvent::KEY_UP)
    {
        // Save state

        eInputElements scancodeElementId = impl->ConvertNativeScancodeToDavaScancode(e.keyEvent.key);
        Private::DigitalElement& element = keys[scancodeElementId - eInputElements::KB_FIRST_SCANCODE];

        if (e.type == MainDispatcherEvent::KEY_DOWN)
        {
            element.Press();
        }
        else
        {
            element.Release();
        }

        // Send event

        eInputElements virtualElementId = impl->ConvertDavaScancodeToDavaVirtual(scancodeElementId);
        CreateAndSendInputEvent(virtualElementId, element, e.window, e.timestamp);

        return true;
    }
    else
    {
        return false;
    }
}

void KeyboardInputDevice::OnEndFrame()
{
    // Promote JustPressed & JustReleased states to Pressed/Released accordingly
    // TODO: optimize?

    for (int i = 0; i < static_cast<uint32>(eInputElements::KB_COUNT_SCANCODE); ++i)
    {
        keys[i].OnEndFrame();
    }
}

void KeyboardInputDevice::OnWindowFocusChanged(DAVA::Window* window, bool focused)
{
    if (!focused)
    {
        for (int i = 0; i < static_cast<uint32>(eInputElements::KB_COUNT_SCANCODE); ++i)
        {
            if (keys[i].IsPressed())
            {
                keys[i].Release();
                CreateAndSendInputEvent(static_cast<eInputElements>(eInputElements::KB_FIRST_SCANCODE + i), keys[i], window, SystemTimer::GetMs());
            }
        }
    }
}
}