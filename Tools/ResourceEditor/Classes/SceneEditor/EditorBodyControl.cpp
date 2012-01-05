#include "EditorBodyControl.h"

#include "ControlsFactory.h"

#include "OutputPanelControl.h"

#include "../BeastProxy.h"

#include "LandscapePropertyControl.h"
#include "LightPropertyControl.h"
#include "CameraPropertyControl.h"
#include "SpherePropertyControl.h"
#include "BoxPropertyControl.h"
#include "ServicenodePropertyControl.h"


EditorBodyControl::EditorBodyControl(const Rect & rect)
    :   UIControl(rect)
	, beastManager(0)
{
    scene = NULL;
    
    selectedNode = NULL;
    savedTreeCell = 0;
    
    ControlsFactory::CusomizeBottomLevelControl(this);


    CreateLeftPanel();
    

    scene3dView = new UI3DView(Rect(
                            ControlsFactory::LEFT_SIDE_WIDTH + SCENE_OFFSET, 
                            SCENE_OFFSET, 
                            rect.dx - ControlsFactory::LEFT_SIDE_WIDTH - ControlsFactory::RIGHT_SIDE_WIDTH - 2 * SCENE_OFFSET, 
                            rect.dy - 2 * SCENE_OFFSET - OUTPUT_PANEL_HEIGHT));
    scene3dView->SetDebugDraw(true);
    scene3dView->SetInputEnabled(false);
    AddControl(scene3dView);
    
    
    
    CreateScene(true);

    outputPanel = new OutputPanelControl(scene, Rect(
                                              ControlsFactory::LEFT_SIDE_WIDTH, 
                                              rect.dy - OUTPUT_PANEL_HEIGHT, 
                                              rect.dx - ControlsFactory::LEFT_SIDE_WIDTH - ControlsFactory::RIGHT_SIDE_WIDTH, 
                                              OUTPUT_PANEL_HEIGHT));
    ControlsFactory::CustomizePanelControl(outputPanel);
    AddControl(outputPanel);

    
    
    CreatePropertyPanel();
	
	CreateModificationPanel();
}


EditorBodyControl::~EditorBodyControl()
{
    ReleaseModificationPanel();
    
    ReleasePropertyPanel();

    SafeRelease(outputPanel);
    
    ReleaseScene();
  
    SafeRelease(scene3dView);

    ReleaseLeftPanel();
}

void EditorBodyControl::CreateLeftPanel()
{
    Rect fullRect = GetRect();
    
    Rect leftRect = Rect(0, 0, ControlsFactory::LEFT_SIDE_WIDTH, fullRect.dy);
    leftPanel = ControlsFactory::CreatePanelControl(leftRect);
    AddControl(leftPanel);

    Rect treeRect = leftRect;
    leftRect.dy -= (ControlsFactory::BUTTON_HEIGHT * 3);
    sceneTree = new UIHierarchy(treeRect);
    ControlsFactory::CusomizeListControl(sceneTree);
    ControlsFactory::SetScrollbar(sceneTree);
    sceneTree->SetCellHeight(CELL_HEIGHT);
    sceneTree->SetDelegate(this);
    sceneTree->SetClipContents(true);
    leftPanel->AddControl(sceneTree);
    
    int32 y = leftRect.dy;
    lookAtButton = ControlsFactory::CreateButton(Rect(
                                        0, y, ControlsFactory::LEFT_SIDE_WIDTH,ControlsFactory::BUTTON_HEIGHT), 
                                        L"Look At Object");
    y += ControlsFactory::BUTTON_HEIGHT;
    removeNodeButton = ControlsFactory::CreateButton(Rect(
                                        0, y, ControlsFactory::LEFT_SIDE_WIDTH, ControlsFactory::BUTTON_HEIGHT), 
                                        L"Remove Object");
    y += ControlsFactory::BUTTON_HEIGHT;
    enableDebugFlagsButton = ControlsFactory::CreateButton(Rect(
                                        0, y, ControlsFactory::LEFT_SIDE_WIDTH, ControlsFactory::BUTTON_HEIGHT), 
                                        L"Debug Flags");
    
    lookAtButton->AddEvent(UIControl::EVENT_TOUCH_UP_INSIDE, Message(this, &EditorBodyControl::OnLookAtButtonPressed));
    removeNodeButton->AddEvent(UIControl::EVENT_TOUCH_UP_INSIDE, Message(this, &EditorBodyControl::OnRemoveNodeButtonPressed));
    enableDebugFlagsButton->AddEvent(UIControl::EVENT_TOUCH_UP_INSIDE, Message(this, &EditorBodyControl::OnEnableDebugFlagsPressed));

    
    leftPanel->AddControl(lookAtButton);
    leftPanel->AddControl(removeNodeButton);
    leftPanel->AddControl(enableDebugFlagsButton);
}

