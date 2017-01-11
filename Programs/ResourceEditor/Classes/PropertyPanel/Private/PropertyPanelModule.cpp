#include "Classes/PropertyPanel/PropertyPanelModule.h"
#include "Classes/PropertyPanel/PropertyModelExt.h"
#include "Classes/Selection/SelectionData.h"
#include "Classes/Application/REGlobal.h"

#include "TArc/Controls/PropertyPanel/PropertiesView.h"
#include "TArc/WindowSubSystem/UI.h"
#include "TArc/WindowSubSystem/ActionUtils.h"
#include "TArc/DataProcessing/DataNode.h"
#include "TArc/Utils/ModuleCollection.h"
#include "TArc/Core/FieldBinder.h"

#include "Scene3D/Entity.h"
#include "Reflection/ReflectionRegistrator.h"
#include "Base/FastName.h"

#include <QPointer>
#include <QQmlEngine>
#include <QList>
#include <QString>
#include <QTimer>

namespace PropertyPanelModuleDetail
{
class PropertyPanelData : public DAVA::TArc::DataNode
{
public:
    Vector<Entity*> selectedEntities;
    Vector<Reflection> propertyPanelObjects;

    static const char* selectedEntitiesProperty;

    DAVA_VIRTUAL_REFLECTION(PropertyPanelData, DAVA::TArc::DataNode)
    {
        DAVA::ReflectionRegistrator<PropertyPanelData>::Begin()
        .Field(selectedEntitiesProperty, &PropertyPanelData::propertyPanelObjects)
        .End();
    }
};

const char* PropertyPanelData::selectedEntitiesProperty = "selectedEntities";
}

PropertyPanelModule::~PropertyPanelModule() = default;

void PropertyPanelModule::PostInit()
{
    using namespace DAVA::TArc;
    using PropertyPanelData = PropertyPanelModuleDetail::PropertyPanelData;
    UI* ui = GetUI();

    ContextAccessor* accessor = GetAccessor();
    DataContext* ctx = accessor->GetGlobalContext();
    ctx->CreateData(std::make_unique<PropertyPanelData>());

    DockPanelInfo panelInfo;
    panelInfo.title = QStringLiteral("New Property Panel");
    panelInfo.actionPlacementInfo = ActionPlacementInfo(CreateMenuPoint(QList<QString>() << "View"
                                                                                         << "Dock"));

    FieldDescriptor propertiesDataSourceField;
    propertiesDataSourceField.type = ReflectedTypeDB::Get<PropertyPanelModuleDetail::PropertyPanelData>();
    propertiesDataSourceField.fieldName = FastName(PropertyPanelModuleDetail::PropertyPanelData::selectedEntitiesProperty);

    PropertiesView* view = new PropertiesView(accessor, propertiesDataSourceField);
    view->RegisterExtension(std::make_shared<REModifyPropertyExtension>(accessor));
    ui->AddView(REGlobal::MainWindowKey, PanelKey(panelInfo.title, panelInfo), view);

    // Bind to current selection changed
    binder.reset(new FieldBinder(accessor));
    DAVA::TArc::FieldDescriptor fieldDescr;
    fieldDescr.fieldName = DAVA::FastName(SelectionData::selectionPropertyName);
    fieldDescr.type = DAVA::ReflectedTypeDB::Get<SelectionData>();
    binder->BindField(fieldDescr, DAVA::MakeFunction(this, &PropertyPanelModule::SceneSelectionChanged));
}

void PropertyPanelModule::SceneSelectionChanged(const DAVA::Any& newSelection)
{
    using namespace DAVA::TArc;

    DataContext* ctx = GetAccessor()->GetGlobalContext();
    PropertyPanelModuleDetail::PropertyPanelData* data = ctx->GetData<PropertyPanelModuleDetail::PropertyPanelData>();
    data->selectedEntities.clear();
    data->selectedEntities.shrink_to_fit();
    data->propertyPanelObjects.clear();

    if (newSelection.CanGet<SelectableGroup>())
    {
        const SelectableGroup& group = newSelection.Get<SelectableGroup>();
        for (auto entity : group.ObjectsOfType<DAVA::Entity>())
        {
            data->selectedEntities.push_back(entity);
        }

        for (size_t i = 0; i < data->selectedEntities.size(); ++i)
        {
            data->propertyPanelObjects.push_back(DAVA::Reflection::Create(&data->selectedEntities[i]));
        }
    }
}

DAVA_REFLECTION_IMPL(PropertyPanelModule)
{
    DAVA::ReflectionRegistrator<PropertyPanelModule>::Begin()
        .ConstructorByPointer()
        .End();
}

DECL_GUI_MODULE(PropertyPanelModule);
