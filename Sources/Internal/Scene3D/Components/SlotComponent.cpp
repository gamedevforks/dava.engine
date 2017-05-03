#include "Scene3D/Components/SlotComponent.h"

#include "Scene3D/Entity.h"
#include "Reflection/ReflectionRegistrator.h"
#include "Reflection/ReflectedMeta.h"
#include "FileSystem/KeyedArchive.h"

namespace DAVA
{
DAVA_VIRTUAL_REFLECTION_IMPL(SlotComponent)
{
    ReflectionRegistrator<SlotComponent>::Begin()
    .ConstructorByPointer()
    .Field(SlotNameFieldName.c_str(), &SlotComponent::GetSlotName, &SlotComponent::SetSlotName)[M::DisplayName("Name")]
    .Field(ConfigPathFieldName.c_str(), &SlotComponent::GetConfigFilePath, &SlotComponent::SetConfigFilePath)[M::DisplayName("Items list")]
    .Field("transform", &SlotComponent::GetAttachmentTransform, &SlotComponent::SetAttachmentTransform)[M::DisplayName("Attachment Transform")]
    .End();
}

Component* SlotComponent::Clone(Entity* toEntity)
{
    SlotComponent* clone = new SlotComponent();
    clone->SetEntity(toEntity);
    clone->slotName = slotName;
    clone->attachmentTransform = attachmentTransform;
    clone->configFilePath = configFilePath;
    clone->typeFilters = typeFilters;

    return clone;
}

void SlotComponent::Serialize(KeyedArchive* archive, SerializationContext* serializationContext)
{
    Component::Serialize(archive, serializationContext);

    if (archive != nullptr)
    {
        archive->SetFastName("sc.slotName", slotName);
        archive->SetMatrix4("sc.attachmentTransform", attachmentTransform);
        archive->SetString("sc.configFilePath", configFilePath.GetRelativePathname(serializationContext->GetScenePath()));
        archive->SetUInt32("sc.typeFiltersCount", actualFiltersCount);
        for (uint32 i = 0; i < actualFiltersCount; ++i)
        {
            archive->SetFastName(Format("sc.typeFilter_%u", i), typeFilters[i]);
        }
    }
}

void SlotComponent::Deserialize(KeyedArchive* archive, SerializationContext* serializationContext)
{
    if (archive != nullptr)
    {
        slotName = archive->GetFastName("sc.slotName");
        attachmentTransform = archive->GetMatrix4("sc.attachmentTransform");
        configFilePath = FilePath(serializationContext->GetScenePath() + archive->GetString("sc.configFilePath"));
        actualFiltersCount = archive->GetUInt32("sc.typeFiltersCount");
        for (uint32 i = 0; i < actualFiltersCount; ++i)
        {
            typeFilters[i] = archive->GetFastName(Format("sc.typeFilter_%u", i));
        }
    }

    Component::Deserialize(archive, serializationContext);
}

FastName SlotComponent::GetSlotName() const
{
    return slotName;
}

void SlotComponent::SetSlotName(FastName name)
{
    slotName = name;
}

const DAVA::Matrix4& SlotComponent::GetAttachmentTransform() const
{
    return attachmentTransform;
}

void SlotComponent::SetAttachmentTransform(const Matrix4& transform)
{
    attachmentTransform = transform;
}

const DAVA::FilePath& SlotComponent::GetConfigFilePath() const
{
    return configFilePath;
}

void SlotComponent::SetConfigFilePath(const FilePath& path)
{
    configFilePath = path;
}

uint32 SlotComponent::GetTypeFiltersCount() const
{
    return actualFiltersCount;
}

FastName SlotComponent::GetTypeFilter(uint32 index) const
{
    DVASSERT(index < actualFiltersCount);
    return typeFilters[index];
}

void SlotComponent::AddTypeFilter(FastName filter)
{
    DVASSERT(actualFiltersCount < MAX_FILTERS_COUNT);
#ifdef __DAVAENGINE_DEBUG__
    for (uint32 i = 0; i < actualFiltersCount; ++i)
    {
        DVASSERT(typeFilters[i] != filter);
    }
#endif
    typeFilters[actualFiltersCount++] = filter;
}

void SlotComponent::RemoveTypeFilter(uint32 index)
{
    DVASSERT(actualFiltersCount > 0 && index < actualFiltersCount);
    std::copy(typeFilters.begin() + index + 1, typeFilters.end(), typeFilters.begin() + index);
    --actualFiltersCount;
}

void SlotComponent::RemoveTypeFilter(FastName filter)
{
    for (uint32 i = 0; i < actualFiltersCount; ++i)
    {
        if (typeFilters[i] == filter)
        {
            RemoveTypeFilter(i);
            break;
        }
    }
}

DAVA::FastName SlotComponent::GetLoadedItemName() const
{
    return loadedItemName;
}

const FastName SlotComponent::SlotNameFieldName = FastName("slotName");
const FastName SlotComponent::ConfigPathFieldName = FastName("configPath");

} // namespace DAVA
