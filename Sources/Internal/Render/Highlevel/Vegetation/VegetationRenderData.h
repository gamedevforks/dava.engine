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


#ifndef __DAVAENGINE_VEGETATIONRENDERDATA_H__
#define __DAVAENGINE_VEGETATIONRENDERDATA_H__

#include "Base/BaseTypes.h"
#include "Base/BaseObject.h"
#include "Base/FastName.h"
#include "Render/RenderBase.h"
#include "Base/BaseMath.h"


namespace DAVA
{

typedef uint16 VegetationIndex;

struct VegetationVertex
{
    Vector3 coord;
    //Vector3 normal; uncomment, when normals will be used for vertex lit implementation
    Vector2 texCoord0;
    Vector3 texCoord1;
    Vector3 texCoord2;
};

/////////////////////////////////////////////////////////////////////////////////

/**
 \brief Represents a single geometry index buffer sorted by polygon for a specific
    camera direction.
 */
struct VegetationSortedBufferItem
{
    Vector3 sortDirection;

    uint32 startIndex;
    uint32 indexCount;
};

/////////////////////////////////////////////////////////////////////////////////

/**
 \brief Encapsulates vegetation layer parameters.
 */
struct VegetationLayerParams
{
    uint32 maxClusterCount;
    float32 instanceRotationVariation; //in angles
    float32 instanceScaleVariation; //0...1. "0" means no variation, "1" means variation from 0 to full scale
};

/////////////////////////////////////////////////////////////////////////////////

/**
 \brief Render data chunk. This chunk is generated by a VegetationGeometry
    subclass in a specific for the implementation way and then is used by the 
    render object to render data in a generic way.
 */
class VegetationRenderData
{
public:

    VegetationRenderData();
    ~VegetationRenderData();

    inline Vector<VegetationVertex>& GetVertices();
    inline Vector<VegetationIndex>& GetIndices();
    inline Vector<Vector<Vector<VegetationSortedBufferItem> > >& GetIndexBuffers();
    inline NMaterial* GetMaterial();
    inline void SetMaterial(NMaterial* mat);

    void ReleaseRenderData();
    
    Vector<Vector<uint32> > instanceCount; //layer - lod
    Vector<Vector<uint32> > vertexCountPerInstance; //layer - lod
    Vector<Vector<uint32> > polyCountPerInstance; //layer - lod

private:
    
    NMaterial* material;
    Vector<VegetationVertex> vertexData;
    Vector<VegetationIndex> indexData;
    Vector<Vector<Vector<VegetationSortedBufferItem> > > indexRenderDataObject; //resolution - cell - direction
};

/////////////////////////////////////////////////////////////////////////////////

inline Vector<VegetationVertex>& VegetationRenderData::GetVertices()
{
    return vertexData;
}

inline Vector<VegetationIndex>& VegetationRenderData::GetIndices()
{
    return indexData;
}

inline Vector<Vector<Vector<VegetationSortedBufferItem> > >& VegetationRenderData::GetIndexBuffers()
{
    return indexRenderDataObject;
}

inline NMaterial* VegetationRenderData::GetMaterial()
{
    return material;
}
    
inline void VegetationRenderData::SetMaterial(NMaterial* mat)
{
    if(mat != material)
    {
        SafeRelease(material);
        material = SafeRetain(mat);
    }
}

};

#endif /* defined(__Framework__VegetationRenderData__) */
