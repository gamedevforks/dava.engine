#include "EditorSystems/EditorSystemsManager.h"

#include "Engine/Qt/RenderWidget.h"

#include "Model/PackageHierarchy/PackageNode.h"
#include "Model/PackageHierarchy/PackageControlsNode.h"
#include "Model/PackageHierarchy/ControlNode.h"

#include "EditorSystems/SelectionSystem.h"
#include "EditorSystems/EditorControlsView.h"
#include "EditorSystems/CursorSystem.h"
#include "EditorSystems/HUDSystem.h"
#include "EditorSystems/EditorTransformSystem.h"
#include "EditorSystems/KeyboardProxy.h"
#include "EditorSystems/EditorControlsView.h"
#include "EditorSystems/EditorCanvas.h"

#include "UI/UIControl.h"
#include "UI/Input/UIModalInputComponent.h"
#include "UI/Input/UIInputSystem.h"
#include "UI/UIControlSystem.h"
#include "UI/UIScreen.h"
#include "UI/UIScreenManager.h"

using namespace DAVA;

EditorSystemsManager::StopPredicate EditorSystemsManager::defaultStopPredicate = [](const ControlNode*) { return false; };

namespace EditorSystemsManagerDetails
{
class InputLayerControl : public UIControl
{
public:
    InputLayerControl(EditorSystemsManager* systemManager_)
        : UIControl()
        , systemManager(systemManager_)
    {
        GetOrCreateComponent<UIModalInputComponent>();
    }

    bool SystemProcessInput(UIEvent* currentInput) override
    {
        //redirect input from the framework to the editor
        systemManager->OnInput(currentInput);
        return true;
    }

private:
    EditorSystemsManager* systemManager = nullptr;
};
}

EditorSystemsManager::EditorSystemsManager(RenderWidget* renderWidget)
    : rootControl(new UIControl())
    , inputLayerControl(new EditorSystemsManagerDetails::InputLayerControl(this))
    , scalableControl(new UIControl())
    , editingRootControls(CompareByLCA)
{
    dragStateChanged.Connect(this, &EditorSystemsManager::OnDragStateChanged);
    displayStateChanged.Connect(this, &EditorSystemsManager::OnDisplayStateChanged);
    activeAreaChanged.Connect(this, &EditorSystemsManager::OnActiveHUDAreaChanged);

    rootControl->SetName(FastName("rootControl"));
    rootControl->AddControl(scalableControl.Get());
    inputLayerControl->SetName("inputLayerControl");
    rootControl->AddControl(inputLayerControl.Get());
    scalableControl->SetName(FastName("scalableContent"));

    InitDAVAScreen();

    packageChanged.Connect(this, &EditorSystemsManager::OnPackageChanged);
    selectionChanged.Connect(this, &EditorSystemsManager::OnSelectionChanged);

    controlViewPtr = new EditorControlsView(scalableControl.Get(), this);
    systems.emplace_back(controlViewPtr);

    selectionSystemPtr = new SelectionSystem(this);
    systems.emplace_back(selectionSystemPtr);
    systems.emplace_back(new HUDSystem(this));
    systems.emplace_back(new CursorSystem(renderWidget, this));
    systems.emplace_back(new ::EditorTransformSystem(this));
    editorCanvasPtr = new EditorCanvas(scalableControl.Get(), this);
    systems.emplace_back(editorCanvasPtr);

    for (auto it = systems.begin(); it != systems.end(); ++it)
    {
        const std::unique_ptr<BaseEditorSystem>& editorSystem = *it;
        BaseEditorSystem* editorSystemPtr = editorSystem.get();
        dragStateChanged.Connect(editorSystemPtr, &BaseEditorSystem::OnDragStateChanged);
        displayStateChanged.Connect(editorSystemPtr, &BaseEditorSystem::OnDisplayStateChanged);
    }
}

EditorSystemsManager::~EditorSystemsManager()
{
    UIScreenManager::Instance()->ResetScreen();
}

