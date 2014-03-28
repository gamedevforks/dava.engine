/*==================================================================================
    Copyright (c) 2008, binaryzebra
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
    * Neither the name of the binaryzebra nor the
    names of its contributors may be used to endorse or promote products
    derived from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE binaryzebra AND CONTRIBUTORS "AS IS" AND
    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL binaryzebra BE LIABLE FOR ANY
    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
=====================================================================================*/


#include "Scene3D/Systems/SoundUpdateSystem.h"
#include "Scene3D/Systems/EventSystem.h"
#include "Scene3D/Entity.h"
#include "Scene3D/Scene.h"
#include "Scene3D/Components/TransformComponent.h"
#include "Scene3D/Components/SoundComponent.h"
#include "Scene3D/Components/ComponentHelpers.h"
#include "Sound/SoundSystem.h"

namespace DAVA
{
SoundUpdateSystem::SoundUpdateSystem(Scene * scene)
:	SceneSystem(scene)
{
	scene->GetEventSystem()->RegisterSystemForEvent(this, EventSystem::WORLD_TRANSFORM_CHANGED);
}

SoundUpdateSystem::~SoundUpdateSystem()
{
}

void SoundUpdateSystem::ImmediateEvent(Entity * entity, uint32 event)
{
	if (event == EventSystem::WORLD_TRANSFORM_CHANGED)
	{
		const Matrix4 & worldTransform = GetTransformComponent(entity)->GetWorldTransform();
		Vector3 translation = worldTransform.GetTranslationVector();

        SoundComponent * sc = GetSoundComponent(entity);
        if(sc)
        {
            Vector3 worldDirection;
            bool needCalcDirection = true;

            uint32 eventsCount = sc->GetEventsCount();
            for(uint32 i = 0; i < eventsCount; ++i)
            {
                SoundEvent * sound = sc->GetSoundEvent(i);
                sound->SetPosition(translation);
                if(sound->IsDirectional())
                {
                    if(needCalcDirection)
                    {
                        worldDirection = MultiplyVectorMat3x3(sc->GetLocalDirection(), worldTransform);
                        needCalcDirection = false;
                    }
                    sound->SetDirection(worldDirection);
                }
                sound->UpdateInstancesPosition();
            }
        }
	}
}
    
void SoundUpdateSystem::Process(float32 timeElapsed)
{
    Camera * activeCamera = GetScene()->GetCurrentCamera();

    if(activeCamera)
    {
        SoundSystem * ss = SoundSystem::Instance();
        ss->SetListenerPosition(activeCamera->GetPosition());
        ss->SetListenerOrientation(activeCamera->GetDirection(), activeCamera->GetLeft());
    }
}

};