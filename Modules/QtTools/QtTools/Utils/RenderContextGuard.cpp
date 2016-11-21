#include "RenderContextGuard.h"

#if defined(__DAVAENGINE_COREV2__)
#include "Engine/Engine.h"
#include "Engine/Window.h"

RenderContextGuard::RenderContextGuard()
{
    DAVA::Engine* engine = DAVA::Engine::Instance();
    DVASSERT(engine != nullptr);
    if (!engine->IsConsoleMode())
    {
        DAVA::Window* window = engine->PrimaryWindow();
        DAVA::PlatformApi::AcqureWindowContext(window);
    }
}

RenderContextGuard::~RenderContextGuard()
{
    DAVA::Engine* engine = DAVA::Engine::Instance();
    DVASSERT(engine != nullptr);
    if (!engine->IsConsoleMode())
    {
        DAVA::Window* window = engine->PrimaryWindow();
        DAVA::PlatformApi::ReleaseWindowContext(window);
    }
}

#endif