void EditorBodyControl::ReleaseLeftPanel()
{
    SafeRelease(sceneTree);
    SafeRelease(leftPanel);
    
    SafeRelease(lookAtButton);
    SafeRelease(removeNodeButton);
    SafeRelease(enableDebugFlagsButton);
}


void EditorBodyControl::CreateScene(bool withCameras)
{
    scene = new EditorScene();
    // Camera setup
    cameraController = new WASDCameraController(40);
    
    if(withCameras)
    {
        Camera * cam = new Camera(scene);
        cam->SetName("editor-camera");
        cam->SetDebugFlags(SceneNode::DEBUG_DRAW_ALL);
        cam->SetUp(Vector3(0.0f, 0.0f, 1.0f));
        cam->SetPosition(Vector3(0.0f, 0.0f, 0.0f));
        cam->SetTarget(Vector3(0.0f, 1.0f, 0.0f));
        
        cam->Setup(70.0f, 320.0f / 480.0f, 1.0f, 5000.0f); 
        
        scene->AddNode(cam);
        scene->AddCamera(cam);
        scene->SetCurrentCamera(cam);
        cameraController->SetCamera(cam);
        
        SafeRelease(cam);
        
        Camera * cam2 = new Camera(scene);
        cam2->SetName("editor-top-camera");
        cam2->SetDebugFlags(SceneNode::DEBUG_DRAW_ALL);
        cam2->SetUp(Vector3(1.0f, 0.0f, 0.0f));
        cam2->SetPosition(Vector3(0.0f, 0.0f, 200.0f));
        cam2->SetTarget(Vector3(0.0f, 250.0f, 0.0f));
        
        cam2->Setup(70.0f, 320.0f / 480.0f, 1.0f, 5000.0f); 
        
        scene->AddNode(cam2);
        scene->AddCamera(cam2);
        
        SafeRelease(cam2);
    }
    
    scene3dView->SetScene(scene);
}

void EditorBodyControl::ReleaseScene()
{
    //TODO: need to release root nodes?
    
    SafeRelease(scene);
    SafeRelease(cameraController);
}


static const wchar_t * mods[3] = { L"M", L"R", L"S"};
static const wchar_t * axises[3] = { L"X", L"Y", L"Z"};

#define BUTTON_W 20 
#define BUTTON_B 5 

void EditorBodyControl::CreateModificationPanel(void)
{
	modState = MOD_MOVE;
	modAxis = AXIS_X;
	
	modificationPanel = ControlsFactory::CreatePanelControl(Rect(scene3dView->GetRect(true).x, 5, 120, 45));
    modificationPanel->GetBackground()->SetColor(Color(1.0, 1.0, 1.0, 0.2));

	for (int i = 0; i < 3; i++)
	{
		btnMod[i] = ControlsFactory::CreateButton(Rect((BUTTON_W + BUTTON_B) * i, 0, BUTTON_W, BUTTON_W), mods[i]);
		btnMod[i]->AddEvent(UIControl::EVENT_TOUCH_UP_INSIDE, Message(this, &EditorBodyControl::OnModificationPressed));
		modificationPanel->AddControl(btnMod[i]);

		btnAxis[i] = ControlsFactory::CreateButton(Rect((BUTTON_W + BUTTON_B) * i, BUTTON_W + BUTTON_B, BUTTON_W, BUTTON_W), axises[i]);
		btnAxis[i]->AddEvent(UIControl::EVENT_TOUCH_UP_INSIDE, Message(this, &EditorBodyControl::OnModificationPressed));
		modificationPanel->AddControl(btnAxis[i]);
	}
	UIStaticText * st = new UIStaticText(Rect(55, 0, 80, BUTTON_W));
    st->SetFont(ControlsFactory::GetFontLight());
	st->SetText(L"w, e, r");
    modificationPanel->AddControl(st);

	st = new UIStaticText(Rect(55, BUTTON_W + BUTTON_B, 80, BUTTON_W));
    st->SetFont(ControlsFactory::GetFontLight());
	st->SetText(L"5, 6, 7, 8");
    modificationPanel->AddControl(st);
	
	
	UpdateModState();
}

