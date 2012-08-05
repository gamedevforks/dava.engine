#include "GraphModel.h"
#include "GraphItem.h"

#include <QTreeView>


GraphModel::GraphModel(QObject *parent)
    :   QAbstractItemModel(parent)
    ,   rootItem(NULL)
    ,   attachedTreeView(NULL)
{
    itemSelectionModel = new QItemSelectionModel(this);
    
    connect(itemSelectionModel, SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)), this, SLOT(SelectionChanged(const QItemSelection &, const QItemSelection &)));
}

GraphModel::~GraphModel()
{
    SafeDelete(itemSelectionModel);
	SafeRelease(rootItem);
}

GraphItem * GraphModel::ParentItem(const QModelIndex &parent) const
{
	if (parent.isValid())
	{
		return static_cast<GraphItem*>(parent.internalPointer());
	}

    return rootItem;
}

QModelIndex GraphModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
	{
		return QModelIndex();
	}

    GraphItem *parentItem = ParentItem(parent);
    GraphItem *childItem = parentItem->Child(row);
    if (childItem)
	{
		return createIndex(row, column, childItem);
	}

	return QModelIndex();
}

QModelIndex GraphModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
	{
		return QModelIndex();
	}

    GraphItem *childItem = static_cast<GraphItem*>(index.internalPointer());
    GraphItem *parentItem = childItem->GetParent();

    if(parentItem && (parentItem != rootItem))
    {
        return createIndex(parentItem->Row(), 0, parentItem);
    }

    return QModelIndex();
}

int GraphModel::rowCount(const QModelIndex &parent) const
{
    if (parent.column() > 0)
	{
		return 0;
	}

    GraphItem *parentItem = ParentItem(parent);
    if(parentItem)
    {
        return parentItem->ChildrenCount();
    }

    return 0;
}

int GraphModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
	{
		return static_cast<GraphItem*>(parent.internalPointer())->ColumnCount();
	}

	return rootItem->ColumnCount();
}

QVariant GraphModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
	{
		return QVariant();
	}

    if (Qt::DisplayRole == role)
	{
        GraphItem *item = static_cast<GraphItem*>(index.internalPointer());
        return item->Data(index.column());
	}

    return QVariant();
}

Qt::ItemFlags GraphModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
	{
		return 0;
	}

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}


QVariant GraphModel::headerData(int section, Qt::Orientation orientation,
                               int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole && rootItem)
	{
		return rootItem->Data(section);
	}

    return QVariant();
}

QItemSelectionModel *GraphModel::GetSelectionModel()
{
    return itemSelectionModel;
}

void GraphModel::SelectionChanged(const QItemSelection &, const QItemSelection &)
{
}

GraphItem * GraphModel::ItemForData(void * data)
{
    if(rootItem)
    {
        return ItemForData(rootItem, data);
    }
    
    return NULL;
}

GraphItem * GraphModel::ItemForData(GraphItem *item, void * data)
{
    if(item->GetUserData() == data)
    {
        return item;
    }
    
    for(int32 i = 0; i < item->ChildrenCount(); ++i)
    {
        GraphItem *foundItem = ItemForData(item->Child(i), data);
        if(foundItem)
        {
            return foundItem;
        }
    }
    
    return NULL;
}

void GraphModel::Deactivate()
{
    attachedTreeView = NULL;
}

void GraphModel::Activate(QTreeView *view)
{
    DVASSERT((NULL == attachedTreeView) && "View must be deactivated")
    
    attachedTreeView = view;
    
    attachedTreeView->setModel(this);
    attachedTreeView->setSelectionModel(itemSelectionModel);
}