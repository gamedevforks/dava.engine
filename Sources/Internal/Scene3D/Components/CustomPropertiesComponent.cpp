/*==================================================================================
 Copyright (c) 2008, DAVA, INC
 All rights reserved.
 
 Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
 * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
 * Neither the name of the DAVA, INC nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
 
 THIS SOFTWARE IS PROVIDED BY THE DAVA, INC AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL DAVA, INC BE LIABLE FOR ANY
 DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 =====================================================================================*/


#include "Scene3D/Components/CustomPropertiesComponent.h"
#include "Scene3D/Entity.h"
#include "Scene3D/Scene.h"
#include "Scene3D/SceneFileV2.h"
#include "Scene3D/Systems/EventSystem.h"
#include "Scene3D/Systems/GlobalEventSystem.h"
#include "FileSystem/FileSystem.h"

namespace DAVA
{
	CustomPropertiesComponent::CustomPropertiesComponent()
	{
		properties = new KeyedArchive();
	}
	
	CustomPropertiesComponent::CustomPropertiesComponent(const KeyedArchive& srcProperties)
	{
		properties = new KeyedArchive(srcProperties);
	}
	
	CustomPropertiesComponent::~CustomPropertiesComponent()
	{
		SafeRelease(properties);
	}
		
	Component* CustomPropertiesComponent::Clone(Entity * toEntity)
	{
		CustomPropertiesComponent* newProperties = new CustomPropertiesComponent(*properties);
		newProperties->SetEntity(toEntity);
		
		return newProperties;
	}
	
	void CustomPropertiesComponent::Serialize(KeyedArchive *archive, SceneFileV2 *sceneFile)
	{
		Component::Serialize(archive, sceneFile);
		
		if(NULL != archive && properties->Count() > 0)
		{
			String savedPath = "";
			if(properties->IsKeyExists("editor.referenceToOwner"))
			{
				savedPath = properties->GetString("editor.referenceToOwner");
				String newPath = FilePath(savedPath).GetRelativePathname(sceneFile->GetScenePath());
				properties->SetString("editor.referenceToOwner", newPath);
			}
			
			archive->SetByteArrayFromArchive("cpc.properties", properties);
			
			if(savedPath.length())
			{
				properties->SetString("editor.referenceToOwner", savedPath);
			}
		}
	}
	
	void CustomPropertiesComponent::Deserialize(KeyedArchive *archive, SceneFileV2 *sceneFile)
	{
		properties->DeleteAllKeys();
		
		if(NULL != archive && archive->IsKeyExists("cpc.properties"))
		{
			LoadFromArchive(*archive, sceneFile);
		}
		
		Component::Deserialize(archive, sceneFile);
	}

	KeyedArchive* CustomPropertiesComponent::GetArchive()
	{
		return properties;
	}
	
	void CustomPropertiesComponent::LoadFromArchive(const KeyedArchive& srcProperties, SceneFileV2 *sceneFile)
	{
		SafeRelease(properties);
		properties = new KeyedArchive(srcProperties);
		
		if(properties && properties->IsKeyExists("editor.referenceToOwner"))
		{
			FilePath newPath(sceneFile->GetScenePath());
			newPath += properties->GetString("editor.referenceToOwner");
			
			//TODO: why we use absolute pathname instead of relative?
			properties->SetString("editor.referenceToOwner", newPath.GetAbsolutePathname());
		}
	}
};