void EditorBodyControl::ReleaseModificationPanel()
{
	for (int i = 0; i < 3; i++)
	{
		SafeRelease(btnMod[i]);
		SafeRelease(btnAxis[i]);
	}
	SafeRelease(modificationPanel);
}

void EditorBodyControl::OnModificationPressed(BaseObject * object, void * userData, void * callerData)
{
	for (int i = 0; i < 3; i++)
	{
		if (object == btnMod[i])
		{
			modState = (eModState)i;
		}
		if (object == btnAxis[i])
		{
			modAxis = (eModAxis)i;
		}
	}
	UpdateModState();
}


void EditorBodyControl::UpdateModState(void)
{
	for (int i = 0; i < 3; i++)
	{
		btnMod[i]->SetState(UIControl::STATE_NORMAL);
		btnAxis[i]->SetState(UIControl::STATE_NORMAL);
	}
	btnMod[modState]->SetState(UIControl::STATE_SELECTED);

	switch (modAxis) 
	{
	case AXIS_X:
	case AXIS_Y:
	case AXIS_Z:
		btnAxis[modAxis]->SetState(UIControl::STATE_SELECTED);
		break;
	case AXIS_XY:
		btnAxis[AXIS_X]->SetState(UIControl::STATE_SELECTED);
		btnAxis[AXIS_Y]->SetState(UIControl::STATE_SELECTED);
		break;
	case AXIS_YZ:
		btnAxis[AXIS_Y]->SetState(UIControl::STATE_SELECTED);
		btnAxis[AXIS_Z]->SetState(UIControl::STATE_SELECTED);
		break;
	case AXIS_XZ:
		btnAxis[AXIS_X]->SetState(UIControl::STATE_SELECTED);
		btnAxis[AXIS_Z]->SetState(UIControl::STATE_SELECTED);
		break;
	default:
		break;
	}
}


void EditorBodyControl::CreatePropertyPanel()
{
    Rect fullRect = GetRect();
    Rect propertyPanelRect = Rect(fullRect.dx - ControlsFactory::RIGHT_SIDE_WIDTH, 0, ControlsFactory::RIGHT_SIDE_WIDTH, size.y);
    rightPanel = ControlsFactory::CreatePanelControl(propertyPanelRect);
    AddControl(rightPanel);

    refreshButton = ControlsFactory::CreateButton(Rect(
                                            0, propertyPanelRect.dy - ControlsFactory::BUTTON_HEIGHT, 
                                            propertyPanelRect.dx, ControlsFactory::BUTTON_HEIGHT), 
                                            L"Refresh");
    refreshButton->AddEvent(UIControl::EVENT_TOUCH_UP_INSIDE, Message(this, &EditorBodyControl::OnRefreshPressed));
    
    rightPanel->AddControl(refreshButton);
    
    
    propertyPanelRect.x = propertyPanelRect.y = 0;
    propertyPanelRect.dy -= ControlsFactory::BUTTON_HEIGHT;
    nodePropertyPanel[ECNID_LANDSCAPE] = new LandscapePropertyControl(propertyPanelRect, true);
    KeyedArchive *keyedArchieve = new KeyedArchive();
    keyedArchieve->Load("~doc:/ResourceEditorOptions.archive");
    String path = keyedArchieve->GetString("LastSavedPath", "/");
    ((LandscapePropertyControl *)nodePropertyPanel[ECNID_LANDSCAPE])->SetProjectPath(path);
    SafeRelease(keyedArchieve);
    
    
    nodePropertyPanel[ECNID_LIGHT] = new LightPropertyControl(propertyPanelRect, true);
    nodePropertyPanel[ECNID_SERVICENODE] = new ServicenodePropertyControl(propertyPanelRect, true);
    nodePropertyPanel[ECNID_BOX] = new BoxPropertyControl(propertyPanelRect, true);
    nodePropertyPanel[ECNID_SPHERE] = new SpherePropertyControl(propertyPanelRect, true);
    nodePropertyPanel[ECNID_CAMERA] = new CameraPropertyControl(propertyPanelRect, true);
    nodePropertyPanel[ECNID_COUNT] = new NodePropertyControl(propertyPanelRect, true);

    for(int32 i = 0; i <= ECNID_COUNT; ++i)
    {
        nodePropertyPanel[i]->InitProperties();
        nodePropertyPanel[i]->SetDelegate(this);
    }
    currentPropertyPanel = NULL;
    
    
//    localMatrixControl = new EditMatrixControl(Rect(0, 0, RIGHT_SIDE_WIDTH, MATRIX_HEIGHT));
//    localMatrixControl->OnMatrixChanged = Message(this, &EditorBodyControl::OnLocalTransformChanged);
//    
//    worldMatrixControl = new EditMatrixControl(Rect(0, 0, RIGHT_SIDE_WIDTH, MATRIX_HEIGHT), true);
//
//    
//    Rect fullRect = GetRect();
//    activePropertyPanel = new PropertyPanel(Rect(fullRect.dx - RIGHT_SIDE_WIDTH, 0, RIGHT_SIDE_WIDTH, size.y));
//    
//    nodeName = SafeRetain(activePropertyPanel->AddHeader(L"Node name:"));
//    
//    activePropertyPanel->AddHeader(L"Local Matrix:");
//    activePropertyPanel->AddPropertyControl(localMatrixControl);
//    activePropertyPanel->AddHeader(L"World Matrix:");
//    activePropertyPanel->AddPropertyControl(worldMatrixControl);
//    nodeBoundingBoxMin = SafeRetain(activePropertyPanel->AddHeader(L"-"));
//    nodeBoundingBoxMax = SafeRetain(activePropertyPanel->AddHeader(L"-"));
//    
//    AddControl(activePropertyPanel);
}

