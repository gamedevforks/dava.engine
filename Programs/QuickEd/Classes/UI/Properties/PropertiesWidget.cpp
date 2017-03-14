#include "PropertiesWidget.h"

#include <qitemeditorfactory>
#include <qstyleditemdelegate>
#include <QMenu>
#include <QItemSelection>

#include "UI/QtModelPackageCommandExecutor.h"
#include "Model/ControlProperties/ComponentPropertiesSection.h"
#include "Model/ControlProperties/StyleSheetProperty.h"
#include "Model/ControlProperties/StyleSheetSelectorProperty.h"
#include "Model/ControlProperties/RootProperty.h"
#include "Model/PackageHierarchy/ControlNode.h"
#include "Model/PackageHierarchy/StyleSheetNode.h"
#include "Utils/QtDavaConvertion.h"

#include <QAbstractItemModel>

#include "ui_PropertiesWidget.h"
#include "PropertiesModel.h"
#include "UI/Properties/PropertiesTreeItemDelegate.h"

#include "Modules/LegacySupportModule/Private/Document.h"
#include "UI/Components/UIComponent.h"
#include "UI/UIControl.h"
#include "UI/Styles/UIStyleSheetPropertyDataBase.h"
#include "Engine/Engine.h"
#include "Entity/ComponentManager.h"

using namespace DAVA;

namespace
{
String GetPathFromIndex(QModelIndex index)
{
    QString path = index.data().toString();
    while (index.parent().isValid())
    {
        index = index.parent();
        path += "/" + index.data().toString();
    }
    return path.toStdString();
}
}

PropertiesWidget::PropertiesWidget(QWidget* parent)
    : QDockWidget(parent)
{
    setupUi(this);
    propertiesModel = new PropertiesModel(treeView);
    propertiesItemsDelegate = new PropertiesTreeItemDelegate(this);
    treeView->setModel(propertiesModel);
    connect(treeView->selectionModel(), &QItemSelectionModel::selectionChanged, this, &PropertiesWidget::OnSelectionChanged);
    connect(propertiesModel, &PropertiesModel::ComponentAdded, this, &PropertiesWidget::OnComponentAdded);

    treeView->setItemDelegate(propertiesItemsDelegate);

    addComponentAction = CreateAddComponentAction();
    treeView->addAction(addComponentAction);

    addStylePropertyAction = CreateAddStylePropertyAction();
    treeView->addAction(addStylePropertyAction);

    addStyleSelectorAction = CreateAddStyleSelectorAction();
    treeView->addAction(addStyleSelectorAction);

    treeView->addAction(CreateSeparator());

    removeAction = CreateRemoveAction();
    treeView->addAction(removeAction);

    connect(treeView, &QTreeView::expanded, this, &PropertiesWidget::OnExpanded);
    connect(treeView, &QTreeView::collapsed, this, &PropertiesWidget::OnCollapsed);
    DVASSERT(nullptr == selectedNode);
    UpdateModel(nullptr);
}

void PropertiesWidget::SetProject(const Project* project)
{
    propertiesItemsDelegate->SetProject(project);
}

void PropertiesWidget::OnDocumentChanged(Document* document)
{
    if (nullptr != document)
    {
        commandExecutor = document->GetCommandExecutor();
    }
    else
    {
        commandExecutor = nullptr;
    }
    UpdateModel(nullptr); //SelectionChanged will invoke by Queued Connection, so selectedNode have invalid value
}

void PropertiesWidget::OnAddComponent(QAction* action)
{
    DVASSERT(nullptr != commandExecutor);
    if (nullptr != commandExecutor)
    {
        const RootProperty* rootProperty = DAVA::DynamicTypeCheck<const RootProperty*>(propertiesModel->GetRootProperty());
        const Type* componentType = action->data().value<Any>().Cast<const Type*>();
        ComponentPropertiesSection* componentSection = rootProperty->FindComponentPropertiesSection(componentType, 0);
        if (componentSection != nullptr && !UIComponent::IsMultiple(componentType))
        {
            QModelIndex index = propertiesModel->indexByProperty(componentSection);
            OnComponentAdded(index);
        }
        else
        {
            commandExecutor->AddComponent(DynamicTypeCheck<ControlNode*>(selectedNode), componentType);
        }
    }
}

