/*==================================================================================
    Copyright (c) 2008, DAVA Consulting, LLC
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
    * Neither the name of the DAVA Consulting, LLC nor the
    names of its contributors may be used to endorse or promote products
    derived from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE DAVA CONSULTING, LLC AND CONTRIBUTORS "AS IS" AND
    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL DAVA CONSULTING, LLC BE LIABLE FOR ANY
    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

    Revision History:
        * Created by Vitaliy Borodovsky 
=====================================================================================*/
#include "AnimationTestScreen.h"
#include "TestScreen.h"
#include "TouchEffectsControl.h"
#include "Scene3D/RotatingCubeNode.h"

void AnimationTestScreen::LoadResources()
{
    //RenderManager::Instance()->EnableOutputDebugStatsEveryNFrame(30);
    RenderManager::Instance()->SetFPS(30.0);
	scene = new Scene();
	//RotatingCubeNode * cubeNode = new RotatingCubeNode(scene);
	//scene->AddNode(cubeNode);
    
	SceneFile * file = new SceneFile();
	file->SetDebugLog(true);
//	file->LoadScene("~res:/Scenes/garage/hungar.sce", scene);
//    scene->AddNode(scene->GetRootNode("~res:/Scenes/garage/hungar.sce"));
//	file->LoadScene("~res:/Scenes/garage_lit/hungar.sce", scene);
//  scene->AddNode(scene->GetRootNode("~res:/Scenes/garage_lit/hungar.sce"));
    file->LoadScene("~res:/Scenes/level_test/level_test_3.sce", scene);
    scene->AddNode(scene->GetRootNode("~res:/Scenes/level_test/level_test_3.sce"));
    
	SafeRelease(file);
        
	currentTankAngle = 0.0f;
	inTouch = false;
	startRotationInSec = 0.0f;
	rotationSpeed = 8.0f;   
    
    
    SceneNode * turretNode = scene->FindByName("node-lod0_turret_01");
    if (turretNode)
    {
        MeshInstanceNode * mesh = dynamic_cast<MeshInstanceNode*>(turretNode->FindByName("instance_0"));
        targetPosition = mesh->GetBoundingBox().GetCenter();
    }
    
	//originalCameraPosition = scene->GetCamera(0)->GetPosition();
//	positionJoypad = new UIJoypad(Rect(0, 320 - 80, 80, 80));
//	positionJoypad->GetBackground()->SetSprite("~res:/Gfx/Joypad/joypad", 0);
//	positionJoypad->SetStickSprite("~res:/Gfx/Joypad/joypad", 1);
//	
//	AddControl(positionJoypad);
    
	scene3dView = 0;
    scene3dView = new UI3DView(Rect(0, 0, 480, 320));
    scene3dView->SetInputEnabled(false);
    scene3dView->SetScene(scene);
    cam = scene->GetCamera(0);
    cam->SetFOV(75.0f);
    //cam->Setup(60.0f, float32 aspect, float32 znear, float32 zfar, bool ortho)

    scene->SetCamera(cam);
    AddControl(scene3dView);
    
//    hierarchy = new UIHierarchy(Rect(0, 0, size.x / 3, size.y));
//    hierarchy->SetCellHeight(25);
//    hierarchy->SetDelegate(this);
//    AddControl(hierarchy);
    
    viewXAngle = 0;
    viewYAngle = 0;
}

void AnimationTestScreen::UnloadResources()
{
    SafeRelease(scene3dView);
    SafeRelease(hierarchy);
	SafeRelease(scene);
}

void AnimationTestScreen::WillAppear()
{
}

void AnimationTestScreen::WillDisappear()
{
	
}

void AnimationTestScreen::Input(UIEvent * touch)
{
//	if (touch->phase == UIEvent::PHASE_BEGAN)
//	{
//		inTouch = true;	
//		touchStart = touch->point;
//		touchTankAngle = currentTankAngle;
//	}
//	
//	if (touch->phase == UIEvent::PHASE_DRAG)
//	{
//		touchCurrent = touch->point;
//		
//		float32 dist = (touchCurrent.x - touchStart.x);
//		//Logger::Debug("%f, %f", currentTankAngle, dist);
//		currentTankAngle = touchTankAngle + dist;
//	}
//	
//	if (touch->phase == UIEvent::PHASE_ENDED)
//	{
//		touchCurrent = touch->point;
//		rotationSpeed = (touchCurrent.x - touchStart.x);
//		inTouch = false;
//		startRotationInSec = 0.0f;
//	}
    
    if (touch->phase == UIEvent::PHASE_BEGAN) 
    {
        oldTouchPoint = touch->point;
    }
    else if(touch->phase == UIEvent::PHASE_DRAG || touch->phase == UIEvent::PHASE_ENDED)
    {
        Vector2 dp = oldTouchPoint - touch->point;
        viewXAngle += dp.x * 0.5f;
        viewYAngle += dp.y * 0.5f;
        oldTouchPoint = touch->point;
        //ClampAngles();
        //LOG_AS_FLOAT(viewXAngle);
        //LOG_AS_FLOAT(viewYAngle);
    }
}