void EditorBodyControl::ReleasePropertyPanel()
{
    SafeRelease(refreshButton);
    
    currentPropertyPanel = NULL;
    for(int32 i = 0; i <= ECNID_COUNT; ++i)
    {
        SafeRelease(nodePropertyPanel[i]);
    }
    SafeRelease(rightPanel);
    
//    SafeRelease(nodeName);
//    SafeRelease(nodeBoundingBoxMin);
//    SafeRelease(nodeBoundingBoxMax);
    
//    SafeRelease(activePropertyPanel);
}

bool EditorBodyControl::IsNodeExpandable(UIHierarchy *forHierarchy, void *forNode)
{
    if(forHierarchy == sceneTree)
    {
        if (forNode) 
        {
            return ((SceneNode*)forNode)->GetChildrenCount() > 0;
        }
        
        return scene->GetChildrenCount() > 0;
    }
    
    return false;
}

int32 EditorBodyControl::ChildrenCount(UIHierarchy *forHierarchy, void *forParent)
{
    if(forHierarchy == sceneTree)
    {
        if (forParent) 
        {
            return ((SceneNode*)forParent)->GetChildrenCount();
        }
        
        return scene->GetChildrenCount();
    }

    return 0;
}

void * EditorBodyControl::ChildAtIndex(UIHierarchy *forHierarchy, void *forParent, int32 index)
{
    if(forHierarchy == sceneTree)
    {
        if (forParent) 
        {
            return ((SceneNode*)forParent)->GetChild(index);
        }
        
        return scene->GetChild(index);
    }
    
    return NULL;
}

UIHierarchyCell * EditorBodyControl::CellForNode(UIHierarchy *forHierarchy, void *node)
{
    UIHierarchyCell *c = NULL;
    if(forHierarchy == sceneTree)
    {
        c = forHierarchy->GetReusableCell("SceneGraph cell"); //try to get cell from the reusable cells store
        if(!c)
        { //if cell of requested type isn't find in the store create new cell
            c = new UIHierarchyCell(Rect(0, 0, ControlsFactory::LEFT_SIDE_WIDTH, CELL_HEIGHT), "SceneGraph cell");
        }
        
        //fill cell whith data
        SceneNode *n = (SceneNode *)node;
        
        c->text->SetText(StringToWString(n->GetName()));
    }

    c->SetSelected(false, false);
    
    ControlsFactory::CustomizeExpandButton(c->openButton);
    ControlsFactory::CustomizeSceneGraphCell(c);
    
    return c;//returns cell
}

