/*==================================================================================
    Copyright (c) 2008, binaryzebra
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
    * Neither the name of the binaryzebra nor the
    names of its contributors may be used to endorse or promote products
    derived from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE binaryzebra AND CONTRIBUTORS "AS IS" AND
    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL binaryzebra BE LIABLE FOR ANY
    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
=====================================================================================*/


#include "Render/Highlevel/Vegetation/VegetationCustomGeometrySerializationData.h"

#include "Scene3D/Components/RenderComponent.h"
#include "Scene3D/Components/ComponentHelpers.h"

namespace DAVA
{

static const FastName VEGETATION_ENTITY_VARIATION_0("variation_0");

static const FastName VEGETATION_ENTITY_LAYER_0("layer_0");
static const FastName VEGETATION_ENTITY_LAYER_1("layer_1");
static const FastName VEGETATION_ENTITY_LAYER_2("layer_2");
static const FastName VEGETATION_ENTITY_LAYER_3("layer_3");

static FastName VEGETATION_ENTITY_LAYER_NAMES[] =
{
    VEGETATION_ENTITY_LAYER_0,
    VEGETATION_ENTITY_LAYER_1,
    VEGETATION_ENTITY_LAYER_2,
    VEGETATION_ENTITY_LAYER_3
};


VegetationCustomGeometrySerializationData::VegetationCustomGeometrySerializationData(Vector<NMaterial*>& materialsData,
                                                                                     Vector<Vector<Vector<Vector3> > >& positionLods,
                                                                                     Vector<Vector<Vector<Vector2> > >& texCoordLods,
                                                                                     Vector<Vector<Vector<Vector3> > >& normalLods,
                                                                                     Vector<Vector<Vector<VegetationIndex> > >& indexLods)
{
    Load(materialsData, positionLods, texCoordLods, normalLods, indexLods);
}

VegetationCustomGeometrySerializationData::VegetationCustomGeometrySerializationData(VegetationCustomGeometrySerializationData& src)
{
    Load(src.materials, src.positions, src.texCoords, src.normals, src.indices);
}

VegetationCustomGeometrySerializationData::~VegetationCustomGeometrySerializationData()
{
    size_t materialCount = materials.size();
    for(size_t materialIndex = 0; materialIndex < materialCount; ++materialIndex)
    {
        SafeRelease(materials[materialIndex]);
    }
}

void VegetationCustomGeometrySerializationData::Load(Vector<NMaterial*>& materialsData,
                                                     Vector<Vector<Vector<Vector3> > >& positionLods,
                                                     Vector<Vector<Vector<Vector2> > >& texCoordLods,
                                                     Vector<Vector<Vector<Vector3> > >& normalLods,
                                                     Vector<Vector<Vector<VegetationIndex> > >& indexLods)
{
    size_t materialCount = materialsData.size();
    for(size_t layerIndex = 0; layerIndex < materialCount; ++layerIndex)
    {
        NMaterial* curMaterial = SafeRetain(materialsData[layerIndex]);
        materials.push_back(curMaterial);
        
        Vector<Vector<Vector3> >& srcPositionLodData = positionLods[layerIndex];
        Vector<Vector<Vector2> >& srcTexCoordLodData = texCoordLods[layerIndex];
        Vector<Vector<Vector3> >& srcNormalLodData = normalLods[layerIndex];
        Vector<Vector<VegetationIndex> >& srcIndexLodData = indexLods[layerIndex];
        
        size_t maxLodCount = Max(
                                 Max(srcPositionLodData.size(), srcTexCoordLodData.size()),
                                 Max(srcNormalLodData.size(), srcIndexLodData.size())
                                 );
        
        DVASSERT(srcPositionLodData.size() == maxLodCount);
        DVASSERT(srcTexCoordLodData.size() == maxLodCount);
        DVASSERT(srcNormalLodData.size() == maxLodCount);
        DVASSERT(srcIndexLodData.size() == maxLodCount);
        
        positions.push_back(Vector<Vector<Vector3> >());
        texCoords.push_back(Vector<Vector<Vector2> >());
        normals.push_back(Vector<Vector<Vector3> >());
        indices.push_back(Vector<Vector<VegetationIndex> >());
        
        Vector<Vector<Vector3> >& dstPositionLodData = positions[positions.size() - 1];
        Vector<Vector<Vector2> >& dstTexCoordLodData = texCoords[texCoords.size() - 1];
        Vector<Vector<Vector3> >& dstNormalLodData = normals[normals.size() - 1];
        Vector<Vector<VegetationIndex> >& dstIndexLodData = indices[indices.size() - 1];
        
        for(size_t lodIndex = 0; lodIndex < maxLodCount; ++lodIndex)
        {
            Vector<Vector3>& srcPositions = srcPositionLodData[lodIndex];
            Vector<Vector2>& srcTexCoords = srcTexCoordLodData[lodIndex];
            Vector<Vector3>& srcNormals = srcNormalLodData[lodIndex];
            Vector<VegetationIndex>& srcIndices = srcIndexLodData[lodIndex];
            
            dstPositionLodData.push_back(Vector<Vector3>());
            dstTexCoordLodData.push_back(Vector<Vector2>());
            dstNormalLodData.push_back(Vector<Vector3>());
            dstIndexLodData.push_back(Vector<VegetationIndex>());
            
            Vector<Vector3>& dstPositions = dstPositionLodData[dstPositionLodData.size() - 1];
            Vector<Vector2>& dstTexCoords = dstTexCoordLodData[dstTexCoordLodData.size() - 1];
            Vector<Vector3>& dstNormals = dstNormalLodData[dstNormalLodData.size() - 1];
            Vector<VegetationIndex>& dstIndices = dstIndexLodData[dstIndexLodData.size() - 1];
            
            size_t positionCount = srcPositions.size();
            for(size_t positionIndex = 0; positionIndex < positionCount; ++positionIndex)
            {
                dstPositions.push_back(srcPositions[positionIndex]);
            }
            
            size_t texCoordCount = srcTexCoords.size();
            for(size_t texCoordIndex = 0; texCoordIndex < texCoordCount; ++texCoordIndex)
            {
                dstTexCoords.push_back(srcTexCoords[texCoordIndex]);
            }
            
            size_t normalCount = srcNormals.size();
            for(size_t normalIndex = 0; normalIndex < normalCount; ++normalIndex)
            {
                dstNormals.push_back(srcNormals[normalIndex]);
            }
            
            size_t indexCount = srcIndices.size();
            for(size_t indexIndex = 0; indexIndex < indexCount; ++indexIndex)
            {
                dstIndices.push_back(srcIndices[indexIndex]);
            }
        }
    }
}

uint32 VegetationCustomGeometrySerializationData::GetLayerCount() const
{
    return static_cast<uint32>(materials.size());
}

uint32 VegetationCustomGeometrySerializationData::GetLodCount(uint32 layerIndex) const
{
    return static_cast<uint32>(positions[layerIndex].size());
}

NMaterial* VegetationCustomGeometrySerializationData::GetMaterial(uint32 layerIndex)
{
    return materials[layerIndex];
}

Vector<Vector3>& VegetationCustomGeometrySerializationData::GetPositions(uint32 layerIndex, uint32 lodIndex)
{
    return positions[layerIndex][lodIndex];
}

Vector<Vector2>& VegetationCustomGeometrySerializationData::GetTextureCoords(uint32 layerIndex, uint32 lodIndex)
{
    return texCoords[layerIndex][lodIndex];
}

Vector<Vector3>& VegetationCustomGeometrySerializationData::GetNormals(uint32 layerIndex, uint32 lodIndex)
{
    return normals[layerIndex][lodIndex];
}

Vector<VegetationIndex>& VegetationCustomGeometrySerializationData::GetIndices(uint32 layerIndex, uint32 lodIndex)
{
    return indices[layerIndex][lodIndex];
}

VegetationCustomGeometrySerializationDataPtr VegetationCustomGeometrySerializationDataReader::ReadScene(const FilePath& scenePath)
{
    VegetationCustomGeometrySerializationDataPtr result;

    ScopedPtr<Scene> scene(new Scene);
    Entity* sceneRootNode = scene->GetRootNode(scenePath);
    
    if (!sceneRootNode)
    {
        Logger::Error("Cannot load custom geometry scene specified");
        return result;
    }
    
    Vector<NMaterial*> materialsData;
    Vector<Vector<Vector<Vector3> > > positionLods;
    Vector<Vector<Vector<Vector2> > > texCoordLods;
    Vector<Vector<Vector<Vector3> > > normalLods;
    Vector<Vector<Vector<VegetationIndex> > > indexLods;
    
    //VI: wait for all scene objects to initialize
    //VI: in order to avoid crashes when scene released
    JobManager::Instance()->WaitMainJobs();

    Entity* currentVariation = sceneRootNode->FindByName(VEGETATION_ENTITY_VARIATION_0);
    if (!currentVariation)
    {
        Logger::Error("Invalid scene structure: variations!");
        return result;
    }

    uint32 layerCount = COUNT_OF(VEGETATION_ENTITY_LAYER_NAMES);
    for (uint32 layerIndex = 0; layerIndex < layerCount; ++layerIndex)
    {
        Entity* layerEntity = currentVariation->FindByName(VEGETATION_ENTITY_LAYER_NAMES[layerIndex]);
        if (!layerEntity)
        {
            Logger::Error("Invalid scene structure: layers!");
            return result;
        }

        positionLods.push_back(Vector<Vector<Vector3> >());
        texCoordLods.push_back(Vector<Vector<Vector2> >());
        normalLods.push_back(Vector<Vector<Vector3> >());
        indexLods.push_back(Vector<Vector<VegetationIndex> >());

        Vector<Vector<Vector3> >& layerPositions = positionLods[positionLods.size() - 1];
        Vector<Vector<Vector2> >& layerTexCoords = texCoordLods[texCoordLods.size() - 1];
        Vector<Vector<Vector3> >& layerNormals = normalLods[normalLods.size() - 1];
        Vector<Vector<VegetationIndex> >& layerIndices = indexLods[indexLods.size() - 1];

        RenderObject* ro = GetRenderObject(layerEntity);
        if (!ro)
        {
            Logger::Error("Invalid scene structure: no render object!");
            return result;
        }

        uint32 renderBatchCount = ro->GetRenderBatchCount();
        if (!ro->GetRenderBatchCount())
        {
            Logger::Error("Invalid scene structure: no render batches!");
            return result;
        }

        NMaterial* parentMaterial = ro->GetRenderBatch(0)->GetMaterial()->GetParent();

        materialsData.push_back(parentMaterial);


        for (uint32 resolutionIndex = 0;
            resolutionIndex < renderBatchCount;
            resolutionIndex++)
        {
            RenderBatch* rb = ro->GetRenderBatch(resolutionIndex);
            DVASSERT(rb);

            PolygonGroup* pg = rb->GetPolygonGroup();

            DVASSERT(pg);

            layerPositions.push_back(Vector<Vector3>());
            layerTexCoords.push_back(Vector<Vector2>());
            layerNormals.push_back(Vector<Vector3>());
            layerIndices.push_back(Vector<VegetationIndex>());

            Vector<Vector3>& positions = layerPositions[layerPositions.size() - 1];
            Vector<Vector2>& texCoords = layerTexCoords[layerTexCoords.size() - 1];
            Vector<Vector3>& normals = layerNormals[layerNormals.size() - 1];
            Vector<VegetationIndex>& indices = layerIndices[layerIndices.size() - 1];

            int32 vertexFormat = pg->GetFormat();
            DVASSERT((vertexFormat & EVF_VERTEX) != 0);

            uint32 vertexCount = pg->GetVertexCount();
            for (uint32 vertexIndex = 0;
                vertexIndex < vertexCount;
                ++vertexIndex)
            {
                Vector3 coord;
                pg->GetCoord(vertexIndex, coord);

                Vector3 normal;
                if ((vertexFormat & EVF_NORMAL) != 0)
                {
                    pg->GetNormal(vertexIndex, normal);
                }

                Vector2 texCoord;
                if ((vertexFormat & EVF_TEXCOORD0) != 0)
                {
                    pg->GetTexcoord(0, vertexIndex, texCoord);
                }

                positions.push_back(coord);
                normals.push_back(normal);
                texCoords.push_back(texCoord);
            }

            uint32 indexCount = pg->GetIndexCount();
            for (uint32 indexIndex = 0;
                indexIndex < indexCount;
                ++indexIndex)
            {
                int32 currentIndex = 0;
                pg->GetIndex(indexIndex, currentIndex);

                indices.push_back(currentIndex);
            }
        }
    }
    
    result.reset(new VegetationCustomGeometrySerializationData(
        materialsData,
        positionLods,
        texCoordLods,
        normalLods,
        indexLods));
    
    return result;
}

}