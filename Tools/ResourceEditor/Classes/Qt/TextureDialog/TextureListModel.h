#ifndef __TEXTURE_LIST_MODEL_H__
#define __TEXTURE_LIST_MODEL_H__

#include <QAbstractListModel>
#include <QVector>
#include "DAVAEngine.h"

class TextureListModel : public QAbstractListModel
{
	Q_OBJECT

public:
	enum TextureListSortMode
	{
		SortByName,
		SortBySize
	};

	enum DisplayRore 
	{
		TexturePath = Qt::UserRole,
		TextureName,
		TextureDimension,
		TextureDataSize,
	};

	TextureListModel(QObject *parent = 0);

	void setScene(DAVA::Scene *scene);
	void setFilter(QString filter);
	void setSortMode(TextureListModel::TextureListSortMode sortMode);

	int rowCount(const QModelIndex &parent = QModelIndex()) const;

	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

	DAVA::Texture* getTexture(const QModelIndex &index) const;
	void dataReady(const DAVA::Texture *texture);

private:
	DAVA::Scene *scene;
	QVector<DAVA::Texture *> texturesAll;
	QVector<DAVA::Texture *> texturesFiltredSorted;

	TextureListSortMode curSortMode;
	QString	curFilter;

	void applyFilterAndSort();

	void searchTexturesInMaterial(DAVA::Material *material);
	void searchTexturesInNodes(DAVA::SceneNode *parentNode);
	void addTexture(DAVA::Texture *texture);

	static bool sortFnByName(const DAVA::Texture* t1, const DAVA::Texture* t2);
	static bool sortFnBySize(const DAVA::Texture* t1, const DAVA::Texture* t2);
};

#endif // __TEXTURE_LIST_MODEL_H__