void EditorBodyControl::OnCellSelected(UIHierarchy *forHierarchy, UIHierarchyCell *selectedCell)
{
    if(forHierarchy == sceneTree)
    {
        savedTreeCell = selectedCell;
        
        UIHierarchyNode * hNode = selectedCell->GetNode();
        SceneNode * node = dynamic_cast<SceneNode*>((BaseObject*)hNode->GetUserNode());
        if (node)
        {
//            if(selectedNode)
//            {
//                selectedNode->SetDebugFlags(SceneNode::DEBUG_DRAW_NONE);
//            }
            selectedNode = node;
            
//            if(selectedNode)
//            {
//                selectedNode->SetDebugFlags(SceneNode::DEBUG_DRAW_AABBOX | SceneNode::DEBUG_DRAW_LOCAL_AXIS);
//            }
            
            UpdatePropertyPanel();
        }
        
        List<UIControl*> children = sceneTree->GetVisibleCells();
        for(List<UIControl*>::iterator it = children.begin(); it != children.end(); ++it)
        {
            UIControl *ctrl = (*it);
            ctrl->SetSelected(false, false);
        }
        
        selectedCell->SetSelected(true, false);
    }
}

void EditorBodyControl::UpdatePropertyPanel()
{
    if(currentPropertyPanel)
    {
        rightPanel->RemoveControl(currentPropertyPanel);   
    }

    if(!selectedNode)
    {
        currentPropertyPanel = NULL;
        return;
    }
    
    
    currentPropertyPanel = nodePropertyPanel[ECNID_COUNT]; // for not categorized nodes
    
    Camera * camera = dynamic_cast<Camera*> (selectedNode);
    if (camera)
    {
        scene->SetCurrentCamera(camera);
        Camera * cam2 = scene->GetCamera(0);
        scene->SetClipCamera(cam2);
        //cameraController->SetCamera(camera);
        
        outputPanel->UpdateCamera();
        currentPropertyPanel = nodePropertyPanel[ECNID_CAMERA];
    }
    
    LightNode * light = dynamic_cast<LightNode*> (selectedNode);
    if (light)
    {
        currentPropertyPanel = nodePropertyPanel[ECNID_LIGHT];
    }
    
//    ServiceNode * serviceNode = dynamic_cast<ServiceNode*> (node);
//    if (serviceNode)
//    {
//        currentPropertyPanel = nodePropertyPanel[ECNID_SERVICENODE];
//    }
    
    CubeNode * box = dynamic_cast<CubeNode*> (selectedNode);
    if (box)
    {
        currentPropertyPanel = nodePropertyPanel[ECNID_BOX];
    }
    
    SphereNode * sphere = dynamic_cast<SphereNode*> (selectedNode);
    if (sphere)
    {
        currentPropertyPanel = nodePropertyPanel[ECNID_SPHERE];
    }
    
    LandscapeNode * landscape = dynamic_cast<LandscapeNode*> (selectedNode);
    if (landscape)
    {
        currentPropertyPanel = nodePropertyPanel[ECNID_LANDSCAPE];
    }
    
//    MeshInstanceNode * mesh = dynamic_cast<MeshInstanceNode*>(selectedNode);
//    if(mesh)
//    {
//        currentPropertyPanel = nodePropertyPanel[ECNID_COUNT];
//    }

    currentPropertyPanel->ReadFromNode(selectedNode);
    rightPanel->AddControl(currentPropertyPanel);
}

