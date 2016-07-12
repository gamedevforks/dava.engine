#pragma once

#if defined(__DAVAENGINE_COREV2__)

#include "Base/BaseTypes.h"

#if defined(__DAVAENGINE_QT__)
// TODO: plarform defines
#elif defined(__DAVAENGINE_MACOS__)

#include "Functional/Signal.h"

#include "Engine/Private/EngineFwd.h"

namespace DAVA
{
namespace Private
{
class CoreOsX final
{
public:
    CoreOsX(EngineBackend* e);
    ~CoreOsX();

    NativeService* GetNativeService() const;

    void Init();
    void Run();
    void Quit();

    // WindowOsX gets notified about application hidden/unhidden state changing
    // to update its visibility state
    Signal<bool> didHideUnhide;

private:
    int OnFrame();

    WindowOsX* CreateNativeWindow(Window* w, float32 width, float32 height);

private:
    EngineBackend* engineBackend = nullptr;
    // TODO: std::unique_ptr
    CoreOsXObjcBridge* objcBridge = nullptr;
    std::unique_ptr<NativeService> nativeService;

    // Friends
    friend struct CoreOsXObjcBridge;
};

inline NativeService* CoreOsX::GetNativeService() const
{
    return nativeService.get();
}

} // namespace Private
} // namespace DAVA

#endif // __DAVAENGINE_MACOS__
#endif // __DAVAENGINE_COREV2__
