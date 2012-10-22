#include "SceneValidator.h"
#include "ErrorNotifier.h"
#include "EditorSettings.h"
#include "SceneInfoControl.h"

#include "Render/LibPVRHelper.h"
#include "Render/TextureDescriptor.h"

#include "../Qt/QtUtils.h"

SceneValidator::SceneValidator()
{
    sceneTextureCount = 0;
    sceneTextureMemory = 0;

    infoControl = NULL;
    
    pathForChecking = String("");
}

SceneValidator::~SceneValidator()
{
    SafeRelease(infoControl);
}

void SceneValidator::ValidateScene(Scene *scene)
{
    errorMessages.clear();

    ValidateScene(scene, errorMessages);

    ShowErrors();
}

void SceneValidator::ValidateScene(Scene *scene, Set<String> &errorsLog)
{
    if(scene) 
    {
        ValidateSceneNode(scene, errorsLog);
        ValidateLodNodes(scene, errorsLog);
        
        for (Set<SceneNode*>::iterator it = emptyNodesForDeletion.begin(); it != emptyNodesForDeletion.end(); ++it)
        {
            SceneNode * node = *it;
            if (node->GetParent())
            {
                node->GetParent()->RemoveNode(node);
            }
        }
        for (Set<SceneNode*>::iterator it = emptyNodesForDeletion.begin(); it != emptyNodesForDeletion.end(); ++it)
        {
            SceneNode * node = *it;
            SafeRelease(node);
        }
        emptyNodesForDeletion.clear();
    }
    else 
    {
        errorsLog.insert(String("Scene in NULL!"));
    }
}

void SceneValidator::ValidateScales(Scene *scene, Set<String> &errorsLog)
{
	if(scene) 
	{
		ValidateScalesInternal(scene, errorsLog);
	}
	else 
	{
		errorsLog.insert(String("Scene in NULL!"));
	}
}

void SceneValidator::ValidateScalesInternal(SceneNode *sceneNode, Set<String> &errorsLog)
{
//  Basic algorithm is here
// 	Matrix4 S, T, R; //Scale Transpose Rotation
// 	S.CreateScale(Vector3(1.5, 0.5, 2.0));
// 	T.CreateTranslation(Vector3(100, 50, 20));
// 	R.CreateRotation(Vector3(0, 1, 0), 2.0);
// 
// 	Matrix4 t = R*S*T; //Calculate complex matrix
// 
//	//Calculate Scale components from complex matrix
// 	float32 sx = sqrt(t._00 * t._00 + t._10 * t._10 + t._20 * t._20);
// 	float32 sy = sqrt(t._01 * t._01 + t._11 * t._11 + t._21 * t._21);
// 	float32 sz = sqrt(t._02 * t._02 + t._12 * t._12 + t._22 * t._22);
// 	Vector3 sCalculated(sx, sy, sz);

	if(!sceneNode) return;

	const Matrix4 & t = sceneNode->GetLocalTransform();
	float32 sx = sqrt(t._00 * t._00 + t._10 * t._10 + t._20 * t._20);
	float32 sy = sqrt(t._01 * t._01 + t._11 * t._11 + t._21 * t._21);
	float32 sz = sqrt(t._02 * t._02 + t._12 * t._12 + t._22 * t._22);

	if ((!FLOAT_EQUAL(sx, 1.0f)) 
		|| (!FLOAT_EQUAL(sy, 1.0f))
		|| (!FLOAT_EQUAL(sz, 1.0f)))
	{
 		errorsLog.insert(Format("Node %s: has scale (%.3f, %.3f, %.3f) ! Re-design level.", sceneNode->GetName().c_str(), sx, sy, sz));
	}

	int32 count = sceneNode->GetChildrenCount();
	for(int32 i = 0; i < count; ++i)
	{
		ValidateScalesInternal(sceneNode->GetChild(i), errorsLog);
	}
}



void SceneValidator::ValidateSceneNode(SceneNode *sceneNode)
{
    errorMessages.clear();

    ValidateSceneNode(sceneNode, errorMessages);
    
    ShowErrors();
    
}