void EditorBodyControl::Input(DAVA::UIEvent *event)
{    
    if (event->phase == UIEvent::PHASE_KEYCHAR)
    {
        if (event->keyChar == '1')
            cameraController->SetSpeed(40);
        if (event->keyChar == '2')
            cameraController->SetSpeed(80);
        if (event->keyChar == '3')
            cameraController->SetSpeed(160);
        if (event->keyChar == '4')
            cameraController->SetSpeed(320);
        
        Camera * newCamera = 0;
        if (event->keyChar == 'z')newCamera = scene->GetCamera(0);
        if (event->keyChar == 'x')newCamera = scene->GetCamera(1);
        if (event->keyChar == 'c')newCamera = scene->GetCamera(2);
        if (event->keyChar == 'v')newCamera = scene->GetCamera(3);
        if (event->keyChar == 'b')newCamera = scene->GetCamera(4);
        if (newCamera)
        {
            scene->SetCurrentCamera(newCamera);
            scene->SetClipCamera(scene->GetCamera(0));
        }
		
        if (event->keyChar == 'w') modState = MOD_MOVE;
		if (event->keyChar == 'e') modState = MOD_ROTATE;
		if (event->keyChar == 'r') modState = MOD_SCALE;
        if (event->keyChar == '5') modAxis = AXIS_X;
		if (event->keyChar == '6') modAxis = AXIS_Y;
		if (event->keyChar == '7') modAxis = AXIS_Z;
		if (event->keyChar == '8') 
		{
			if (modAxis < AXIS_XY)
				modAxis = AXIS_XY;
			else
				modAxis = (eModAxis)(AXIS_XY + ((modAxis + 1 - AXIS_XY) % 3));
		}
		UpdateModState();
	}   
	
	//selection with second mouse button 
	if (event->phase == UIEvent::PHASE_BEGAN && event->tid == UIEvent::BUTTON_2)
	{
		Camera * cam = scene->GetCurrentCamera();
		const Rect & rect = scene3dView->GetLastViewportRect();
		Vector3 from = cam->GetPosition();
		Vector3 to = cam->UnProject(event->point.x, event->point.y, 0, rect);
		to -= from;
		to *= 1000.f;
		to += from;
		scene->TrySelection(from, to);
	}	
	
    SceneNode * selection = scene->GetSelection();
	if (selection != 0)
	{
		if (event->phase == UIEvent::PHASE_BEGAN)
		{
			if (event->tid == UIEvent::BUTTON_1)
			{
				inTouch = true;	
				touchStart = event->point;
				
				startTransform = selection->GetLocalTransform();
			}
		}	
		if (event->phase == UIEvent::PHASE_DRAG)
		{
			if (event->tid == UIEvent::BUTTON_1)
			{
				PrepareModMatrix(event->point.x - touchStart.x, event->point.y - touchStart.y);
				selection->SetLocalTransform(currTransform);
			}
		}
		if (event->phase == UIEvent::PHASE_ENDED)
		{
			if (event->tid == UIEvent::BUTTON_1)
			{
				inTouch = false;
			}
		}
	}
	else
	{
		cameraController->Input(event);
	}
	UIControl::Input(event);
}

static Vector3 vect[3] = {Vector3(1, 0, 0), Vector3(0, 1, 0), Vector3(0, 0, 1)};

void EditorBodyControl::PrepareModMatrix(float32 winx, float32 winy)
{	
	Matrix4 modification;
	modification.Identity();
	
	if (modState == MOD_MOVE)
	{
		switch (modAxis) 
		{
			case AXIS_X:
			case AXIS_Y:
				modification.CreateTranslation(vect[modAxis] * winx * axisSign[modAxis]);
				break;
			case AXIS_Z:
				modification.CreateTranslation(vect[modAxis] * winy * axisSign[AXIS_Z]);
				break;
			case AXIS_XY:
				modification.CreateTranslation(vect[AXIS_X] * winx * axisSign[AXIS_X] + vect[AXIS_Y] * winy * axisSign[AXIS_Y]);
				break;
			case AXIS_YZ:
				modification.CreateTranslation(vect[AXIS_Y] * winx * axisSign[AXIS_Y] + vect[AXIS_Z] * winy * axisSign[AXIS_Z]);
				break;
			case AXIS_XZ:
				modification.CreateTranslation(vect[AXIS_X] * winx * axisSign[AXIS_X] + vect[AXIS_Z] * winy * axisSign[AXIS_Z]);
				break;
			default:
				break;
		}
	}
	else if (modState == MOD_ROTATE)
	{
		Matrix4 d;
		Matrix4 translate1, translate2;
		
		translate1.CreateTranslation(-startTransform.GetTranslationVector());
		translate2.CreateTranslation(startTransform.GetTranslationVector());
		
		switch (modAxis) 
		{
			case AXIS_X:
			case AXIS_Y:
				modification.CreateRotation(vect[modAxis], winy / 100.0f);
				break;
			case AXIS_Z:
				modification.CreateRotation(vect[modAxis], winx / 100.0f);
				break;
			case AXIS_XY:
				modification.CreateRotation(vect[AXIS_X], winx / 100.0f);
				d.CreateRotation(vect[AXIS_Y], winy / 100.0f);
				modification *= d;
				break;
			case AXIS_YZ:
				modification.CreateRotation(vect[AXIS_Y], winx / 100.0f);
				d.CreateRotation(vect[AXIS_Z], winy / 100.0f);
				modification *= d;
				break;
			case AXIS_XZ:
				modification.CreateRotation(vect[AXIS_X], winx / 100.0f);
				d.CreateRotation(vect[AXIS_Z], winy / 100.0f);
				modification *= d;
				break;
			default:
				break;
		}
		modification = (translate1 * modification) * translate2;
	}
	else if (modState == MOD_SCALE)
	{
//		modification.CreateScale(Vector3(1,1,1) + vect[modAxis] * dist/100);
		modification.CreateScale(Vector3(1,1,1) + Vector3(1,1,1) * (winx/100.0f));
	}
	currTransform = startTransform * modification;
}


