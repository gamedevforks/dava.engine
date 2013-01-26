#include "Scene3D/Systems/ParticleEffectSystem.h"
#include "Scene3D/Components/ParticleEffectComponent.h"
#include "Particles/ParticleEmitter.h"
#include "Platform/SystemTimer.h"

namespace DAVA
{

ParticleEffectSystem::ParticleEffectSystem()
:	BaseProcessSystem(Component::PARTICLE_EFFECT_COMPONENT)
{

}

void ParticleEffectSystem::Process()
{
	float32 timeElapsed = SystemTimer::Instance()->FrameDelta();

	size = components.size();
	for(index = 0; index < size; ++index)
	{
		ParticleEffectComponent * component = static_cast<ParticleEffectComponent*>(components[index]);
		component->EffectUpdate(timeElapsed);
	}
}

void ParticleEffectSystem::RemoveEntity(SceneNode * entity)
{
	BaseProcessSystem::RemoveEntity(entity);
	--size;
	--index;
}

}