void PropertiesWidget::OnRemove()
{
    DVASSERT(nullptr != commandExecutor);
    if (nullptr != commandExecutor)
    {
        QModelIndexList indices = treeView->selectionModel()->selectedIndexes();
        if (!indices.empty())
        {
            const QModelIndex& index = indices.first();
            AbstractProperty* property = static_cast<AbstractProperty*>(index.internalPointer());

            if ((property->GetFlags() & AbstractProperty::EF_CAN_REMOVE) != 0)
            {
                ComponentPropertiesSection* section = dynamic_cast<ComponentPropertiesSection*>(property);
                if (section)
                {
                    commandExecutor->RemoveComponent(DynamicTypeCheck<ControlNode*>(selectedNode), section->GetComponentType(), section->GetComponentIndex());
                }
                else
                {
                    StyleSheetProperty* styleProperty = dynamic_cast<StyleSheetProperty*>(property);
                    if (styleProperty)
                    {
                        commandExecutor->RemoveStyleProperty(DynamicTypeCheck<StyleSheetNode*>(selectedNode), styleProperty->GetPropertyIndex());
                    }
                    else
                    {
                        StyleSheetSelectorProperty* selectorProperty = dynamic_cast<StyleSheetSelectorProperty*>(property);
                        if (selectorProperty)
                        {
                            int32 index = property->GetParent()->GetIndex(selectorProperty);
                            if (index != -1)
                            {
                                commandExecutor->RemoveStyleSelector(DynamicTypeCheck<StyleSheetNode*>(selectedNode), index);
                            }
                        }
                    }
                }
            }
        }
        UpdateActions();
    }
}

void PropertiesWidget::OnAddStyleProperty(QAction* action)
{
    DVASSERT(nullptr != commandExecutor);
    if (nullptr != commandExecutor)
    {
        uint32 propertyIndex = action->data().toUInt();
        if (propertyIndex < UIStyleSheetPropertyDataBase::STYLE_SHEET_PROPERTY_COUNT)
        {
            commandExecutor->AddStyleProperty(DynamicTypeCheck<StyleSheetNode*>(selectedNode), propertyIndex);
        }
        else
        {
            DVASSERT(propertyIndex < UIStyleSheetPropertyDataBase::STYLE_SHEET_PROPERTY_COUNT);
        }
    }
}

void PropertiesWidget::OnAddStyleSelector()
{
    DVASSERT(nullptr != commandExecutor);
    if (nullptr != commandExecutor)
    {
        commandExecutor->AddStyleSelector(DynamicTypeCheck<StyleSheetNode*>(selectedNode));
    }
}

void PropertiesWidget::OnSelectionChanged(const QItemSelection& /*selected*/, const QItemSelection& /*deselected*/)
{
    UpdateActions();
}

QAction* PropertiesWidget::CreateAddComponentAction()
{
    QMenu* addComponentMenu = new QMenu(this);
    auto& types = GetEngineContext()->componentManager->GetRegisteredTypes();
    for (auto& pair : types)
    {
        const String& name = ReflectedTypeDB::GetByType(pair.first)->GetPermanentName();
        QAction* componentAction = new QAction(name.c_str(), this); // TODO: Localize name
        componentAction->setData(QVariant::fromValue(Any(pair.first)));
        addComponentMenu->addAction(componentAction);
    }
    connect(addComponentMenu, &QMenu::triggered, this, &PropertiesWidget::OnAddComponent);

    QAction* action = new QAction(tr("Add Component"), this);
    action->setMenu(addComponentMenu);
    addComponentMenu->setEnabled(false);
    return action;
}

QAction* PropertiesWidget::CreateAddStyleSelectorAction()
{
    QAction* action = new QAction(tr("Add Style Selector"), this);
    connect(action, &QAction::triggered, this, &PropertiesWidget::OnAddStyleSelector);
    action->setEnabled(false);
    return action;
}

