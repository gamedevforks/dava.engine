/*==================================================================================
    Copyright (c) 2014, thorin
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



#include "Scene3D/Systems/AnimationSystem.h"
#include "Scene3D/Components/AnimationComponent.h"
#include "Scene3D/Entity.h"
#include "Debug/DVAssert.h"
#include "Scene3D/Systems/EventSystem.h"
#include "Scene3D/Scene.h"
#include "Scene3D/Systems/GlobalEventSystem.h"
#include "Debug/Stats.h"
#include "Scene3D/AnimationData.h"
#include "Scene3D/Components/TransformComponent.h"

namespace DAVA
{

AnimationSystem::AnimationSystem(Scene * scene)
:	SceneSystem(scene)
{

}

AnimationSystem::~AnimationSystem()
{

}

void AnimationSystem::Process(float32 timeElapsed)
{
    TIME_PROFILE("TransformSystem::Process");
    Vector<AnimationComponent*>::iterator it, endit;
    for (it = items.begin(), endit = items.end(); it!= endit; ++it)
    {
        AnimationComponent * comp = *it;
        if (comp->isPlaying && comp->animation)
        {
            comp->time += timeElapsed;

            if (comp->time > comp->animation->duration)
            {
                if (comp->repeat)
                    comp->time -= comp->animation->duration;
                else
                    comp->SetIsPlaying(false);
            }

            SceneNodeAnimationKey & key = comp->animation->Interpolate(comp->time);
            Matrix4 animTransform;
            key.GetMatrix(animTransform);
            Matrix4 result = comp->animation->invPose * animTransform;
            comp->GetEntity()->SetLocalTransform(result);
        }
    }
}

void AnimationSystem::RegisterComponent(Entity* entity, Component* component)
{
    if (component->GetType() == Component::ANIMATION_COMPONENT)
    {
        items.push_back(static_cast< AnimationComponent* >(component));
    }
}

void AnimationSystem::UnregisterComponent(Entity* entity, Component* component)
{
    if (component->GetType() == Component::ANIMATION_COMPONENT)
    {
        for (uint32 i = 0; i < items.size(); ++i)
        {
            items[i] = items.back();
        }
        items.pop_back();
    }
}

void AnimationSystem::ImmediateEvent(Entity * entity, uint32 event)
{

}


void AnimationSystem::AddEntity(Entity * entity)
{
    if (entity->GetComponent(Component::ANIMATION_COMPONENT))
        RegisterComponent(entity, entity->GetComponent(Component::ANIMATION_COMPONENT));
}

void AnimationSystem::RemoveEntity(Entity * entity)
{
    if (entity->GetComponent(Component::ANIMATION_COMPONENT))
        UnregisterComponent(entity, entity->GetComponent(Component::ANIMATION_COMPONENT));
}

void AnimationSystem::PlaySceneAnimations( void )
{
    Vector<AnimationComponent*>::iterator it, endit;
    for (it = items.begin(), endit = items.end(); it!= endit; ++it)
    {
        AnimationComponent * comp = *it;
        if (comp->autoStart)
        {
            comp->SetIsPlaying(true);
        }
    }
}

void AnimationSystem::StopSceneAnimations( void )
{
    Vector<AnimationComponent*>::iterator it, endit;
    for (it = items.begin(), endit = items.end(); it!= endit; ++it)
    {
        AnimationComponent * comp = *it;
        comp->SetIsPlaying(false);
    }
}

};
