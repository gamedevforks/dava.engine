#ifndef __RESOURCEEDITORQT__SELECTPATHWIDGET__
#define __RESOURCEEDITORQT__SELECTPATHWIDGET__

#include <QWidget>
#include <QMimeData>
#include <qlineedit.h>
#include <qtoolbutton.h>

#include "DAVAEngine.h"
class SceneEditor2;

class SelectPathWidgetBase : public QLineEdit
{
    Q_OBJECT

public:
    explicit SelectPathWidgetBase(QWidget* parent = 0, bool checkForProjectPath = false, DAVA::String openDialoDefualtPath = "", DAVA::String relativPath = "",
                                  DAVA::String openFileDialogTitle = "Open File", DAVA::String fileFormatDescriotion = "*.*");

    virtual ~SelectPathWidgetBase();

    void setText(const DAVA::String&);

    DAVA::String getText();

    virtual void EraseWidget();

    void SetAllowedFormatsList(const DAVA::List<DAVA::String>& _allowedFormatsList)
    {
        allowedFormatsList = _allowedFormatsList;
    }

    bool IsOpenButtonVisible() const;

    void SetOpenButtonVisible(bool value);

    bool IsClearButtonVisible() const;

    void SetClearButtonVisible(bool value);

    DAVA::String GetOpenDialogDefaultPath() const;

    void SetOpenDialogDefaultPath(const DAVA::FilePath& path);

    DAVA::String GetFileFormatFilter() const;

    void SetFileFormatFilter(const DAVA::String& filter);

public slots:

    void setText(const QString&);

    void acceptEditing();

    void setVisible(bool);

signals:

    void PathSelected(DAVA::String selectedFile);

protected:
    void resizeEvent(QResizeEvent*);

    virtual void Init(DAVA::String& _openDialogDefualtPath, DAVA::String& _relativPath, DAVA::String _openFileDialogTitle, DAVA::String _fileFormatDescriotion);

    virtual void HandlePathSelected(DAVA::String name);

    DAVA::String ConvertToRelativPath(const DAVA::String& path);

    QToolButton* CreateToolButton(const DAVA::String& iconPath);

    void dragEnterEvent(QDragEnterEvent* event);
    void dragMoveEvent(QDragMoveEvent* event);
    void dropEvent(QDropEvent* event);

    DAVA::FilePath relativePath;

    DAVA::String openDialogDefaultPath;

    DAVA::String fileFormatFilter; // like "Scene File (*.sc2)"

    DAVA::List<DAVA::String> allowedFormatsList;

    DAVA::String openFileDialogTitle;

    QMimeData mimeData;

    bool checkForProjectPath;

protected slots:

    void EraseClicked();

    void OpenClicked();

private:
    QToolButton* clearButton;
    QToolButton* openButton;
};

#endif /* defined(__RESOURCEEDITORQT__SELECTPATHWIDGET__) */