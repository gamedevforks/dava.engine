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

#include "SoundComponent.h"
#include "Sound/SoundSystem.h"
#include "Sound/SoundEvent.h"
#include "Base/FastName.h"

using namespace DAVA;
	REGISTER_CLASS(SoundComponent)

SoundComponent::SoundComponent() {}

SoundComponent::~SoundComponent()
{
    RemoveAllEvents();
}

SoundEvent * SoundComponent::GetSoundEvent(int32 index)
{
    DVASSERT(index >= 0 && index < (int32)events.size());
    return events[index];
}

void SoundComponent::AddSoundEvent(SoundEvent * _event)
{
    DVASSERT(_event);

    SafeRetain(_event);
    events.push_back(_event);
}

int32 SoundComponent::GetEventsCount()
{
    return events.size();
}

void SoundComponent::RemoveSoundEvent(SoundEvent * event)
{
    Vector<SoundEvent *>::iterator it = events.begin();
    Vector<SoundEvent *>::const_iterator itEnd = events.end();
    for(; it != itEnd; ++it)
    {
        if((*it) == event)
        {
            (*it)->Release();
            events.erase(it);
            return;
        }
    }
}

void SoundComponent::RemoveAllEvents()
{
    int32 eventsCount = events.size();
    for(int32 i = 0; i < eventsCount; ++i)
        SafeRelease(events[i]);

    events.clear();
}

Component * SoundComponent::Clone(Entity * toEntity)
{
    SoundComponent * soundComponent = new SoundComponent();
    soundComponent->SetEntity(toEntity);
    
    int32 eventCount = events.size();
    for(int32 i = 0; i < eventCount; ++i)
        soundComponent->AddSoundEvent(events[i]);
    
    return soundComponent;
}

void SoundComponent::Serialize(KeyedArchive *archive, SerializationContext *serializationContext)
{
    Component::Serialize(archive, serializationContext);

    if(archive)
    {
        uint32 eventsCount = events.size();
        archive->SetUInt32("sc.eventCount", eventsCount);
        for(uint32 i = 0; i < eventsCount; ++i)
        {
            KeyedArchive* eventArchive = new KeyedArchive();
            SoundSystem::Instance()->SerializeEvent(events[i], eventArchive);
            archive->SetArchive(KeyedArchive::GenKeyFromIndex(i), eventArchive);
            SafeRelease(eventArchive);
        }
    }
}

void SoundComponent::Deserialize(KeyedArchive *archive, SerializationContext *serializationContext)
{
    events.clear();

    if(archive)
    {
        uint32 eventsCount = archive->GetUInt32("sc.eventCount");
        for(uint32 i = 0; i < eventsCount; ++i)
        {
            KeyedArchive* eventArchive = archive->GetArchive(KeyedArchive::GenKeyFromIndex(i));
            SoundEvent * sEvent = SoundSystem::Instance()->DeserializeEventFromArchive(eventArchive);
            AddSoundEvent(sEvent);
            SafeRelease(sEvent);
        }
    }

    Component::Deserialize(archive, serializationContext);
}
