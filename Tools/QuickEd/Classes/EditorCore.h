#pragma once

#include "Base/Introspection.h"
#include "Project/Project.h"
#include "Base/BaseTypes.h"
#include "Base/Singleton.h"
#include "AssetCache/AssetCacheClient.h"
#include "UI/mainwindow.h"
#include <QObject>

class QAction;
class Document;
class DocumentGroup;
class Project;
class PackageNode;
class SpritesPacker;

namespace DAVA
{
class AssetCacheClient;
}

class EditorCore : public QObject, public DAVA::Singleton<EditorCore>, public DAVA::InspBase
{
    Q_OBJECT
public:
    explicit EditorCore(QObject* parent = nullptr);
    ~EditorCore();
    MainWindow* GetMainWindow() const;
    //Project* GetProject() const;
    void Start();

private slots:
    bool CloseProject();
    void OnReloadSpritesStarted();
    void OnReloadSpritesFinished();

    void OnProjectPathChanged(const QString& path);
    void OnGLWidgedInitialized();

    void RecentMenu(QAction*);

    void UpdateLanguage();

    void OnRtlChanged(bool isRtl);
    void OnBiDiSupportChanged(bool support);
    void OnGlobalStyleClassesChanged(const QString& classesStr);

    void OnExit();
    void OnNewProject();
    void OnProjectOpenChanged(bool arg);

private:
    void OpenProject(const QString& path);

    bool IsUsingAssetCache() const;
    void SetUsingAssetCacheEnabled(bool enabled);

    void EnableCacheClient();
    void DisableCacheClient();

    void OnProjectOpen(const Project* project);
    void OnProjectClose(const Project* project);

    QStringList GetProjectsHistory() const;

    std::unique_ptr<SpritesPacker> spritesPacker;
    std::unique_ptr<DAVA::AssetCacheClient> cacheClient;

    std::unique_ptr<Project> project;
    DocumentGroup* documentGroup = nullptr;
    std::unique_ptr<MainWindow> mainWindow;

    DAVA::AssetCacheClient::ConnectionParams connectionParams;
    bool assetCacheEnabled;

    DAVA::String projectsHistory;
    DAVA::uint32 projectsHistorySize;

public:
    INTROSPECTION(EditorCore,
                  PROPERTY("isUsingAssetCache", "Asset cache/Use asset cache", IsUsingAssetCache, SetUsingAssetCacheEnabled, DAVA::I_PREFERENCE)
                  MEMBER(projectsHistory, "ProjectInternal/ProjectsHistory", DAVA::I_SAVE | DAVA::I_PREFERENCE)
                  //maximum size of projects history
                  MEMBER(projectsHistorySize, "Project/projects history size", DAVA::I_SAVE | DAVA::I_PREFERENCE)
                  )
};

// inline EditorFontSystem* GetEditorFontSystem()
// {
//     return EditorCore::Instance()->GetProject()->GetEditorFontSystem();
// }