void SceneValidator::ValidateSceneNode(SceneNode *sceneNode, Set<String> &errorsLog)
{
    if(!sceneNode) return;
    
    int32 count = sceneNode->GetChildrenCount();
    for(int32 i = 0; i < count; ++i)
    {
        SceneNode *node = sceneNode->GetChild(i);
        MeshInstanceNode *mesh = dynamic_cast<MeshInstanceNode*>(node);
        if(mesh)
        {
            ValidateMeshInstance(mesh, errorsLog);
        }
        else 
        {
            LandscapeNode *landscape = dynamic_cast<LandscapeNode*>(node);
            if (landscape) 
            {
                ValidateLandscape(landscape, errorsLog);
            }
            else
            {
                ValidateSceneNode(node, errorsLog);
            }
        }
        
        KeyedArchive *customProperties = node->GetCustomProperties();
        if(customProperties->IsKeyExists("editor.referenceToOwner"))
        {
            String dataSourcePath = EditorSettings::Instance()->GetDataSourcePath();
            if(1 < dataSourcePath.length())
            {
                if('/' == dataSourcePath[0])
                {
                    dataSourcePath = dataSourcePath.substr(1);
                }
                
                String referencePath = customProperties->GetString("editor.referenceToOwner");
                String::size_type pos = referencePath.rfind(dataSourcePath);
                if((String::npos != pos) && (1 != pos))
                {
                    referencePath.replace(pos, dataSourcePath.length(), "");
                    customProperties->SetString("editor.referenceToOwner", referencePath);
                    
                    errorsLog.insert(Format("Node %s: referenceToOwner isn't correct. Re-save level.", node->GetName().c_str()));
                }
            }
        }
    }
    
    if(typeid(SceneNode) == typeid(*sceneNode))
    {
        Set<DataNode*> dataNodeSet;
        sceneNode->GetDataNodes(dataNodeSet);
        if (dataNodeSet.size() == 0)
        {
            if(NodeRemovingDisabled(sceneNode))
            {
                return;
            }
            
            SceneNode * parent = sceneNode->GetParent();
            if (parent)
            {
                emptyNodesForDeletion.insert(SafeRetain(sceneNode));
            }
        }
    }
}

bool SceneValidator::NodeRemovingDisabled(SceneNode *node)
{
    KeyedArchive *customProperties = node->GetCustomProperties();
    return (customProperties && customProperties->IsKeyExists("editor.donotremove"));
}


void SceneValidator::ValidateTexture(Texture *texture)
{
    errorMessages.clear();

    ValidateTexture(texture, errorMessages);

    ShowErrors();
}

void SceneValidator::ValidateTexture(Texture *texture, Set<String> &errorsLog)
{
    if(!texture) return;

    bool pathIsCorrect = ValidatePathname(texture->GetPathname());
    if(!pathIsCorrect)
    {
        String path = FileSystem::AbsoluteToRelativePath(EditorSettings::Instance()->GetDataSourcePath(), texture->GetPathname());
        errorsLog.insert("Wrong path of: " + path);
    }
    if(!IsPowerOf2(texture->GetWidth()) || !IsPowerOf2(texture->GetHeight()))
    {
        String path = FileSystem::AbsoluteToRelativePath(EditorSettings::Instance()->GetDataSourcePath(), texture->GetPathname());
        errorsLog.insert("Wrong size of " + path);
    }
    
	// if there is no descriptor file for this texture - generate it
	if(pathIsCorrect)
	{
		CreateDescriptorIfNeed(texture->GetPathname());
	}
}

void SceneValidator::ValidateLandscape(LandscapeNode *landscape)
{
    errorMessages.clear();
    
    ValidateLandscape(landscape, errorMessages);
    
    ShowErrors();
}

void SceneValidator::ValidateLandscape(LandscapeNode *landscape, Set<String> &errorsLog)
{
    if(!landscape) return;
    
    for(int32 i = 0; i < LandscapeNode::TEXTURE_COUNT; ++i)
    {
        if(LandscapeNode::TEXTURE_DETAIL == (LandscapeNode::eTextureLevel)i)
        {
            continue;
        }

		// TODO:
		// new texture path
		DAVA::String landTexName = landscape->GetTextureName((LandscapeNode::eTextureLevel)i);
		if(!IsTextureDescriptorPath(landTexName))
		{
			landTexName = ConvertTexturePathToDescriptorPath(landTexName);
			landscape->SetTextureName((LandscapeNode::eTextureLevel)i, landTexName);
		}
        
        ValidateTexture(landscape->GetTexture((LandscapeNode::eTextureLevel)i), errorsLog);
    }
    
    bool pathIsCorrect = ValidatePathname(landscape->GetHeightmapPathname());
    if(!pathIsCorrect)
    {
        String path = FileSystem::AbsoluteToRelativePath(EditorSettings::Instance()->GetDataSourcePath(), landscape->GetHeightmapPathname());
        errorsLog.insert("Wrong path of Heightmap: " + path);
    }
}

