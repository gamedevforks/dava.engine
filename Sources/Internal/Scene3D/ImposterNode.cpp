/*==================================================================================
    Copyright (c) 2008, DAVA Consulting, LLC
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
    * Neither the name of the DAVA Consulting, LLC nor the
    names of its contributors may be used to endorse or promote products
    derived from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE DAVA CONSULTING, LLC AND CONTRIBUTORS "AS IS" AND
    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL DAVA CONSULTING, LLC BE LIABLE FOR ANY
    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
=====================================================================================*/

#include "ImposterNode.h"
#include "Utils/Utils.h"
#include "Render/Image.h"

namespace DAVA
{

REGISTER_CLASS(ImposterNode);

ImposterNode::ImposterNode(Scene * scene)
:	SceneNode(scene)
{
	state = STATE_3D;
	renderData = 0;
	fbo = Texture::CreateFBO(512, 512, FORMAT_RGBA4444, Texture::DEPTH_RENDERBUFFER);
}

ImposterNode::~ImposterNode()
{
	SafeRelease(fbo);
	SafeRelease(renderData);
}

void ImposterNode::Update(float32 timeElapsed)
{
	SceneNode::Update(timeElapsed);
}

void ImposterNode::Draw()
{
	if(GetChildrenCount() > 0)
	{
		DVASSERT(GetChildrenCount() == 1);
		AABBox3 bbox = GetChild(0)->GetWTMaximumBoundingBox();
		Vector3 bboxCenter = bbox.GetCenter();
		float32 distanceSquare = (scene->GetCurrentCamera()->GetPosition() - bboxCenter).SquareLength();
		distanceSquare *= scene->GetCurrentCamera()->GetZoomFactor() * scene->GetCurrentCamera()->GetZoomFactor();

		if((STATE_3D == state))
		{
			if(distanceSquare > 300)
			{
				UpdateImposter(false);
				state = STATE_IMPOSTER;
			}
			else
			{
				SceneNode::Draw();
			}
		}

		if(STATE_IMPOSTER == state)
		{
			Matrix4 modelViewMatrix = RenderManager::Instance()->GetMatrix(RenderManager::MATRIX_MODELVIEW); 
			const Matrix4 & cameraMatrix = scene->GetCurrentCamera()->GetMatrix();
			Matrix4 meshFinalMatrix;

			//Matrix4 inverse(Matrix4::IDENTITY);

			//inverse._00 = cameraMatrix._00;
			//inverse._01 = cameraMatrix._10;
			//inverse._02 = cameraMatrix._20;

			//inverse._10 = cameraMatrix._01;
			//inverse._11 = cameraMatrix._11;
			//inverse._12 = cameraMatrix._21;

			//inverse._20 = cameraMatrix._02;
			//inverse._21 = cameraMatrix._12;
			//inverse._22 = cameraMatrix._22;

			//meshFinalMatrix = inverse * worldTransform * modelViewMatrix;

			meshFinalMatrix = /*worldTransform **/ cameraMatrix;

			RenderManager::Instance()->SetMatrix(RenderManager::MATRIX_MODELVIEW, meshFinalMatrix);
			RenderManager::Instance()->SetRenderEffect(RenderManager::TEXTURE_MUL_FLAT_COLOR_ALPHA_TEST);

			RenderManager::Instance()->SetColor(1.f, 1.f, 1.f, 1.f);
			
			//RenderManager::Instance()->SetState(/*RenderStateBlock::STATE_BLEND |*/ RenderStateBlock::STATE_TEXTURE0/* | RenderStateBlock::STATE_CULL*/);
			RenderManager::Instance()->RemoveState(RenderStateBlock::STATE_CULL);
			//RenderManager::Instance()->RemoveState(RenderStateBlock::STATE_DEPTH_WRITE);
			//RenderManager::Instance()->AppendState(RenderStateBlock::STATE_BLEND);
			eBlendMode src = RenderManager::Instance()->GetSrcBlend();
			eBlendMode dst = RenderManager::Instance()->GetDestBlend();
			RenderManager::Instance()->SetBlendMode(BLEND_SRC_ALPHA, BLEND_ONE_MINUS_SRC_ALPHA);

			RenderManager::Instance()->SetTexture(fbo);

			RenderManager::Instance()->SetRenderData(renderData);

			RenderManager::Instance()->FlushState();
			RenderManager::Instance()->DrawArrays(PRIMITIVETYPE_TRIANGLESTRIP, 0, 4);

			//RenderManager::Instance()->AppendState(RenderStateBlock::STATE_DEPTH_WRITE);
			RenderManager::Instance()->SetState(RenderStateBlock::DEFAULT_3D_STATE);
			RenderManager::Instance()->SetBlendMode(src, dst);

			RenderManager::Instance()->SetMatrix(RenderManager::MATRIX_MODELVIEW, modelViewMatrix);

			if(distanceSquare < 300)
			{
				state = STATE_3D;
			}
			else
			{
				Vector3 newDirection = scene->GetCurrentCamera()->GetPosition()-center;
				newDirection.Normalize();
				if(newDirection.DotProduct(direction) < 0.996f)
				{
					UpdateImposter(false);
				}
				else
				{
					UpdateImposter(true);
				}
			}
		}
	}
}

void ImposterNode::UpdateImposter(bool onlyGeometry)
{
	SafeRelease(renderData);
	verts.clear();
	texCoords.clear();

	Camera * camera = scene->GetCurrentCamera();
	Camera * imposterCamera = new Camera();
	Vector3 cameraPos = camera->GetPosition();

	SceneNode * child = GetChild(0);
	AABBox3 bbox = child->GetWTMaximumBoundingBox();
	Vector3 bboxVertices[8];
	Vector3 screenVertices[8];
	bbox.GetCorners(bboxVertices);

	imposterCamera->Setup(90.f, 1.33f, 1.f, 1000.f);
	imposterCamera->SetTarget(bbox.GetCenter());
	imposterCamera->SetPosition(cameraPos);
	imposterCamera->SetUp(camera->GetUp());
	imposterCamera->SetLeft(camera->GetLeft());
	imposterCamera->Set();


	Rect viewport = RenderManager::Instance()->GetViewport();
	

	Matrix4 mvp = imposterCamera->GetUniformProjModelMatrix();

	AABBox3 screenBounds;
	for(int32 i = 0; i < 8; ++i)
	{
		//project
		Vector4 pv(bboxVertices[i]);
		pv = pv*mvp;
		pv.x = (viewport.dx * 0.5f) * (1.f + pv.x/pv.w)+viewport.x;
		pv.y = (viewport.dy * 0.5f) * (1.f + pv.y/pv.w)+viewport.y;
		pv.z = (pv.z/pv.w + 1.f) * 0.5f;

		screenVertices[i] = Vector3(pv.x, pv.y, pv.z);
		screenBounds.AddPoint(screenVertices[i]);
	}

	Vector3 screenBillboardVertices[4];
	screenBillboardVertices[0] = Vector3(screenBounds.min.x, screenBounds.min.y, screenBounds.min.z);
	screenBillboardVertices[1] = Vector3(screenBounds.max.x, screenBounds.min.y, screenBounds.min.z);
	screenBillboardVertices[2] = Vector3(screenBounds.min.x, screenBounds.max.y, screenBounds.min.z);
	screenBillboardVertices[3] = Vector3(screenBounds.max.x, screenBounds.max.y, screenBounds.min.z);

	center = Vector3();
	Matrix4 invMvp = mvp;
	invMvp.Inverse();
	for(int32 i = 0; i < 4; ++i)
	{
		//unproject
		Vector4 out;
		out.x = 2.f*(screenBillboardVertices[i].x-viewport.x)/viewport.dx-1.f;
		out.y = 2.f*(screenBillboardVertices[i].y-viewport.y)/viewport.dy-1.f;
		out.z = 2.f*screenBillboardVertices[i].z-1.f;
		out.w = 1.f;

		out = out*invMvp;
		DVASSERT(out.w != 0.f);

		out.x /= out.w;
		out.y /= out.w;
		out.z /= out.w;

		imposterVertices[i] = Vector3(out.x, out.y, out.z);
		center += imposterVertices[i];
	}
	center /= 4.f;

	if(!onlyGeometry)
	{
		direction = camera->GetPosition()-center;
		direction.Normalize();

		float32 nearPlane = (center-cameraPos).Length();
		float32 farPlane = nearPlane + (bbox.max.z-bbox.min.z);
		float32 w = (imposterVertices[1]-imposterVertices[0]).Length();
		float32 h = (imposterVertices[2]-imposterVertices[0]).Length();
		imposterCamera->Setup(-w/2.f, w/2.f, -h/2.f, h/2.f, nearPlane, nearPlane+50.f);

		Rect oldViewport = RenderManager::Instance()->GetViewport();

		RenderManager::Instance()->SetRenderTarget(fbo);
		imposterCamera->SetTarget(center);
		imposterCamera->Set();
		RenderManager::Instance()->ClearWithColor(1.f, 1.f, 1.f, 0.f);
		RenderManager::Instance()->ClearDepthBuffer();

		child->Draw();

#ifdef __DAVAENGINE_OPENGL__
		BindFBO(RenderManager::Instance()->fboViewFramebuffer);
#endif

		//Image * img = fbo->CreateImageFromMemory();
		//img->Save("imposter.png");
		RenderManager::Instance()->SetViewport(oldViewport, true);
		
	}

	camera->Set();
	SafeRelease(imposterCamera);

	verts.push_back(imposterVertices[0].x);
	verts.push_back(imposterVertices[0].y);
	verts.push_back(imposterVertices[0].z);

	verts.push_back(imposterVertices[1].x);
	verts.push_back(imposterVertices[1].y);
	verts.push_back(imposterVertices[1].z);

	verts.push_back(imposterVertices[2].x);
	verts.push_back(imposterVertices[2].y);
	verts.push_back(imposterVertices[2].z);

	verts.push_back(imposterVertices[3].x);
	verts.push_back(imposterVertices[3].y);
	verts.push_back(imposterVertices[3].z);

	texCoords.push_back(0);
	texCoords.push_back(0);

	texCoords.push_back(1);
	texCoords.push_back(0);

	texCoords.push_back(0);
	texCoords.push_back(1);

	texCoords.push_back(1);
	texCoords.push_back(1);

	renderData = new RenderDataObject();
	renderData->SetStream(EVF_VERTEX, TYPE_FLOAT, 3, 0, &(verts[0]));
	renderData->SetStream(EVF_TEXCOORD0, TYPE_FLOAT, 2, 0, &(texCoords[0]));
}

SceneNode* ImposterNode::Clone(SceneNode *dstNode /*= NULL*/)
{
	if (!dstNode) 
	{
		dstNode = new ImposterNode(scene);
	}

	SceneNode::Clone(dstNode);

	return dstNode;
}

}