void AnimationTestScreen::Update(float32 timeElapsed)
{
    aimUser.Identity();
    Matrix4 mt, mt2;
    mt.CreateTranslation(Vector3(0,10,0));
    aimUser *= mt;
    mt.CreateRotation(Vector3(0,0,1), DegToRad(viewXAngle));
    mt2.CreateRotation(Vector3(1,0,0), DegToRad(viewYAngle));
    mt2 *= mt;
    aimUser *= mt2;
    
    Vector3 dir = Vector3() * aimUser;
    cam->SetDirection(dir);
    
//    cam->SetTarget(targetPosition);
//
//    Vector3 position = targetPosition - Vector3(sinf(DegToRad(viewXAngle)) * 50, 50, cosf(DegToRad(viewXAngle)) * 50);
//    cam->SetPosition(position);
    
    
//    Camera * cam = scene->GetCamera(0);
//
//    Vector3 pos = cam->GetPosition();
//    cam->SetPosition(pos - cam->GetDirection());
//    
//	startRotationInSec -= timeElapsed;
//	if (startRotationInSec < 0.0f)
//		startRotationInSec = 0.0f;
//    
//	if (startRotationInSec == 0.0f)
//	{
//		if (Abs(rotationSpeed) > 8.0)
//		{
//			rotationSpeed = rotationSpeed * 0.8f;
//		}
//		
//		currentTankAngle += timeElapsed * rotationSpeed;
//	}
}

void AnimationTestScreen::Draw(const UIGeometricData &geometricData)
{
    //glClearColor(0.0, 0.0, 0.0, 1.0f);
    //glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    RenderManager::Instance()->ClearDepthBuffer();
}


bool AnimationTestScreen::IsNodeExpandable(UIHierarchy *forHierarchy, void *forNode)
{
    if (forNode) 
    {
        return ((SceneNode*)forNode)->GetChildrenCount() > 0;
    }
    
    return scene->GetChildrenCount() > 0;
}

int32 AnimationTestScreen::ChildrenCount(UIHierarchy *forHierarchy, void *forParent)
{
    if (forParent) 
    {
        return ((SceneNode*)forParent)->GetChildrenCount();
    }
    
    return scene->GetChildrenCount();
}


void *AnimationTestScreen::ChildAtIndex(UIHierarchy *forHierarchy, void *forParent, int32 index)
{
    if (forParent) 
    {
        return ((SceneNode*)forParent)->GetChild(index);
    }
    
    return scene->GetChild(index);
}

UIHierarchyCell *AnimationTestScreen::CellForNode(UIHierarchy *forHierarchy, void *node)
{
    UIHierarchyCell *c = forHierarchy->GetReusableCell("Node cell"); //try to get cell from the reusable cells store
    if(!c)
    { //if cell of requested type isn't find in the store create new cell
        c = new UIHierarchyCell(Rect(0, 0, 200, 15), "Node cell");
    }
        //fill cell whith data
    Font *fnt;
    fnt = FTFont::Create("~res:/Fonts/MyriadPro-Regular.otf");
//    fnt = GraphicsFont::Create("~res:/Fonts/korinna.def", "~res:/Gfx/Fonts2/korinna");
    fnt->SetSize(12);
    
    SceneNode *n = (SceneNode *)node;
    
    c->text->SetFont(fnt);
    c->text->SetText(StringToWString(n->name));
    c->text->SetAlign(ALIGN_LEFT|ALIGN_VCENTER);
    SafeRelease(fnt);
    
    Color color(0.1, 0.5, 0.05, 1.0);
	c->openButton->SetStateDrawType(UIControl::STATE_NORMAL, UIControlBackground::DRAW_FILL);
	c->openButton->SetStateDrawType(UIControl::STATE_PRESSED_INSIDE, UIControlBackground::DRAW_FILL);
	c->openButton->SetStateDrawType(UIControl::STATE_HOVER, UIControlBackground::DRAW_FILL);
	c->openButton->GetStateBackground(UIControl::STATE_NORMAL)->color = color;
	c->openButton->GetStateBackground(UIControl::STATE_HOVER)->color = color + 0.1;
	c->openButton->GetStateBackground(UIControl::STATE_PRESSED_INSIDE)->color = color + 0.3;
 
    return c;//returns cell
}


void AnimationTestScreen::OnCellSelected(UIHierarchy *forHierarchy, UIHierarchyCell *selectedCell)
{
}