void SceneValidator::ShowErrors()
{
    if(0 < errorMessages.size())
    {
        ErrorNotifier::Instance()->ShowError(errorMessages);
    }
}

void SceneValidator::ValidateMeshInstance(MeshInstanceNode *meshNode, Set<String> &errorsLog)
{
    meshNode->RemoveFlag(SceneNode::NODE_INVALID);
    
    const Vector<PolygonGroupWithMaterial*> & polygroups = meshNode->GetPolygonGroups();
    //Vector<Material *>materials = meshNode->GetMaterials();
    for(int32 iMat = 0; iMat < (int32)polygroups.size(); ++iMat)
    {
        Material * material = polygroups[iMat]->GetMaterial();

        ValidateMaterial(material, errorsLog);

        if (material->Validate(polygroups[iMat]->GetPolygonGroup()) == Material::VALIDATE_INCOMPATIBLE)
        {
            meshNode->AddFlag(SceneNode::NODE_INVALID);
            errorsLog.insert(Format("Material: %s incompatible with node:%s.", material->GetName().c_str(), meshNode->GetFullName().c_str()));
            errorsLog.insert("For lightmapped objects check second coordinate set. For normalmapped check tangents, binormals.");
        }
    }
    
    int32 lightmapCont = meshNode->GetLightmapCount();
    for(int32 iLight = 0; iLight < lightmapCont; ++iLight)
    {
        ValidateTexture(meshNode->GetLightmapDataForIndex(iLight)->lightmap, errorsLog);
    }
}


void SceneValidator::ValidateMaterial(Material *material)
{
    errorMessages.clear();

    ValidateMaterial(material, errorMessages);

    ShowErrors();
}

void SceneValidator::ValidateMaterial(Material *material, Set<String> &errorsLog)
{
    for(int32 iTex = 0; iTex < Material::TEXTURE_COUNT; ++iTex)
    {
        Texture *texture = material->GetTexture((Material::eTextureLevel)iTex);
        if(texture)
        {
            ValidateTexture(texture, errorsLog);
            
            // TODO:
            // new texture path
            String matTexName = material->GetTextureName((Material::eTextureLevel)iTex);
            if(!IsTextureDescriptorPath(matTexName))
            {
                matTexName = ConvertTexturePathToDescriptorPath(matTexName);
                material->SetTexture((Material::eTextureLevel)iTex, matTexName);
            }
            
            /*
             if(material->GetTextureName((Material::eTextureLevel)iTex).find(".pvr.png") != String::npos)
             {
             errorsLog.insert(material->GetName() + ": wrong texture name " + material->GetTextureName((Material::eTextureLevel)iTex));
             }
             */
        }
    }
}

void SceneValidator::EnumerateSceneTextures()
{
    sceneTextureCount = 0;
    sceneTextureMemory = 0;
    
    const Map<String, Texture*> textureMap = Texture::GetTextureMap();
    KeyedArchive *settings = EditorSettings::Instance()->GetSettings(); 
    String projectPath = settings->GetString("ProjectPath");
	for(Map<String, Texture *>::const_iterator it = textureMap.begin(); it != textureMap.end(); ++it)
	{
		Texture *t = it->second;
        if(String::npos != t->relativePathname.find(projectPath))
        {
            String::size_type pvrPos = t->relativePathname.find(".pvr");
            if(String::npos != pvrPos)
            {   //We need real info about textures size. In Editor on desktop pvr textures are decompressed to RGBA8888, so they have not real size.
                sceneTextureMemory += LibPVRHelper::GetDataLength(t->relativePathname);
            }
            else 
            {
                sceneTextureMemory += t->GetDataSize();
            }
            
            ++sceneTextureCount;
        }
	}
    
    if(infoControl)
    {
        infoControl->InvalidateTexturesInfo(sceneTextureCount, sceneTextureMemory);
    }
}

void SceneValidator::SetInfoControl(SceneInfoControl *newInfoControl)
{
    SafeRelease(infoControl);
    infoControl = SafeRetain(newInfoControl);
    
    sceneStats.Clear();
}

void SceneValidator::CollectSceneStats(const RenderManager::Stats &newStats)
{
    sceneStats = newStats;
    infoControl->SetRenderStats(sceneStats);
}

