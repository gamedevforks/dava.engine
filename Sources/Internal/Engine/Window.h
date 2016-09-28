#pragma once

#if defined(__DAVAENGINE_COREV2__)

#include "Base/BaseTypes.h"
#include "Functional/Signal.h"
#include "Math/Vector.h"

#include "UI/UIEvent.h"

#include "Engine/Private/EnginePrivateFwd.h"
#include "Engine/Private/EngineBackend.h"

namespace rhi
{
struct InitParam;
}

namespace DAVA
{
class InputSystem;
class UIControlSystem;
class VirtualCoordinatesSystem;

class Window final
{
    friend class Private::EngineBackend;
    friend class Private::PlatformCore;

private:
    Window(Private::EngineBackend* engineBackend, bool primary);
    ~Window();

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

public:
    bool IsPrimary() const;
    bool IsVisible() const;
    bool HasFocus() const;

    float32 GetDPI() const;
    Size2f GetSize() const;
    Size2f GetPhysicalSize() const;

    float GetUserScale() const;
    void SetUsetScale(float scale);

    void Resize(const Size2f& size);
    void Close();
    void SetTitle(const String& title);

    Engine* GetEngine() const;
    void* GetNativeHandle() const;
    WindowNativeService* GetNativeService() const;

    void RunAsyncOnUIThread(const Function<void()>& task);

public:
    // Signals
    Signal<Window&, bool> visibilityChanged;
    Signal<Window&, bool> focusChanged;
    Signal<Window&> destroyed;
    Signal<Window&, Size2f> sizeChanged;
    Signal<Window&, Size2f> physicalSizeChanged;
    //Signal<Window&> beginUpdate;
    //Signal<Window&> beginDraw;
    Signal<Window&, float32> update;
    //Signal<Window&> endDraw;
    //Signal<Window&> endUpdate;

private:
    /// Get pointer to WindowBackend which may be used by PlatformCore
    Private::WindowBackend* GetBackend() const;

    /// Initialize platform specific render params, e.g. acquire/release context functions for Qt platform
    void InitCustomRenderParams(rhi::InitParam& params);
    void Update(float32 frameDelta);
    void Draw();

    /// Process main dispatcher events targeting this window
    void EventHandler(const Private::MainDispatcherEvent& e);
    /// Do some window specific tasks after all dispatcher events have been processed on current frame,
    /// e.g. initiate processing tasks on window UI thread
    void FinishEventHandlingOnCurrentFrame();

    void HandleWindowCreated(const Private::MainDispatcherEvent& e);
    void HandleWindowDestroyed(const Private::MainDispatcherEvent& e);
    void HandleSizeChanged(const Private::MainDispatcherEvent& e);
    void HandleFocusChanged(const Private::MainDispatcherEvent& e);
    void HandleVisibilityChanged(const Private::MainDispatcherEvent& e);
    void HandleMouseClick(const Private::MainDispatcherEvent& e);
    void HandleMouseWheel(const Private::MainDispatcherEvent& e);
    void HandleMouseMove(const Private::MainDispatcherEvent& e);
    void HandleTouchClick(const Private::MainDispatcherEvent& e);
    void HandleTouchMove(const Private::MainDispatcherEvent& e);
    void HandleKeyPress(const Private::MainDispatcherEvent& e);
    void HandleKeyChar(const Private::MainDispatcherEvent& e);

    void UpdateVirtualCoordinatesSystem();
    void ClearMouseButtons();

private:
    // TODO: unique_ptr
    Private::EngineBackend& engineBackend;
    Private::MainDispatcher& mainDispatcher;
    Private::WindowBackend* windowBackend = nullptr;

    InputSystem* inputSystem = nullptr;
    UIControlSystem* uiControlSystem = nullptr;

    bool isPrimary = false;
    bool isVisible = false;
    bool hasFocus = false;
    bool sizeEventHandled = false; // Flag indicating that compressed size events are handled on current frame

    float32 dpi = 0;
    float32 userScale = 1.0f;
    Size2f size = { 0.0f, 0.0f };
    Size2f physicalSize = { 0.0f, 0.0f };

    Bitset<static_cast<size_t>(UIEvent::MouseButton::NUM_BUTTONS)> mouseButtonState;
};

inline bool Window::IsPrimary() const
{
    return isPrimary;
}

inline bool Window::IsVisible() const
{
    return isVisible;
}

inline bool Window::HasFocus() const
{
    return hasFocus;
}

/*
inline float32 Window::GetWidth() const
{
    return width;
}

inline float32 Window::GetHeight() const
{
    return height;
}

inline float32 Window::GetRenderSurfaceWidth() const
{
    return width * scaleX * userScale;
}

inline float32 Window::GetRenderSurfaceHeight() const
{
    return height * scaleY * userScale;
}

inline float32 Window::GetScaleX() const
{
    return scaleX;
}

inline float32 Window::GetScaleY() const
{
    return scaleY;
}

inline float32 Window::GetUserScale() const
{
    return userScale;
}

inline float32 Window::GetRenderSurfaceScaleX() const
{
    return scaleX * userScale;
}

inline float32 Window::GetRenderSurfaceScaleY() const
{
    return scaleY * userScale;
}

inline Vector2 Window::GetSize() const
{
    return Vector2(GetWidth(), GetHeight());
}

inline Vector2 Window::GetScale() const
{
    return Vector2(GetScaleX(), GetScaleY());
}

inline void Window::Resize(Vector2 size)
{
    Resize(size.dx, size.dy);
}
*/

inline float32 Window::GetDPI() const
{
    return dpi;
}

inline Size2f Window::GetSize() const
{
    return size;
}

inline Size2f Window::GetPhysicalSize() const
{
    return physicalSize;
}

inline float32 Window::GetUserScale() const
{
    return userScale;
}

inline Private::WindowBackend* Window::GetBackend() const
{
    return windowBackend;
}

} // namespace DAVA

#endif // __DAVAENGINE_COREV2__
