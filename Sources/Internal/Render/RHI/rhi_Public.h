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


#ifndef __RHI_PUBLIC_H__
#define __RHI_PUBLIC_H__

    #include "rhi_Type.h"


namespace rhi
{
////////////////////////////////////////////////////////////////////////////////
// base operation

void    Initialize( Api api );
void    Uninitialize();

void    Present(); // execute all submitted command-buffers & do flip/present

Api     HostApi();
bool    TextureFormatSupported( TextureFormat format );


////////////////////////////////////////////////////////////////////////////////
// vertex buffer

Handle  CreateVertexBuffer( uint32 size );
void    DeleteVertexBuffer( Handle vb );

void*   MapVertexBuffer( Handle vb, uint32 offset, uint32 size );
void    UnmapVertexBuffer( Handle vb );

void    UpdateVertexBuffer( Handle vb, const void* data, uint32 offset, uint32 size );


////////////////////////////////////////////////////////////////////////////////
// index buffer

Handle  CreateIndexBuffer( uint32 size );
void    DeleteIndexBuffer( Handle ib );

void*   MapIndexBuffer( Handle ib, uint32 offset, uint32 size );
void    UnmapIndexBuffer( Handle ib );

void    UpdateIndexBuffer( Handle ib, const void* data, uint32 offset, uint32 size );


////////////////////////////////////////////////////////////////////////////////
// render-pipeline state & const-buffers

Handle  AcquireRenderPipelineState( const PipelineState::Descriptor& desc );
void    ReleaseRenderPipelineState( Handle rps );

bool    CreateVertexConstBuffers( Handle rps, uint32 maxCount, Handle* constBuf );
bool    CreateFragmentConstBuffers( Handle rps, uint32 maxCount, Handle* constBuf );

bool    UpdateConstBuffer( Handle constBuf, uint32 constIndex, const float* data, uint32 constCount );


////////////////////////////////////////////////////////////////////////////////
// texture-set

Handle  CreateTexture( const Texture::Descriptor& desc );
void    DeleteTexture( Handle tex );

void*   MapTexture( Handle tex, uint32 level=0 );
void    UnmapTexture( Handle tex );

void    UpdateTexture( Handle tex, uint32 level, TextureFace face, const void* data );


struct
TextureSetDescriptor
{
    uint32  count;
    Handle  texture[MAX_TEXTURE_SAMPLER_COUNT];
};

Handle  AcquireTextureSet( const TextureSetDescriptor& desc );
Handle  CopyTextureSet( Handle ts );
void    ReleaseTextureSet( Handle ts );


////////////////////////////////////////////////////////////////////////////////
//  depthstencil-state

Handle  AcquireDepthStencilState( const DepthStencilState::Descriptor& desc );
Handle  CopyDepthStencilState( Handle ds );
void    ReleaseDepthStencilState( Handle ds );


////////////////////////////////////////////////////////////////////////////////
//  sampler-state

Handle  AcquireSamplerState( const SamplerState::Descriptor& desc );
Handle  CopySamplerState( Handle ss );
void    ReleaseSamplerState( Handle ss );



////////////////////////////////////////////////////////////////////////////////
// render-pass

Handle  AllocateRenderPass( const RenderPassConfig& passDesc, uint32 packetListCount, Handle* packetList );
void    BeginRenderPass( Handle pass );
void    EndRenderPass( Handle pass ); // no explicit render-pass 'release' needed


////////////////////////////////////////////////////////////////////////////////
// rendering

struct
Packet
{
    uint32          vertexStreamCount;
    Handle          vertexStream[MAX_VERTEX_STREAM_COUNT];
    uint32          vertexLayout;
    Handle          indexBuffer;
    Handle          renderPipelineState;
    Handle          depthStencilState;
    Handle          samplerState;
    uint32          vertexConstCount;
    Handle          vertexConst[MAX_CONST_BUFFER_COUNT];
    uint32          fragmentConstCount;
    Handle          fragmentConst[MAX_CONST_BUFFER_COUNT];
    Handle          textureSet;
    PrimitiveType   primitiveType;
    uint32          primitiveCount;

                    Packet()
                      : vertexStreamCount(0),
                        vertexLayout(VertexLayout::InvalidUID),
                        indexBuffer(InvalidHandle),
                        renderPipelineState(InvalidHandle),
                        depthStencilState(InvalidHandle),
                        samplerState(InvalidHandle),
                        vertexConstCount(0),
                        fragmentConstCount(0),
                        textureSet(InvalidHandle),
                        primitiveCount(0)
                    {}
};

void    BeginPacketList( Handle packetList );
void    AddPackets( Handle packetList, const Packet* packet, uint32 packetCount );
void    AddPacket( Handle packetList, const Packet& packet );
void    EndPacketList( Handle packetList ); // 'packetList' handle invalid after this, no explicit "release" needed



} // namespace rhi
#endif // __RHI_PUBLIC_H__