void EditorBodyControl::DrawAfterChilds(const UIGeometricData &geometricData)
{
	UIControl::DrawAfterChilds(geometricData);
	SceneNode * selection = scene->GetSelection();
	if (selection)
	{
//		RenderHelper::SetClip();
		
		const Rect & rect = scene3dView->GetLastViewportRect();
		Matrix4 wt = selection->GetWorldTransform();
		Vector3 offs = wt.GetTranslationVector();
		Camera * cam = scene->GetCurrentCamera(); 
		Vector2 start = cam->GetOnScreenPosition(offs, rect);
		Vector2 end;
	
		
		for(int i = 0; i < 3; i++)
		{
			if (modAxis == i
				|| (i == AXIS_X && (modAxis == AXIS_XY || modAxis == AXIS_XZ)) 
				|| (i == AXIS_Y && (modAxis == AXIS_XY || modAxis == AXIS_YZ)) 
				|| (i == AXIS_Z && (modAxis == AXIS_XZ || modAxis == AXIS_YZ)))
			{
				RenderManager::Instance()->SetColor(0, 1.0f, 0, 1.0f);					
			}
			else 
			{
				RenderManager::Instance()->SetColor(1.0f, 0, 0, 1.0f);	
			}

			Vector3 v = offs + vect[i] * 5.0;
			end = cam->GetOnScreenPosition(v, rect);
			RenderHelper::Instance()->DrawLine(start, end);

		
			if (i == AXIS_X 
				|| (i == AXIS_Y && modAxis == AXIS_Y)
				|| (i == AXIS_Y && modAxis == AXIS_YZ)
				)
			{
				axisSign[i] = (start.x > end.x) ? -1.0f: 1.0f;
			}
			else if (i == AXIS_Y && modAxis == AXIS_XY)
			{
				axisSign[i] = (start.y > end.y) ? -1.0f: 1.0f;				
			}
			else if (i == AXIS_Z)
			{
				axisSign[i] = (start.y > end.y) ? -1.0f: 1.0f;
			}
		}
		RenderManager::Instance()->ResetColor();
	}
}

void EditorBodyControl::Update(float32 timeElapsed)
{
	SceneNode * selection = scene->GetSelection();
	if (selection && modificationPanel->GetParent() == 0)
	{
		AddControl(modificationPanel);
	}
	else if (selection == 0 && modificationPanel->GetParent() != 0)
	{
		RemoveControl(modificationPanel);
	}
	
    UIControl::Update(timeElapsed);
}

//void EditorBodyControl::OnLocalTransformChanged(BaseObject * object, void * userData, void * callerData)
//{
//    if (selectedNode)
//    {
//        selectedNode->SetLocalTransform(localMatrixControl->GetMatrix());
//    }
//}

void EditorBodyControl::OnLookAtButtonPressed(BaseObject * obj, void *, void *)
{
    MeshInstanceNode * mesh = dynamic_cast<MeshInstanceNode*>(selectedNode);
    if (mesh)
    {
        AABBox3 bbox = mesh->GetBoundingBox();
        AABBox3 transformedBox;
        bbox.GetTransformedBox(mesh->GetWorldTransform(), transformedBox);
        Vector3 center = transformedBox.GetCenter();
        scene->GetCurrentCamera()->SetTarget(center);
    }
}

void EditorBodyControl::OnRemoveNodeButtonPressed(BaseObject * obj, void *, void *)
{
    if (selectedNode)
    {
        SceneNode * parentNode = selectedNode->GetParent();
        if (parentNode)
        {
            parentNode->RemoveNode(selectedNode);
            
            selectedNode = NULL;
            savedTreeCell = NULL;
            UpdatePropertyPanel();

            sceneTree->Refresh();
        }
    }
}

void EditorBodyControl::OnEnableDebugFlagsPressed(BaseObject * obj, void *, void *)
{
    if (selectedNode)
    {
        if (selectedNode->GetDebugFlags() & SceneNode::DEBUG_DRAW_ALL)
        {
            selectedNode->SetDebugFlags(0, true);
        }else
        {
            selectedNode->SetDebugFlags(SceneNode::DEBUG_DRAW_ALL, true);
        }
    }
}