QAction* PropertiesWidget::CreateAddStylePropertyAction()
{
    QMenu* propertiesMenu = new QMenu(this);
    QMenu* groupMenu = nullptr;
    UIStyleSheetPropertyGroup* prevGroup = nullptr;
    UIStyleSheetPropertyDataBase* db = UIStyleSheetPropertyDataBase::Instance();
    for (int32 i = 0; i < UIStyleSheetPropertyDataBase::STYLE_SHEET_PROPERTY_COUNT; i++)
    {
        const UIStyleSheetPropertyDescriptor& descr = db->GetStyleSheetPropertyByIndex(i);
        if (descr.group != prevGroup)
        {
            prevGroup = descr.group;
            if (descr.group->prefix.empty())
            {
                groupMenu = propertiesMenu;
            }
            else
            {
                groupMenu = new QMenu(QString::fromStdString(descr.group->prefix), this);
                propertiesMenu->addMenu(groupMenu);
            }
        }
        QAction* componentAction = new QAction(descr.name.c_str(), this);
        componentAction->setData(i);

        groupMenu->addAction(componentAction);
    }
    connect(propertiesMenu, &QMenu::triggered, this, &PropertiesWidget::OnAddStyleProperty);

    QAction* action = new QAction(tr("Add Style Property"), this);
    action->setMenu(propertiesMenu);
    propertiesMenu->setEnabled(false);
    return action;
}

QAction* PropertiesWidget::CreateRemoveAction()
{
    QAction* action = new QAction(tr("Remove"), this);
    connect(action, &QAction::triggered, this, &PropertiesWidget::OnRemove);
    action->setEnabled(false);
    return action;
}

QAction* PropertiesWidget::CreateSeparator()
{
    QAction* separator = new QAction(this);
    separator->setSeparator(true);
    return separator;
}

void PropertiesWidget::OnModelUpdated()
{
    bool blocked = treeView->blockSignals(true);
    treeView->expandToDepth(0);
    treeView->blockSignals(blocked);
    treeView->resizeColumnToContents(0);
    ApplyExpanding();
}

void PropertiesWidget::OnExpanded(const QModelIndex& index)
{
    itemsState[GetPathFromIndex(index)] = true;
}

void PropertiesWidget::OnCollapsed(const QModelIndex& index)
{
    itemsState[GetPathFromIndex(index)] = false;
}

void PropertiesWidget::OnComponentAdded(const QModelIndex& index)
{
    treeView->expand(index);
    treeView->setCurrentIndex(index);
    int rowCount = propertiesModel->rowCount(index);
    if (rowCount > 0)
    {
        QModelIndex lastChildIndex = index.child(rowCount - 1, 0);
        treeView->scrollTo(lastChildIndex, QAbstractItemView::EnsureVisible);
    }
    treeView->scrollTo(index, QAbstractItemView::EnsureVisible);
}

void PropertiesWidget::UpdateModel(PackageBaseNode* node)
{
    if (node == selectedNode)
    {
        return;
    }
    if (nullptr != selectedNode)
    {
        auto index = treeView->indexAt(QPoint(0, 0));
        lastTopIndexPath = GetPathFromIndex(index);
    }
    selectedNode = node;
    propertiesModel->Reset(selectedNode, commandExecutor);
    bool isControl = dynamic_cast<ControlNode*>(selectedNode) != nullptr;
    bool isStyle = dynamic_cast<StyleSheetNode*>(selectedNode) != nullptr;
    addComponentAction->menu()->setEnabled(isControl);
    addStylePropertyAction->menu()->setEnabled(isStyle);
    addStyleSelectorAction->setEnabled(isStyle);
    removeAction->setEnabled(false);

    //delay long time work with view
    QMetaObject::invokeMethod(this, "OnModelUpdated", Qt::QueuedConnection);
}

void PropertiesWidget::UpdateActions()
{
    QModelIndexList indices = treeView->selectionModel()->selectedIndexes();
    if (!indices.empty())
    {
        AbstractProperty* property = static_cast<AbstractProperty*>(indices.first().internalPointer());
        removeAction->setEnabled((property->GetFlags() & AbstractProperty::EF_CAN_REMOVE) != 0);
    }
}

void PropertiesWidget::ApplyExpanding()
{
    QModelIndex index = propertiesModel->index(0, 0);
    while (index.isValid())
    {
        const auto& path = GetPathFromIndex(index);
        if (path == lastTopIndexPath)
        {
            treeView->scrollTo(index, QTreeView::PositionAtTop);
        }
        auto iter = itemsState.find(path);
        if (iter != itemsState.end())
        {
            treeView->setExpanded(index, iter->second);
        }

        index = treeView->indexBelow(index);
    }
}
