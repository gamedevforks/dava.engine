#include "Tests/UIStylesTest.h"
#include "Infrastructure/TestBed.h"
#include <Time/SystemTimer.h>
#include <UI/UIPackageLoader.h>
#include <UI/Input/UIActionBindingComponent.h>
#include <Utils/StringFormat.h>

using namespace DAVA;

static const FastName STYLE_ON = FastName("on");
static const FastName STYLE_OFF = FastName("off");

UIStylesTest::UIStylesTest(TestBed& app)
    : BaseScreen(app, "UIStylesTest")
{
}

void UIStylesTest::LoadResources()
{
    BaseScreen::LoadResources();

    DefaultUIPackageBuilder pkgBuilder;
    UIPackageLoader().LoadPackage("~res:/UI/StylesTest.yaml", &pkgBuilder);
    UIControl* main = pkgBuilder.GetPackage()->GetControl("Window");
    UIActionBindingComponent* actions = main->GetOrCreateComponent<UIActionBindingComponent>();
    if (actions)
    {
        actions->GetActionMap().Put(FastName("ADD"), [&]() {
            container->RemoveClass(STYLE_OFF);
            container->AddClass(STYLE_ON);
        });
        actions->GetActionMap().Put(FastName("REMOVE"), [&]() {
            container->RemoveClass(STYLE_ON);
            container->AddClass(STYLE_OFF);
        });
        actions->GetActionMap().Put(FastName("MORE"), [&]() {
            for (uint32 i = 0; i < 1000; ++i)
            {
                container->AddControl(proto->Clone());
            }
        });
    }

    proto = pkgBuilder.GetPackage()->GetControl("Proto");
    container = main->FindByName("Container");
    statusText = main->FindByName<UIStaticText>("StatusText");

    for (uint32 i = 0; i < 1000; ++i)
    {
        container->AddControl(proto->Clone());
    }

    AddControl(main);
}

void UIStylesTest::UnloadResources()
{
    BaseScreen::UnloadResources();
}

void UIStylesTest::Update(float32 delta)
{
    BaseScreen::Update(delta);
    statusText->SetUtf8Text(Format("FPS: %f, count: %d", 1. / SystemTimer::GetRealFrameDelta(), container->GetChildren().size()));
}