void EditorSystemsManager::OnInput(UIEvent* currentInput)
{
    if (currentInput->device == eInputDevices::MOUSE)
    {
        mouseDelta = currentInput->point - lastMousePos;
        lastMousePos = currentInput->point;
    }

    eDragState newState = NoDrag;
    for (auto it = systems.rbegin(); it != systems.rend(); ++it)
    {
        const std::unique_ptr<BaseEditorSystem>& editorSystem = *it;

        if (newState == NoDrag)
        {
            newState = editorSystem->RequireNewState(currentInput);
        }
        else
        {
            DVASSERT(editorSystem->RequireNewState(currentInput) == NoDrag, "Two different states required by systems on one input");
        }
    }
    SetDragState(newState);

    for (auto it = systems.rbegin(); it != systems.rend(); ++it)
    {
        const std::unique_ptr<BaseEditorSystem>& editorSystem = *it;
        if (editorSystem->CanProcessInput(currentInput))
        {
            editorSystem->ProcessInput(currentInput);
        }
    }
}

void EditorSystemsManager::HighlightNode(ControlNode* node)
{
    highlightNode.Emit(node);
}

void EditorSystemsManager::ClearHighlight()
{
    highlightNode.Emit(nullptr);
}

void EditorSystemsManager::SetEmulationMode(bool emulationMode)
{
    SetDisplayState(emulationMode ? Emulation : previousDisplayState);
}

ControlNode* EditorSystemsManager::GetControlNodeAtPoint(const DAVA::Vector2& point) const
{
    if (!KeyboardProxy::IsKeyPressed(KeyboardProxy::KEY_ALT))
    {
        return selectionSystemPtr->GetCommonNodeUnderPoint(point);
    }
    return selectionSystemPtr->GetNearestNodeUnderPoint(point);
}

uint32 EditorSystemsManager::GetIndexOfNearestRootControl(const DAVA::Vector2& point) const
{
    if (editingRootControls.empty())
    {
        return 0;
    }
    uint32 index = controlViewPtr->GetIndexByPos(point);
    bool insertToEnd = (index == editingRootControls.size());

    auto iter = editingRootControls.begin();
    std::advance(iter, insertToEnd ? index - 1 : index);
    PackageBaseNode* target = *iter;
    PackageControlsNode* controlsNode = package->GetPackageControlsNode();
    for (uint32 i = 0, count = controlsNode->GetCount(); i < count; ++i)
    {
        if (controlsNode->Get(i) == target)
        {
            return insertToEnd ? i + 1 : i;
        }
    }

    return controlsNode->GetCount();
}

void EditorSystemsManager::SelectAll()
{
    selectionSystemPtr->SelectAllControls();
}

void EditorSystemsManager::FocusNextChild()
{
    selectionSystemPtr->FocusNextChild();
}

void EditorSystemsManager::FocusPreviousChild()
{
    selectionSystemPtr->FocusPreviousChild();
}

void EditorSystemsManager::ClearSelection()
{
    selectionSystemPtr->ClearSelection();
}

void EditorSystemsManager::SelectNode(ControlNode* node)
{
    selectionSystemPtr->SelectNode(node);
}

void EditorSystemsManager::SetDisplayState(eDisplayState newDisplayState)
{
    if (displayState == newDisplayState)
    {
        return;
    }

    if (displayState == Emulation)
    {
        // go to previous state when emulation flag will be cleared
        previousDisplayState = newDisplayState;
    }
    else
    {
        previousDisplayState = displayState;
        displayState = newDisplayState;
        displayStateChanged.Emit(displayState, previousDisplayState);
    }
}

void EditorSystemsManager::OnSelectionChanged(const SelectedNodes& selected, const SelectedNodes& deselected)
{
    SelectionContainer::MergeSelectionToContainer(selected, deselected, selectedControlNodes);
    if (!selectedControlNodes.empty())
    {
        RefreshRootControls();
    }
}

void EditorSystemsManager::OnEditingRootControlsChanged(const SortedPackageBaseNodeSet& rootControls)
{
    SetDisplayState(rootControls.size() <= 1 ? Display : Preview);
}

void EditorSystemsManager::OnActiveHUDAreaChanged(const HUDAreaInfo& areaInfo)
{
    currentHUDArea = areaInfo;
}

void EditorSystemsManager::OnPackageChanged(PackageNode* package_)
{
    if (nullptr != package)
    {
        package->RemoveListener(this);
    }
    package = package_;
    RefreshRootControls();
    if (nullptr != package)
    {
        package->AddListener(this);
    }
}

void EditorSystemsManager::ControlWasRemoved(ControlNode* node, ControlsContainerNode* /*from*/)
{
    if (std::find(editingRootControls.begin(), editingRootControls.end(), node) != editingRootControls.end())
    {
        editingRootControls.erase(node);
        editingRootControlsChanged.Emit(editingRootControls);
    }
}