void EditorBodyControl::OpenScene(const String &pathToFile)
{
    SceneFile * file = new SceneFile();
    file->SetDebugLog(true);
    file->LoadScene(pathToFile.c_str(), scene);
    scene->AddNode(scene->GetRootNode(pathToFile));
    SafeRelease(file);
    
    if (scene->GetCamera(0))
    {
        scene->SetCurrentCamera(scene->GetCamera(0));
        cameraController->SetCamera(scene->GetCamera(0));
    }
    sceneTree->Refresh();
}

void EditorBodyControl::WillAppear()
{
    selectedNode = NULL;
    savedTreeCell = NULL;
    
    sceneTree->Refresh();
}

void EditorBodyControl::ShowProperties(bool show)
{
    if(show && !rightPanel->GetParent())
    {
        AddControl(rightPanel);
        
        ChangeControlWidthRight(scene3dView, -ControlsFactory::RIGHT_SIDE_WIDTH);
        ChangeControlWidthRight(outputPanel, -ControlsFactory::RIGHT_SIDE_WIDTH);
    }
    else if(!show && rightPanel->GetParent())
    {
        RemoveControl(rightPanel);
        
        ChangeControlWidthRight(scene3dView, ControlsFactory::RIGHT_SIDE_WIDTH);
        ChangeControlWidthRight(outputPanel, ControlsFactory::RIGHT_SIDE_WIDTH);
    }
}

bool EditorBodyControl::PropertiesAreShown()
{
    return (rightPanel->GetParent() != NULL);
}

void EditorBodyControl::ShowSceneGraph(bool show)
{
    if(show && !leftPanel->GetParent())
    {
        AddControl(leftPanel);

        ChangeControlWidthLeft(scene3dView, ControlsFactory::LEFT_SIDE_WIDTH);
        ChangeControlWidthLeft(outputPanel, ControlsFactory::LEFT_SIDE_WIDTH);
        
        sceneTree->Refresh();
    }
    else if(!show && leftPanel->GetParent())
    {
        RemoveControl(leftPanel);
        
        ChangeControlWidthLeft(scene3dView, -ControlsFactory::LEFT_SIDE_WIDTH);
        ChangeControlWidthLeft(outputPanel, -ControlsFactory::LEFT_SIDE_WIDTH);
    }
}

bool EditorBodyControl::SceneGraphAreShown()
{
    return (leftPanel->GetParent() != NULL);
}

void EditorBodyControl::UpdateLibraryState(bool isShown, int32 width)
{
    if(isShown)
    {
        ShowProperties(false);
        
        ChangeControlWidthRight(scene3dView, -width);
        ChangeControlWidthRight(outputPanel, -width);
    }
    else
    {
        ChangeControlWidthRight(scene3dView, ControlsFactory::RIGHT_SIDE_WIDTH);
        ChangeControlWidthRight(outputPanel, ControlsFactory::RIGHT_SIDE_WIDTH);
    }
}

void EditorBodyControl::BeastProcessScene()
{
	beastManager = BeastProxy::Instance()->CreateManager();

	BeastProxy::Instance()->ParseScene(beastManager, scene);
	BeastProxy::Instance()->CreateSkyLight(beastManager);
	BeastProxy::Instance()->SetCamera(beastManager, scene->GetCurrentCamera());
	BeastProxy::Instance()->WindowedRender(beastManager);
}

EditorScene * EditorBodyControl::GetScene()
{
    return scene;
}

void EditorBodyControl::AddNode(SceneNode *node)
{
    scene->AddNode(node);
    sceneTree->Refresh();
}

void EditorBodyControl::ChangeControlWidthRight(UIControl *c, float32 width)
{
    Rect r = c->GetRect();
    r.dx += width;
    c->SetRect(r);
}

void EditorBodyControl::ChangeControlWidthLeft(UIControl *c, float32 width)
{
    Rect r = c->GetRect();
    r.dx -= width;
    r.x += width;
    c->SetRect(r);
}

void EditorBodyControl::NodePropertyChanged()
{
    if(selectedNode && currentPropertyPanel)
    {
        currentPropertyPanel->ReadToNode(selectedNode);
        savedTreeCell->text->SetText(StringToWString(selectedNode->GetName()));
    }
}

void EditorBodyControl::OnRefreshPressed(BaseObject * obj, void *, void *)
{
    if(selectedNode && currentPropertyPanel)
    {
        currentPropertyPanel->ReadToNode(selectedNode);
        savedTreeCell->text->SetText(StringToWString(selectedNode->GetName()));
    }
}

void EditorBodyControl::Refresh()
{
    sceneTree->Refresh();
}
