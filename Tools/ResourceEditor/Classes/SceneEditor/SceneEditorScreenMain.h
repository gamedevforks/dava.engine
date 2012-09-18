#ifndef __SCENE_EDITOR_SCREEN_MAIN_H__
#define __SCENE_EDITOR_SCREEN_MAIN_H__

#include "DAVAEngine.h"
#include "LibraryControl.h"
#include "MenuPopupControl.h"

#include "CreateNodesDialog.h"

#include "SettingsDialog.h"

#include "../Constants.h"



using namespace DAVA;

#if defined(DAVA_QT)
class ScenePreviewDialog;
#endif //#if defined(DAVA_QT)

class EditorBodyControl;
class MaterialEditor;
class SettingsDialog;
class TextureTrianglesDialog;
class TextureConverterDialog;
class HelpDialog;
class ParticlesEditorControl;
class SceneEditorScreenMain: 
    public UIScreen,
#if !defined(DAVA_QT)
    public UIFileSystemDialogDelegate,
    public LibraryControlDelegate,
    public MenuPopupDelegate,
#endif //#if !defined(DAVA_QT)
    public CreateNodesDialogDelegeate,
    public SettingsDialogDelegate
{

    enum eConst
    {        
        LINE_HEIGHT = 1,

        BODY_Y_OFFSET = 50,
        
        TAB_BUTTONS_OFFSET = 250,
    };

    enum DIALOG_OPERATION
    {
        DIALOG_OPERATION_NONE = 0,
        DIALOG_OPERATION_MENU_OPEN,
        DIALOG_OPERATION_MENU_SAVE,
        DIALOG_OPERATION_MENU_PROJECT,
        DIALOG_OPERATION_MENU_EXPORT,
    };
    
    enum eMenuIDS
    {
        MENUID_OPEN = 100,
        MENUID_CREATENODE = 200,
        MENUID_VIEWPORT = 300,
        MENUID_EXPORTTOGAME = 400,
    };
    
    
    enum eOpenMenuIDS
    {
        EOMID_OPEN = 0,
        EOMID_OPENLAST_STARTINDEX,
        
        EOMID_COUNT
    };

public:
    enum eLandscapeEditorModeIDS
    {
        ELEMID_HEIGHTMAP = 0,
        ELEMID_COLOR_MAP,
        
        ELEMID_COUNT
    };
    

public:
    
	SceneEditorScreenMain();

    struct BodyItem;


	virtual void LoadResources();
	virtual void UnloadResources();
	virtual void WillAppear();
	virtual void WillDisappear();
	
	virtual void Update(float32 timeElapsed);
	virtual void Draw(const UIGeometricData &geometricData);

#if defined (DAVA_QT)
    virtual void SetSize(const Vector2 &newSize);
#else //#if defined (DAVA_QT)
	virtual void OnEditSCE(const String &pathName, const String &name);
	virtual void OnAddSCE(const String &pathName);
	virtual void OnReloadSCE(const String &pathName);

    //menu
    virtual void MenuCanceled();
	virtual void MenuSelected(int32 menuID, int32 itemID);
    virtual WideString MenuItemText(int32 menuID, int32 itemID);
    virtual int32 MenuItemsCount(int32 menuID);
#endif //#if defined (DAVA_QT)

    // create node dialog
    virtual void DialogClosed(int32 retCode);

    void EditMaterial(Material *material);

	void EditParticleEmitter(ParticleEmitterNode * emitter);
    
    void ShowTextureTriangles(PolygonGroup *polygonGroup);

	BodyItem * FindCurrentBody();
    
    virtual void SettingsChanged();
    virtual void Input(UIEvent * event);

	struct BodyItem
	{
		UIButton *headerButton;
		UIButton *closeButton;
		EditorBodyControl *bodyControl;
	};
    
    void RecreteFullTilingTexture();

	ParticlesEditorControl * GetParticlesEditor() { return particlesEditor; }
    
#if defined (DAVA_QT)
    void SelectNodeQt(SceneNode *node);
    void OnReloadRootNodesQt();
    
    void ShowScenePreview(const String scenePathname);
    void HideScenePreview();

    bool LandscapeEditorModeEnabled();
    bool TileMaskEditorEnabled();
#endif //#if defined (DAVA_QT)
    
    void AddBodyItem(const WideString &text, bool isCloseable);
    

private:
    
    void InitControls();
    
    
    void AutoSaveLevel(BaseObject * obj, void *, void *);
    void SetupAnimation();
    
    void AddLineControl(Rect r);
    
    Vector<BodyItem *> bodies;
    
    void InitializeBodyList();
    void ReleaseBodyList();
    
    void OnSelectBody(BaseObject * owner, void * userData, void * callerData);
    void OnCloseBody(BaseObject * owner, void * userData, void * callerData);
    

#if !defined (DAVA_QT)
    //FileDialog
    UIFileSystemDialog * fileSystemDialog;
    uint32 fileSystemDialogOpMode;
    
    void OnFileSelected(UIFileSystemDialog *forDialog, const String &pathToFile);
    void OnFileSytemDialogCanceled(UIFileSystemDialog *forDialog);
    
    //menu
    void CreateTopMenu();
    void ReleaseTopMenu();
    
    UIButton * btnOpen;
    UIButton * btnSave;
    UIButton * btnExport;
    UIButton * btnMaterials;
    UIButton * btnCreate;
    UIButton * btnNew;
    UIButton * btnProject;
	UIButton * btnBeast;
	UIButton * btnLandscapeHeightmap;
	UIButton * btnLandscapeColor;
	UIButton * btnViewPortSize;
    UIButton * btnTextureConverter;
    
    
    void OnOpenPressed(BaseObject * obj, void *, void *);
    void OnSavePressed(BaseObject * obj, void *, void *);
    void OnExportPressed(BaseObject * obj, void *, void *);
    void OnMaterialsPressed(BaseObject * obj, void *, void *);
    void OnCreatePressed(BaseObject * obj, void *, void *);
    void OnNewPressed(BaseObject * obj, void *, void *);
    void OnOpenProjectPressed(BaseObject * obj, void *, void *);
	void OnBeastPressed(BaseObject * obj, void *, void *);
	void OnLandscapeHeightmapPressed(BaseObject * obj, void *, void *);
	void OnLandscapeColorPressed(BaseObject * obj, void *, void *);
    void OnViewPortSize(BaseObject * obj, void *, void *);
    void OnTextureConverter(BaseObject * obj, void *, void *);
    
    
    //Body list
    void OnBakeScene(BaseObject *, void *, void *);

    //SceneGraph
    UIButton *sceneGraphButton;
    void OnSceneGraphPressed(BaseObject * obj, void *, void *);
    
    //DataGraph
    UIButton *dataGraphButton;
    void OnDataGraphPressed(BaseObject * obj, void *, void *);
    
	//Entities
	UIButton *entitiesButton;
	void OnEntitiesPressed(BaseObject * obj, void *, void *);
    
    //Library
    UIButton *libraryButton;
    LibraryControl *libraryControl;
    void OnLibraryPressed(BaseObject * obj, void *, void *);
    
    UIButton *propertiesButton;
    void OnPropertiesPressed(BaseObject * obj, void *, void *);
    
    UIButton *sceneInfoButton;
    void OnSceneInfoPressed(BaseObject * obj, void *, void *);

    void OnSettingsPressed(BaseObject * obj, void *, void *);
    
    // menu
    MenuPopupControl *menuPopup;
    
    
    //Open menu
    void ShowOpenFileDialog();
    void ShowOpenLastDialog();

#endif //#if !defined (DAVA_QT)
    
    

    //create node dialog
    CreateNodesDialog *nodeDialog;
    
    MaterialEditor *materialEditor;
	ParticlesEditorControl * particlesEditor;
    void InitializeNodeDialogs();
    void ReleaseNodeDialogs();
    
    UIControl *dialogBack;

    SettingsDialog *settingsDialog;
    
    TextureTrianglesDialog *textureTrianglesDialog;
    TextureConverterDialog *textureConverterDialog;
    
    // general
    Font *font;
    
	bool useConvertedTextures;
    
    HelpDialog *helpDialog;
    
    void ReleaseResizedControl(UIControl *control);
    
public: //For Qt integration
    void OpenFileAtScene(const String &pathToFile);
    void NewScene();

    bool SaveIsAvailable();
    String CurrentScenePathname();
    void SaveSceneToFile(const String &pathToFile);
   

    void ExportAs(ResourceEditor::eExportFormat format);
    
    void CreateNode(ResourceEditor::eNodeType nodeType);
    void SetViewport(ResourceEditor::eViewportType viewportType);
    
    void MaterialsTriggered();
    void TextureConverterTriggered();
    void HeightmapTriggered();
    void TilemapTriggered();
    
    void ToggleSceneInfo();
    void ShowSettings();
    
    void ProcessBeast();
    
#if defined (DAVA_QT)
    ScenePreviewDialog *scenePreviewDialog;
#endif //#if defined (DAVA_QT)

    UIControl *focusedControl;
};

#endif // __SCENE_EDITOR_SCREEN_MAIN_H__