void SceneValidator::ReloadTextures()
{
    const Map<String, Texture*> textureMap = Texture::GetTextureMap();
	for(Map<String, Texture *>::const_iterator it = textureMap.begin(); it != textureMap.end(); ++it)
	{
		Texture *texture = it->second;
        
        if(WasTextureChanged(texture))
        {
            //TODO: need correct code for different formates
            Image *image = Image::CreateFromFile(texture->relativePathname, false);
            if(image)
            {
                texture->TexImage(0, image->GetWidth(), image->GetHeight(), image->GetData(), 0);
                SafeRelease(image);
            }
        }
	}
}


void SceneValidator::FindTexturesForCompression()
{
    List<Texture *>texturesForCompression;
    
    const Map<String, Texture*> textureMap = Texture::GetTextureMap();
	for(Map<String, Texture *>::const_iterator it = textureMap.begin(); it != textureMap.end(); ++it)
	{
		Texture *texture = it->second;
        if(WasTextureChanged(texture))
        {
            texturesForCompression.push_back(SafeRetain(texture));
        }
	}

    List<Texture *>::const_iterator endIt = texturesForCompression.end();
	for(List<Texture *>::const_iterator it = texturesForCompression.begin(); it != endIt; ++it)
	{
		Texture *texture = *it;
        //TODO: compress texture
        
        SafeRelease(texture);
	}
    texturesForCompression.clear();
}

bool SceneValidator::WasTextureChanged(Texture *texture)
{
    if(!texture->isRenderTarget)
    {
        String texturePathname = texture->GetPathname();
        return (IsPathCorrectForProject(texturePathname) && IsTextureChanged(texturePathname));
    }
    
    return false;
}



void SceneValidator::ValidateLodNodes(Scene *scene, Set<String> &errorsLog)
{
    Vector<LodNode *> lodnodes;
    scene->GetChildNodes(lodnodes); 
    
    for(int32 index = 0; index < (int32)lodnodes.size(); ++index)
    {
        LodNode *ln = lodnodes[index];
        
        int32 layersCount = ln->GetLodLayersCount();
        for(int32 layer = 0; layer < layersCount; ++layer)
        {
            float32 distance = ln->GetLodLayerDistance(layer);
            if(LodNode::INVALID_DISTANCE == distance)
            {
                ln->SetLodLayerDistance(layer, ln->GetDefaultDistance(layer));
                errorsLog.insert(Format("Node %s: lod distances weren't correct. Re-save.", ln->GetName().c_str()));
            }
        }
        
        List<LodNode::LodData *>lodLayers;
        ln->GetLodData(lodLayers);
        
        List<LodNode::LodData *>::const_iterator endIt = lodLayers.end();
        int32 layer = 0;
        for(List<LodNode::LodData *>::iterator it = lodLayers.begin(); it != endIt; ++it, ++layer)
        {
            LodNode::LodData * ld = *it;
            
            if(ld->layer != layer)
            {
                ld->layer = layer;

                errorsLog.insert(Format("Node %s: lod layers weren't correct. Rename childs. Re-save.", ln->GetName().c_str()));
            }
        }
    }
}

String SceneValidator::SetPathForChecking(const String &pathname)
{
    String oldPath = pathForChecking;
    pathForChecking = pathname;
    return pathForChecking;
}


bool SceneValidator::ValidateTexturePathname(const String &pathForValidation, Set<String> &errorsLog)
{
	DVASSERT(!pathForChecking.empty() && "Need to set pathname for DataSource folder");

	bool pathIsCorrect = IsPathCorrectForProject(pathForValidation);
	if(pathIsCorrect)
	{
		String textureExtension = FileSystem::Instance()->GetExtension(pathForValidation);
		String::size_type extPosition = GetTextureFileExtensions().find(textureExtension);
		if(String::npos == extPosition)
		{
			errorsLog.insert(Format("Path %s has incorrect extension", pathForValidation.c_str()));
			return false;
		}

		CreateDescriptorIfNeed(pathForValidation);
	}
	else
	{
		errorsLog.insert(Format("Path %s is incorrect for project %s", pathForValidation.c_str(), pathForChecking.c_str()));
	}

	return pathIsCorrect;
}