void EditorSystemsManager::ControlWasAdded(ControlNode* node, ControlsContainerNode* destination, int)
{
    if (displayState == Preview || displayState == Emulation)
    {
        DVASSERT(nullptr != package);
        if (nullptr != package)
        {
            PackageControlsNode* packageControlsNode = package->GetPackageControlsNode();
            if (destination == packageControlsNode)
            {
                editingRootControls.insert(node);
                editingRootControlsChanged.Emit(editingRootControls);
            }
        }
    }
}

void EditorSystemsManager::RefreshRootControls()
{
    SortedPackageBaseNodeSet newRootControls(CompareByLCA);

    if (nullptr == package)
    {
        return;
    }
    if (selectedControlNodes.empty())
    {
        PackageControlsNode* controlsNode = package->GetPackageControlsNode();
        for (int index = 0; index < controlsNode->GetCount(); ++index)
        {
            newRootControls.insert(controlsNode->Get(index));
        }
    }
    else
    {
        for (ControlNode* selectedControlNode : selectedControlNodes)
        {
            PackageBaseNode* root = static_cast<PackageBaseNode*>(selectedControlNode);
            while (nullptr != root->GetParent() && nullptr != root->GetParent()->GetControl())
            {
                root = root->GetParent();
            }
            if (nullptr != root)
            {
                newRootControls.insert(root);
            }
        }
    }
    if (editingRootControls != newRootControls)
    {
        editingRootControls = newRootControls;
        editingRootControlsChanged.Emit(editingRootControls);
        UIControlSystem::Instance()->GetInputSystem()->SetCurrentScreen(UIControlSystem::Instance()->GetScreen()); // reset current screen
    }
}

void EditorSystemsManager::InitDAVAScreen()
{
    RefPtr<UIControl> backgroundControl(new UIControl());

    backgroundControl->SetName(FastName("Background control of scroll area controller"));
    ScopedPtr<UIScreen> davaUIScreen(new UIScreen());
    davaUIScreen->GetBackground()->SetDrawType(UIControlBackground::DRAW_FILL);
    davaUIScreen->GetBackground()->SetColor(Color(0.3f, 0.3f, 0.3f, 1.0f));
    UIScreenManager::Instance()->RegisterScreen(0, davaUIScreen);
    UIScreenManager::Instance()->SetFirst(0);

    UIScreenManager::Instance()->GetScreen()->AddControl(backgroundControl.Get());
    backgroundControl->AddControl(rootControl.Get());
}

void EditorSystemsManager::OnDragStateChanged(eDragState currentState, eDragState previousState)
{
    if (currentState == Transform || previousState == Transform)
    {
        DVASSERT(package != nullptr);
        //calling this function can refresh all properties and styles in this node
        package->SetCanUpdateAll(previousState == Transform);
    }
}

void EditorSystemsManager::OnDisplayStateChanged(eDisplayState currentState, eDisplayState previousState)
{
    DVASSERT(currentState != previousState);
    if (currentState == Preview || previousState == Preview
        || currentState == Emulation || previousState == Emulation)
    {
        RefreshRootControls();
    }
    if (currentState == Emulation)
    {
        rootControl->RemoveControl(inputLayerControl.Get());
    }
    if (previousState == Emulation)
    {
        rootControl->AddControl(inputLayerControl.Get());
    }
}

UIControl* EditorSystemsManager::GetRootControl() const
{
    return rootControl.Get();
}

Vector2 EditorSystemsManager::GetMouseDelta() const
{
    return mouseDelta;
}

DAVA::Vector2 EditorSystemsManager::GetLastMousePos() const
{
    return lastMousePos;
}

EditorSystemsManager::eDragState EditorSystemsManager::GetDragState() const
{
    return dragState;
}

EditorSystemsManager::eDisplayState EditorSystemsManager::GetDisplayState() const
{
    return displayState;
}

HUDAreaInfo EditorSystemsManager::GetCurrentHUDArea() const
{
    return currentHUDArea;
}

EditorCanvas* EditorSystemsManager::GetEditorCanvas() const
{
    return editorCanvasPtr;
}

void EditorSystemsManager::SetDragState(eDragState newDragState)
{
    if (dragState == newDragState)
    {
        return;
    }
    previousDragState = dragState;
    dragState = newDragState;
    dragStateChanged.Emit(dragState, previousDragState);
}
