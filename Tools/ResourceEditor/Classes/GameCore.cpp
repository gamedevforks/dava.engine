/*
 *  GameCore.cpp
 *  TemplateProjectMacOS
 *
 *  Created by Vitaliy  Borodovsky on 3/19/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "GameCore.h"
#include "AppScreens.h"
#include "ResourcePackerScreen.h"
#include "SceneEditor/SceneEditorScreen.h"
#include "SceneEditor/SceneEditorScreenMain.h"

#ifdef __DAVAENGINE_BEAST__
#include "BeastProxyImpl.h"
#endif //__DAVAENGINE_BEAST__


using namespace DAVA;


GameCore::GameCore()
{
}

GameCore::~GameCore()
{
	
}

void GameCore::OnAppStarted()
{
	RenderManager::Instance()->SetFPS(30);

#ifdef __DAVAENGINE_BEAST__
	new BeastProxyImpl();
#endif //__DAVAENGINE_BEAST__
	
	resourcePackerScreen = new ResourcePackerScreen();
    sceneEditorScreen = new SceneEditorScreen();
    sceneEditorScreenMain = new SceneEditorScreenMain();

	UIScreenManager::Instance()->RegisterScreen(SCREEN_RESOURCE_PACKER, resourcePackerScreen);
    UIScreenManager::Instance()->RegisterScreen(SCREEN_SCENE_EDITOR, sceneEditorScreen);
    UIScreenManager::Instance()->RegisterScreen(SCREEN_SCENE_EDITOR_MAIN, sceneEditorScreenMain);
    
//	UIScreenManager::Instance()->SetFirst(SCREEN_SCENE_EDITOR);
    UIScreenManager::Instance()->SetFirst(SCREEN_SCENE_EDITOR_MAIN);
}

void GameCore::OnAppFinished()
{
	SafeRelease(resourcePackerScreen);
    SafeRelease(sceneEditorScreen);
}

void GameCore::OnSuspend()
{
    ApplicationCore::OnSuspend();
}

void GameCore::OnResume()
{
    ApplicationCore::OnResume();
}

void GameCore::OnBackground()
{
	//ApplicationCore::OnBackground();
}

void GameCore::BeginFrame()
{
	ApplicationCore::BeginFrame();
	RenderManager::Instance()->ClearWithColor(0, 0, 0, 0);
}

void GameCore::Update(float32 timeElapsed)
{	
	ApplicationCore::Update(timeElapsed);
}

void GameCore::Draw()
{
	ApplicationCore::Draw();

}