bool SceneValidator::ValidateHeightmapPathname(const String &pathForValidation, Set<String> &errorsLog)
{
	DVASSERT(!pathForChecking.empty() && "Need to set pathname for DataSource folder");

	bool pathIsCorrect = IsPathCorrectForProject(pathForValidation);
	if(pathIsCorrect)
	{
		String::size_type posPng = pathForValidation.find(".png");
		String::size_type posHeightmap = pathForValidation.find(Heightmap::FileExtension());
		return ((String::npos != posPng) || (String::npos != posHeightmap));
	}
	else
	{
		errorsLog.insert(Format("Path %s is incorrect for project %s", pathForValidation.c_str(), pathForChecking.c_str()));
	}

	return pathIsCorrect;
}


void SceneValidator::CreateDescriptorIfNeed(const String &forPathname)
{
	String descriptorPathname = FileSystem::Instance()->ReplaceExtension(forPathname, TextureDescriptor::GetDefaultExtension());
	bool fileExists = FileSystem::Instance()->IsFile(descriptorPathname);
	if(!fileExists)
	{
		Logger::Warning("[SceneValidator::CreateDescriptorIfNeed] Need descriptor for file %s", forPathname.c_str());
	
		TextureDescriptor *descriptor = new TextureDescriptor();
		descriptor->textureFileFormat = Texture::PNG_FILE;
		descriptor->Save(descriptorPathname);
	}
}


#include "FuckingErrorDialog.h"
bool SceneValidator::ValidatePathname(const String &pathForValidation)
{
    DVASSERT(0 < pathForChecking.length()); 
    //Need to set path to DataSource/3d for path correction  
    //Use SetPathForChecking();
    
    String::size_type fboFound = pathForValidation.find(String("FBO"));
    String::size_type resFound = pathForValidation.find(String("~res:"));
    if((String::npos != fboFound) || (String::npos != resFound))
    {
        return true;   
    }
    
    bool pathIsCorrect = IsPathCorrectForProject(pathForValidation);
    if(!pathIsCorrect)
    {
        UIScreen *screen = UIScreenManager::Instance()->GetScreen();
        
        FuckingErrorDialog *dlg = new FuckingErrorDialog(screen->GetRect(), String("Wrong path: ") + pathForValidation);
        screen->AddControl(dlg);
        SafeRelease(dlg);
    }
    
    return pathIsCorrect;
}

bool SceneValidator::IsPathCorrectForProject(const String pathname)
{
    String normalizedPath = FileSystem::NormalizePath(pathname);
    String::size_type foundPos = normalizedPath.find(pathForChecking);
    return (String::npos != foundPos);
}


void SceneValidator::EnumerateNodes(DAVA::Scene *scene)
{
    int32 nodesCount = 0;
    if(scene)
    {
        for(int32 i = 0; i < scene->GetChildrenCount(); ++i)
        {
            nodesCount += EnumerateSceneNodes(scene->GetChild(i));
        }
    }
    
    if(infoControl)
        infoControl->SetNodesCount(nodesCount);
}

int32 SceneValidator::EnumerateSceneNodes(DAVA::SceneNode *node)
{
    //TODO: lode node can have several nodes at layer
    
    int32 nodesCount = 1;
    for(int32 i = 0; i < node->GetChildrenCount(); ++i)
    {
        nodesCount += EnumerateSceneNodes(node->GetChild(i));
    }
    
    return nodesCount;
}


bool SceneValidator::IsTextureChanged(const String &texturePathname)
{
    TextureDescriptor *descriptor = Texture::CreateDescriptorForTexture(texturePathname);
    if(descriptor)
    {
        String sourceTexturePathname = FileSystem::Instance()->ReplaceExtension(texturePathname, ".png");
        const char8 *modificationDate = File::GetModificationDate(sourceTexturePathname);
        
        if(modificationDate && (0 != CompareStrings(String(modificationDate), String(descriptor->modificationDate))))
        {
            uint8 crc[MD5::DIGEST_SIZE];
            MD5::ForFile(sourceTexturePathname, crc);
            
            int32 cmpResult = Memcmp(crc, descriptor->crc, MD5::DIGEST_SIZE * sizeof(uint8));
            SafeRelease(descriptor);
            return (0 != cmpResult);
        }
            
        SafeRelease(descriptor);
    }
    return false;
}

bool SceneValidator::IsTextureDescriptorPath(const String &path)
{
	String ext = FileSystem::GetExtension(path);
	return (ext == TextureDescriptor::GetDefaultExtension());
}

String SceneValidator::ConvertTexturePathToDescriptorPath(const String &path)
{
	return FileSystem::ReplaceExtension(path, TextureDescriptor::GetDefaultExtension());
}
