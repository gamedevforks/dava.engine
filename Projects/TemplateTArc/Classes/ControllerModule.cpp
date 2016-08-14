#include "ControllerModule.h"

#include "SharedData.h"

#include "WindowSubSystem/UI.h"
#include "TArcCore/ContextAccessor.h"

#include <QListWidget>

void TemplateControllerModule::OnContextCreated(tarc::DataContext& context)
{
    context.CreateData(std::make_unique<SharedData>());
}

void TemplateControllerModule::OnContextDeleted(tarc::DataContext& context)
{
}

void TemplateControllerModule::PostInit(tarc::UI& ui)
{
    tarc::ContextManager& manager = GetContextManager();
    contextID = manager.CreateContext();
    manager.ActivateContext(contextID);
    wrapper = GetAccessor().CreateWrapper(DAVA::Type::Instance<SharedData>());
    wrapper.AddListener(this);

    tarc::CentralPanelInfo info;
    ui.AddView(tarc::WindowKey(DAVA::FastName("TemplateTArc"), "RenderWidget", info), manager.GetRenderWidget());
    manager.GetRenderWidget()->show();
}

void TemplateControllerModule::OnDataChanged(const tarc::DataWrapper&, const DAVA::Set<DAVA::String>& fields)
{
    tarc::DataContext::ContextID newContext = tarc::DataContext::Empty;
    if (wrapper.HasData())
    {
        tarc::DataEditor<SharedData> editor = wrapper.CreateEditor<SharedData>();
        DAVA::Logger::Info("Data changed. New value : %d", editor->GetValue());
    }
    else
    {
        DAVA::Logger::Info("Data changed. New value : empty");
        newContext = contextID;
    }
    //GetContextManager().ActivateContext(newContext);
}
