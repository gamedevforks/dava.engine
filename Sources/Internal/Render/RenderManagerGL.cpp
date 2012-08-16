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

    Revision History:
        * Created by Vitaliy Borodovsky 
=====================================================================================*/
#include "Render/RenderBase.h"
#include "Render/RenderManager.h"
#include "Render/Texture.h"
#include "Render/2D/Sprite.h"
#include "Utils/Utils.h"
#include "Core/Core.h"
#include "Render/OGLHelpers.h"
#include "Render/Shader.h"

#include "Render/Image.h"
#include "FileSystem/FileSystem.h"
#include "Utils/StringFormat.h"

#ifdef __DAVAENGINE_OPENGL__

namespace DAVA
{
	
#if defined(__DAVAENGINE_WIN32__)

static HDC hDC;
static HGLRC hRC;
static HWND hWnd;
static HINSTANCE hInstance;

bool RenderManager::Create(HINSTANCE _hInstance, HWND _hWnd)
{
	hInstance = _hInstance;
	hWnd = _hWnd;

	hDC = GetDC(hWnd);

	PIXELFORMATDESCRIPTOR pfd;
	ZeroMemory( &pfd, sizeof( pfd ) );
	pfd.nSize = sizeof( pfd );
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 24;
	pfd.cDepthBits = 24;
	pfd.cStencilBits = 8;
	pfd.iLayerType = PFD_MAIN_PLANE;
	int iFormat = ChoosePixelFormat( hDC, &pfd );
	SetPixelFormat( hDC, iFormat, &pfd );

	hRC = wglCreateContext(hDC);
	Thread::secondaryContext = wglCreateContext(hDC);
	Thread::currentDC = hDC;

	wglShareLists(Thread::secondaryContext, hRC);
	wglMakeCurrent(hDC, hRC);

	glewInit();

	DetectRenderingCapabilities();
	return true;
}

void RenderManager::Release()
{
	wglMakeCurrent(0, 0);
	wglDeleteContext(hRC);
	ReleaseDC(hWnd, hDC);	
	
	Singleton<RenderManager>::Release();
}

bool RenderManager::ChangeDisplayMode(DisplayMode mode, bool isFullscreen)
{
	hardwareState.Reset(false);
	currentState.Reset(true);
	return true;
}	
	
#else //#if defined(__DAVAENGINE_WIN32__)

bool RenderManager::Create()
{
	DetectRenderingCapabilities();
	return true;
}
	
void RenderManager::Release()
{
	
}

#endif //#if defined(__DAVAENGINE_WIN32__)

void RenderManager::DetectRenderingCapabilities()
{
#if defined(__DAVAENGINE_MACOS__)
	caps.isHardwareCursorSupported = true;
#else
	caps.isHardwareCursorSupported = false;
#endif
}

bool RenderManager::IsDeviceLost()
{
	return false;
}

void RenderManager::BeginFrame()
{
    stats.Clear();
    SetViewport(Rect(0, 0, -1, -1), true);
	
	SetRenderOrientation(Core::Instance()->GetScreenOrientation());
	DVASSERT(!currentRenderTarget);
	//DVASSERT(!currentRenderEffect);
	DVASSERT(clipStack.empty());
	DVASSERT(renderTargetStack.empty());
	DVASSERT(renderEffectStack.empty());
	Reset();
	isInsideDraw = true;
    ClearUniformMatrices();
}
	
//void RenderManager::SetDrawOffset(const Vector2 &offset)
//{
//	glMatrixMode(GL_PROJECTION);
//	glTranslatef(offset.x, offset.y, 0.0f);
//	glMatrixMode(GL_MODELVIEW);
//}

void RenderManager::PrepareRealMatrix()
{
    if (mappingMatrixChanged)
    {
        mappingMatrixChanged = false;
        Vector2 realDrawScale(viewMappingDrawScale.x * userDrawScale.x, viewMappingDrawScale.y * userDrawScale.y);
        Vector2 realDrawOffset(viewMappingDrawOffset.x + userDrawOffset.x * viewMappingDrawScale.x, viewMappingDrawOffset.y + userDrawOffset.y * viewMappingDrawScale.y);
	
	if (realDrawScale != currentDrawScale || realDrawOffset != currentDrawOffset) 
	{

		currentDrawScale = realDrawScale;
		currentDrawOffset = realDrawOffset;

        
        Matrix4 glTranslate, glScale;
        glTranslate.glTranslate(currentDrawOffset.x, currentDrawOffset.y, 0.0f);
        glScale.glScale(currentDrawScale.x, currentDrawScale.y, 1.0f);
        
        glTranslate = glScale * glTranslate;
        SetMatrix(MATRIX_MODELVIEW, glTranslate);
//        Logger::Info("2D matricies recalculated");
//        Matrix4 modelViewSave = RenderManager::Instance()->GetMatrix(RenderManager::MATRIX_MODELVIEW);
//        Logger::Info("Model matrix");
//        modelViewSave.Dump();
//        Matrix4 projectionSave = RenderManager::Instance()->GetMatrix(RenderManager::MATRIX_PROJECTION);
//        Logger::Info("Proj matrix");
//        projectionSave.Dump();
    }
}
}
	

void RenderManager::EndFrame()
{
	isInsideDraw = false;
#if defined(__DAVAENGINE_WIN32__)
	::SwapBuffers(hDC);
#endif //#if defined(__DAVAENGINE_WIN32__)
	
	RENDER_VERIFY("");	// verify at the end of the frame
    
    if(needGLScreenShot)
    {
        needGLScreenShot = false;
        MakeGLScreenShot();
    }
}
    
void RenderManager::MakeGLScreenShot()
{
    Logger::Debug("RenderManager::MakeGLScreenShot");
    
    if(testSprite && screenShotSprite)
    {
        int32 width = frameBufferWidth;
        int32 height = frameBufferHeight;
        PixelFormat format = FORMAT_RGBA8888;
        
        // picture is rotated (framebuffer coordinates start from bottom left)
        Image *image = Image::Create(height, width, format);
        uint8 *imageData = image->GetData();
        
        int32 formatSize = Image::GetFormatSize(format);
        uint32 imageDataSize = width * height * formatSize;
        uint8 *tempData = new uint8[imageDataSize];
        
#if defined(__DAVAENGINE_OPENGL__)
        
        LockNonMain();
        
        glBindFramebuffer(GL_FRAMEBUFFER_BINDING_OES, fboViewRenderbuffer);
        
        RENDER_VERIFY(glPixelStorei( GL_UNPACK_ALIGNMENT, 1 ));
        switch(format)
        {
            case FORMAT_RGBA8888:
                RENDER_VERIFY(glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid *)tempData));
                break;
            case FORMAT_RGB565:
                RENDER_VERIFY(glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_SHORT_5_6_5, (GLvoid *)tempData));
                break;
            case FORMAT_A8:
                RENDER_VERIFY(glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid *)tempData));
                break;
            case FORMAT_RGBA4444:
                RENDER_VERIFY(glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_SHORT_4_4_4_4, (GLvoid *)tempData));
                break;
            case FORMAT_A16:
                RENDER_VERIFY(glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_SHORT, (GLvoid *)tempData));
                break;
            default:
                break;
        }
        UnlockNonMain();
        
#endif //#if defined(__DAVAENGINE_OPENGL__)
        
        //TODO: optimize (ex. use pre-allocated buffer instead of dynamic allocation)
        
        // frame buffer starts from bottom left corner, but we need from top left, so we rotate picture here
        uint32 newIndex = 0;
        uint32 oldIndex = 0;
        
        for(uint32 w = 0; w < width; ++w)
        {
            for(uint32 h = 0; h < height; ++h)
            {
                for(int32 b = 0; b < formatSize; ++b)
                {
                    oldIndex = formatSize*width*h + formatSize*w + b;
                    imageData[newIndex++] = tempData[oldIndex];
                }
            }
        }
        SafeDeleteArray(tempData);
        
        if(image)
        {
            image->Save(FileSystem::Instance()->SystemPathForFrameworkPath(Format("~doc:screenshot%d.png", ++screenShotIndex)));
            SafeRelease(image);
        }
    }
}
    
void RenderManager::SetViewport(const Rect & rect, bool precaleulatedCoordinates)
{    
    if ((rect.dx < 0.0f) && (rect.dy < 0.0f))
    {
        viewport = rect;
        RENDER_VERIFY(glViewport(0, 0, frameBufferWidth, frameBufferHeight));
        return;
    }
    if (precaleulatedCoordinates) 
    {
        viewport = rect;
        RENDER_VERIFY(glViewport((int32)rect.x, (int32)rect.y, (int32)rect.dx, (int32)rect.dy));
        return;
    }

    PrepareRealMatrix();
    

	int32 x = (int32)(rect.x * currentDrawScale.x + currentDrawOffset.x);
	int32 y = (int32)(rect.y * currentDrawScale.y + currentDrawOffset.y);
	int32 width, height;
    width = (int32)(rect.dx * currentDrawScale.x);
    height = (int32)(rect.dy * currentDrawScale.y);    
    
    
    switch(renderOrientation)
    {
        case Core::SCREEN_ORIENTATION_PORTRAIT:
        { 
            y = frameBufferHeight - y - height;
        }
        break;    
        
        case Core::SCREEN_ORIENTATION_LANDSCAPE_LEFT:
		{
			int32 tmpY = y;
			y = x;
			x = tmpY;
			tmpY = height;
			height = width;
			width = tmpY;
		}
        break;
        case Core::SCREEN_ORIENTATION_LANDSCAPE_RIGHT:
		{
			int32 tmpY = height;
			height = width;
			width = tmpY;
			tmpY = y;
			y = frameBufferHeight/* * Core::GetVirtualToPhysicalFactor()*/ - x - height;
			x = frameBufferWidth/* * Core::GetVirtualToPhysicalFactor()*/ - tmpY - width;
		}
        break;
            
    }    
    RENDER_VERIFY(glViewport(x, y, width, height));
    viewport.x = (float32)x;
    viewport.y = (float32)y;
    viewport.dx = (float32)width;
    viewport.dy = (float32)height;
}

    

// Viewport management
void RenderManager::SetRenderOrientation(int32 orientation)
{
	renderOrientation = orientation;
	
//	glMatrixMode(GL_PROJECTION);
//	glLoadIdentity();
//#if defined(__DAVAENGINE_IPHONE__)
//	RENDER_VERIFY(glOrthof(0.0f, frameBufferWidth, frameBufferHeight, 0.0f, -1.0f, 1.0f));
//#else // for NON ES platforms
//	RENDER_VERIFY(glOrtho(0.0f, frameBufferWidth, frameBufferHeight, 0.0f, -1.0f, 1.0f));
//#endif
    
    Matrix4 orthoMatrix; 
    Matrix4 glTranslate;
    Matrix4 glRotate;

    orthoMatrix.glOrtho(0.0f, (float32)frameBufferWidth, (float32)frameBufferHeight, 0.0f, -1.0f, 1.0f);
	
    switch (orientation) 
	{
		case Core::SCREEN_ORIENTATION_PORTRAIT:
		case Core::SCREEN_ORIENTATION_TEXTURE:
			retScreenWidth = frameBufferWidth;
			retScreenHeight = frameBufferHeight;
			break;
		case Core::SCREEN_ORIENTATION_LANDSCAPE_LEFT:
            
            //mark glTranslatef(0.0f, (float32)frameBufferHeight, 0.0f);
			//mark glRotatef(-90.0f, 0.0f, 0.0f, 1.0f);
            
            glTranslate.glTranslate(0.0f, (float32)frameBufferHeight, 0.0f);
            glRotate.glRotate(-90.0f, 0.0f, 0.0f, 1.0f);
            
            orthoMatrix = glRotate * glTranslate * orthoMatrix;
            
			retScreenWidth = frameBufferHeight;
			retScreenHeight = frameBufferWidth;
            break;
		case Core::SCREEN_ORIENTATION_LANDSCAPE_RIGHT:
			//mark glTranslatef((float32)frameBufferWidth, 0.0f, 0.0f);
			//mark glRotatef(90.0f, 0.0f, 0.0f, 1.0f);
            
            glTranslate.glTranslate((float32)frameBufferWidth, 0.0f, 0.0f);
            glRotate.glRotate(90.0f, 0.0f, 0.0f, 1.0f);
            
            orthoMatrix = glRotate * glTranslate * orthoMatrix;

			retScreenWidth = frameBufferHeight;
			retScreenHeight = frameBufferWidth;
			break;
	}
    
    SetMatrix(MATRIX_PROJECTION, orthoMatrix);

//	if (orientation != Core::SCREEN_ORIENTATION_TEXTURE) 
//	{
//		glTranslatef(Core::Instance()->GetPhysicalDrawOffset().x, Core::Instance()->GetPhysicalDrawOffset().y, 0.0f);
//	}
	
    //glMatrixMode(GL_MODELVIEW);
	//glLoadIdentity();
	

    IdentityModelMatrix();
    
	RENDER_VERIFY("");

	IdentityMappingMatrix();
	SetVirtualViewScale();
	SetVirtualViewOffset();

}



void RenderManager::SetBlendMode(eBlendMode sfactor, eBlendMode dfactor)
{
	currentState.SetBlendMode(sfactor, dfactor);
}
	
eBlendMode RenderManager::GetSrcBlend()
{
	return currentState.sourceFactor;
}

eBlendMode RenderManager::GetDestBlend()
{
	return currentState.destFactor;
}


/*
 void RenderManager::EnableBlending(bool isEnabled)
{
	if((int32)isEnabled != oldBlendingEnabled)
	{
		if(isEnabled)
		{
			RENDER_VERIFY(glEnable(GL_BLEND));
		}
		else
		{
			RENDER_VERIFY(glDisable(GL_BLEND));
		}
		oldBlendingEnabled = isEnabled;
	}
}
    
void RenderManager::EnableDepthTest(bool isEnabled)
{
	if((int32)isEnabled != depthTestEnabled)
	{
		if(isEnabled)
		{
            RENDER_VERIFY(glEnable(GL_DEPTH_TEST));
		}
		else
		{
            RENDER_VERIFY(glDisable(GL_DEPTH_TEST));
		}
		depthTestEnabled = isEnabled;
	}
}

void RenderManager::EnableDepthWrite(bool isEnabled)
{
	if((int32)isEnabled != depthWriteEnabled)
	{
		if(isEnabled)
		{
            RENDER_VERIFY(glDepthMask(GL_TRUE));
		}
		else
		{
            RENDER_VERIFY(glDepthMask(GL_TRUE));
		}
		depthWriteEnabled = isEnabled;
	}
}
*/

void RenderManager::EnableVertexArray(bool isEnabled)
{
    if(isEnabled != oldVertexArrayEnabled)
    {
        if(isEnabled)
        {
            RENDER_VERIFY(glEnableClientState(GL_VERTEX_ARRAY));
        }
        else
        {
            RENDER_VERIFY(glDisableClientState(GL_VERTEX_ARRAY));
        }
        oldVertexArrayEnabled = isEnabled;
    }
}

void RenderManager::EnableNormalArray(bool isEnabled)
{
    if(isEnabled != oldNormalArrayEnabled)
    {
        if(isEnabled)
        {
            RENDER_VERIFY(glEnableClientState(GL_NORMAL_ARRAY));
        }
        else
        {
            RENDER_VERIFY(glDisableClientState(GL_NORMAL_ARRAY));
        }
        oldNormalArrayEnabled = isEnabled;
    }
}

void RenderManager::EnableTextureCoordArray(bool isEnabled, int32 textureLevel)
{
    if(isEnabled != oldTextureCoordArrayEnabled[textureLevel])
    {
        glClientActiveTexture(GL_TEXTURE0 + textureLevel);

        if(isEnabled)
        {
            RENDER_VERIFY(glEnableClientState(GL_TEXTURE_COORD_ARRAY));
        }
        else
        {
            RENDER_VERIFY(glDisableClientState(GL_TEXTURE_COORD_ARRAY));
        }
        oldTextureCoordArrayEnabled[textureLevel] = isEnabled;
    }
}

void RenderManager::EnableColorArray(bool isEnabled)
{
    if(isEnabled != oldColorArrayEnabled)
    {
        if(isEnabled)
        {
            RENDER_VERIFY(glEnableClientState(GL_COLOR_ARRAY));
        }
        else
        {
            RENDER_VERIFY(glDisableClientState(GL_COLOR_ARRAY));
        }
        oldColorArrayEnabled = isEnabled;
    }
}

    
void RenderManager::FlushState()
{
	PrepareRealMatrix();
    
    currentState.Flush(&hardwareState);
}

void RenderManager::FlushState(RenderStateBlock * stateBlock)
{
	PrepareRealMatrix();
	
	stateBlock->Flush(&hardwareState);
}

void RenderManager::SetTexCoordPointer(int size, eVertexDataType _typeIndex, int stride, const void *pointer)
{
	GLint type = VERTEX_DATA_TYPE_TO_GL[_typeIndex];
    RENDER_VERIFY(glTexCoordPointer(size, type, stride, pointer));
}

void RenderManager::SetVertexPointer(int size, eVertexDataType _typeIndex, int stride, const void *pointer)
{
	GLint type = VERTEX_DATA_TYPE_TO_GL[_typeIndex];
    RENDER_VERIFY(glVertexPointer(size, type, stride, pointer));
}

void RenderManager::SetNormalPointer(eVertexDataType _typeIndex, int stride, const void *pointer)
{
	GLint type = VERTEX_DATA_TYPE_TO_GL[_typeIndex];
    RENDER_VERIFY(glNormalPointer(type, stride, pointer));
}

void RenderManager::SetColorPointer(int size, eVertexDataType _typeIndex, int stride, const void *pointer)
{
	GLint type = VERTEX_DATA_TYPE_TO_GL[_typeIndex];
    RENDER_VERIFY(glColorPointer(size, type, stride, pointer));
}

void RenderManager::HWDrawArrays(ePrimitiveType type, int32 first, int32 count)
{
	const int32 types[PRIMITIVETYPE_COUNT] = 
	{
		GL_POINTS,			// 		PRIMITIVETYPE_POINTLIST = 0,
		GL_LINES,			// 		PRIMITIVETYPE_LINELIST,
		GL_LINE_STRIP,		// 		PRIMITIVETYPE_LINESTRIP,
		GL_TRIANGLES,		// 		PRIMITIVETYPE_TRIANGLELIST,
		GL_TRIANGLE_STRIP,	// 		PRIMITIVETYPE_TRIANGLESTRIP,
		GL_TRIANGLE_FAN,	// 		PRIMITIVETYPE_TRIANGLEFAN,
	};

	GLuint mode = types[type];

	if(debugEnabled)
	{
		Logger::Debug("Draw arrays texture: id %d", currentState.currentTexture[0]->id);
	}

    RENDER_VERIFY(glDrawArrays(mode, first, count));
    stats.drawArraysCalls++;
    switch(type)
    {
        case PRIMITIVETYPE_POINTLIST: 
            stats.primitiveCount[type] += count;
            break;
        case PRIMITIVETYPE_LINELIST:
            stats.primitiveCount[type] += count / 2;
            break;
        case PRIMITIVETYPE_LINESTRIP:
            stats.primitiveCount[type] += count - 1;
            break;
        case PRIMITIVETYPE_TRIANGLELIST:
            stats.primitiveCount[type] += count / 3;
            break;
        case PRIMITIVETYPE_TRIANGLEFAN:
        case PRIMITIVETYPE_TRIANGLESTRIP:
            stats.primitiveCount[type] += count - 2;
            break;
        default:
            break;
    };
}
    
void RenderManager::HWDrawElements(ePrimitiveType type, int32 count, eIndexFormat indexFormat, void * indices)
{
	const int32 types[PRIMITIVETYPE_COUNT] = 
	{
		GL_POINTS,			// 		PRIMITIVETYPE_POINTLIST = 0,
		GL_LINES,			// 		PRIMITIVETYPE_LINELIST,
		GL_LINE_STRIP,		// 		PRIMITIVETYPE_LINESTRIP,
		GL_TRIANGLES,		// 		PRIMITIVETYPE_TRIANGLELIST,
		GL_TRIANGLE_STRIP,	// 		PRIMITIVETYPE_TRIANGLESTRIP,
		GL_TRIANGLE_FAN,	// 		PRIMITIVETYPE_TRIANGLEFAN,
	};
	
	GLuint mode = types[type];
	
	if(debugEnabled)
	{
		Logger::Debug("Draw arrays texture: id %d", currentState.currentTexture[0]->id);
	}
#if defined(__DAVAENGINE_IPHONE__)
#if not defined(GL_UNSIGNED_INT)
#define GL_UNSIGNED_INT 0
#endif //not defined(GL_UNSIGNED_INT)
#endif // __DAVAENGINE_IPHONE__

	const int32 indexTypes[2] = 
	{
		GL_UNSIGNED_SHORT, 
		GL_UNSIGNED_INT,
	};
	
	RENDER_VERIFY(glDrawElements(mode, count, indexTypes[indexFormat], indices));
    stats.drawElementsCalls++;
    switch(type)
    {
        case PRIMITIVETYPE_POINTLIST: 
            stats.primitiveCount[type] += count;
            break;
        case PRIMITIVETYPE_LINELIST:
            stats.primitiveCount[type] += count / 2;
            break;
        case PRIMITIVETYPE_LINESTRIP:
            stats.primitiveCount[type] += count - 1;
            break;
        case PRIMITIVETYPE_TRIANGLELIST:
            stats.primitiveCount[type] += count / 3;
            break;
        case PRIMITIVETYPE_TRIANGLEFAN:
        case PRIMITIVETYPE_TRIANGLESTRIP:
            stats.primitiveCount[type] += count - 2;
            break;
        default:
            break;
    };
}

void RenderManager::ClearWithColor(float32 r, float32 g, float32 b, float32 a)
{
	RENDER_VERIFY(glClearColor(r, g, b, a));
	RENDER_VERIFY(glClear(GL_COLOR_BUFFER_BIT));
}

void RenderManager::ClearDepthBuffer(float32 depth)
{
#if defined(__DAVAENGINE_IPHONE__) || defined (__DAVAENGINE_ANDROID__)
    RENDER_VERIFY(glClearDepthf(depth));
#else //#if defined(__DAVAENGINE_IPHONE__) || defined (__DAVAENGINE_ANDROID__)
    RENDER_VERIFY(glClearDepth(depth));
#endif //#if defined(__DAVAENGINE_IPHONE__) || defined (__DAVAENGINE_ANDROID__)
    RENDER_VERIFY(glClear(GL_DEPTH_BUFFER_BIT));
}

void RenderManager::ClearStencilBuffer(int32 stencil)
{
	RENDER_VERIFY(glClearStencil(stencil));
	RENDER_VERIFY(glClear(GL_STENCIL_BUFFER_BIT));
}
    
void RenderManager::Clear(const Color & color, float32 depth, int32 stencil)
{
    RENDER_VERIFY(glClearColor(color.r, color.g, color.b, color.a));
#if defined(__DAVAENGINE_IPHONE__) || defined (__DAVAENGINE_ANDROID__)
    RENDER_VERIFY(glClearDepthf(depth));
#else //#if defined(__DAVAENGINE_IPHONE__) || defined (__DAVAENGINE_ANDROID__)
    RENDER_VERIFY(glClearDepth(depth));
#endif //#if defined(__DAVAENGINE_IPHONE__) || defined (__DAVAENGINE_ANDROID__)
    RENDER_VERIFY(glClearStencil(stencil));

    
    RENDER_VERIFY(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT));
}

void RenderManager::SetHWClip(const Rect &rect)
{
	PrepareRealMatrix();
	currentClip = rect;
	if(rect.dx < 0 || rect.dy < 0)
	{
		RENDER_VERIFY(glDisable(GL_SCISSOR_TEST));
		return;
	}
	int32 x = (int32)(rect.x * currentDrawScale.x + currentDrawOffset.x);
	int32 y = (int32)(rect.y * currentDrawScale.y + currentDrawOffset.y);
	int32 width = (int32)(rect.dx * currentDrawScale.x);
	int32 height = (int32)(rect.dy * currentDrawScale.y);
	switch (renderOrientation) 
	{
	case Core::SCREEN_ORIENTATION_PORTRAIT:
		{
			//			x = frameBufferWidth - x;
			y = frameBufferHeight/* * Core::GetVirtualToPhysicalFactor()*/ - y - height;
		}
		break;
	case Core::SCREEN_ORIENTATION_LANDSCAPE_LEFT:
		{
			int32 tmpY = y;
			y = x;
			x = tmpY;
			tmpY = height;
			height = width;
			width = tmpY;
		}
		break;
	case Core::SCREEN_ORIENTATION_LANDSCAPE_RIGHT:
		{
			int32 tmpY = height;
			height = width;
			width = tmpY;
			tmpY = y;
			y = frameBufferHeight/* * Core::GetVirtualToPhysicalFactor()*/ - x - height;
			x = frameBufferWidth/* * Core::GetVirtualToPhysicalFactor()*/ - tmpY - width;
		}
		break;
	}
	RENDER_VERIFY(glEnable(GL_SCISSOR_TEST));
	RENDER_VERIFY(glScissor(x, y, width, height));
}


void RenderManager::SetHWRenderTargetSprite(Sprite *renderTarget)
{
	if (renderTarget == NULL)
	{
//#if defined(__DAVAENGINE_IPHONE__)
//		RENDER_VERIFY(glBindFramebufferOES(GL_FRAMEBUFFER_OES, fboViewFramebuffer));
//#elif defined(__DAVAENGINE_ANDROID__)
////        renderTarget->GetTexture()->renderTargetModified = true;
//#else //Non ES platforms
//		RENDER_VERIFY(glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fboViewFramebuffer));
//#endif //PLATFORMS
        BindFBO(fboViewFramebuffer);

        SetViewport(Rect(0, 0, -1, -1), true);

		SetRenderOrientation(Core::Instance()->GetScreenOrientation());
	}
	else
	{
		renderOrientation = Core::SCREEN_ORIENTATION_TEXTURE;
//#if defined(__DAVAENGINE_IPHONE__)
//		RENDER_VERIFY(glBindFramebufferOES(GL_FRAMEBUFFER_OES, renderTarget->GetTexture()->fboID));
//#elif defined(__DAVAENGINE_ANDROID__)
//		BindFBO(renderTarget->GetTexture()->fboID);
//        renderTarget->GetTexture()->renderTargetModified = true;
//#else //Non ES platforms
//		RENDER_VERIFY(glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, renderTarget->GetTexture()->fboID));
//#endif //PLATFORMS
		BindFBO(renderTarget->GetTexture()->fboID);
#if defined(__DAVAENGINE_ANDROID__)
        renderTarget->GetTexture()->renderTargetModified = true;
#endif //#if defined(__DAVAENGINE_ANDROID__)

        
        SetViewport(Rect(0, 0, renderTarget->GetTexture()->width, renderTarget->GetTexture()->height), true);

//		RENDER_VERIFY(glMatrixMode(GL_PROJECTION));
//		RENDER_VERIFY(glLoadIdentity());
//#if defined(__DAVAENGINE_IPHONE__)
//		RENDER_VERIFY(glOrthof(0.0f, renderTarget->GetTexture()->width, 0.0f, renderTarget->GetTexture()->height, -1.0f, 1.0f));
//#else 
//		RENDER_VERIFY(glOrtho(0.0f, renderTarget->GetTexture()->width, 0.0f, renderTarget->GetTexture()->height, -1.0f, 1.0f));
//#endif

        Matrix4 orthoMatrix; 
        orthoMatrix.glOrtho(0.0f, (float32)renderTarget->GetTexture()->width, 0.0f, (float32)renderTarget->GetTexture()->height, -1.0f, 1.0f);
        SetMatrix(MATRIX_PROJECTION, orthoMatrix);
        
		//RENDER_VERIFY(glMatrixMode(GL_MODELVIEW));
		//RENDER_VERIFY(glLoadIdentity());
        IdentityModelMatrix();
		IdentityMappingMatrix();

		viewMappingDrawScale.x = renderTarget->GetResourceToPhysicalFactor();
		viewMappingDrawScale.y = renderTarget->GetResourceToPhysicalFactor();
//		Logger::Info("Sets with render target: Scale %.4f,    Offset: %.4f, %.4f", viewMappingDrawScale.x, viewMappingDrawOffset.x, viewMappingDrawOffset.y);
		RemoveClip();
	}
	
	currentRenderTarget = renderTarget;
}

void RenderManager::SetHWRenderTargetTexture(Texture * renderTarget)
{
	//renderOrientation = Core::SCREEN_ORIENTATION_TEXTURE;
	//IdentityModelMatrix();
	//IdentityMappingMatrix();
	BindFBO(renderTarget->fboID);
	RemoveClip();
}

    
void RenderManager::SetMatrix(eMatrixType type, const Matrix4 & matrix)
{
    GLint matrixMode[2] = {GL_MODELVIEW, GL_PROJECTION};
    matrices[type] = matrix;
    uniformMatrixFlags[UNIFORM_MATRIX_MODELVIEWPROJECTION] = 0; // require update
    uniformMatrixFlags[UNIFORM_MATRIX_NORMAL] = 0; // require update
    
    if (renderer != Core::RENDERER_OPENGL_ES_2_0)
    {
        RENDER_VERIFY(glMatrixMode(matrixMode[type]));
        RENDER_VERIFY(glLoadMatrixf(matrix.data));
    }
}
        
void RenderManager::HWglBindBuffer(GLenum target, GLuint buffer)
{
    DVASSERT(target - GL_ARRAY_BUFFER <= 1);
    if (bufferBindingId[target - GL_ARRAY_BUFFER] != buffer)
    {
        RENDER_VERIFY(glBindBuffer(target, buffer));
        bufferBindingId[target - GL_ARRAY_BUFFER] = buffer;
    }
}
    
void RenderManager::AttachRenderData()
{
    if (!currentRenderData)return;

    const int DEBUG = 0;
	Shader * shader = hardwareState.shader;
    
    RenderManager::Instance()->LockNonMain();
    if (!shader)
    {
        // TODO: should be moved to RenderManagerGL
        HWglBindBuffer(GL_ARRAY_BUFFER, currentRenderData->vboBuffer);
        HWglBindBuffer(GL_ELEMENT_ARRAY_BUFFER, currentRenderData->indexBuffer);
        
        
#if defined(__DAVAENGINE_DIRECTX9__)
        DVASSERT(currentRenderData->vboBuffer == 0);
#endif
        if (enabledAttribCount != 0)
        {
            for (int32 p = 0; p < enabledAttribCount; ++p)
            {
                if (DEBUG)Logger::Debug("!shader glDisableVertexAttribArray: %d", p);
                RENDER_VERIFY(glDisableVertexAttribArray(p));
            }
            enabledAttribCount = 0;
            pointerArraysRendererState = 0;
        }
        
        pointerArraysCurrentState = 0;
        int32 size = (int32)currentRenderData->streamArray.size();
        for (int32 k = 0; k < size; ++k)
        {
            RenderDataStream * stream = currentRenderData->streamArray[k];
            switch(stream->formatMark)
            {
                case EVF_VERTEX:
                    if (DEBUG)Logger::Debug("!shader SetVertexPointer");

                    SetVertexPointer(stream->size, stream->type, stream->stride, stream->pointer);
                    pointerArraysCurrentState |= EVF_VERTEX;
                    break;
                case EVF_NORMAL:
                    if (DEBUG)Logger::Debug("!shader SetNormalPointer");

                    SetNormalPointer(stream->type, stream->stride, stream->pointer);
                    pointerArraysCurrentState |= EVF_NORMAL;
                    break;
                case EVF_TEXCOORD0:
                    if (DEBUG)Logger::Debug("!shader SetTexCoordPointer 0");

                    glClientActiveTexture(GL_TEXTURE0);
                    SetTexCoordPointer(stream->size, stream->type, stream->stride, stream->pointer);
                    pointerArraysCurrentState |= EVF_TEXCOORD0;
                    break;
                case EVF_TEXCOORD1:
                    if (DEBUG)Logger::Debug("!shader SetTexCoordPointer 1");

                    glClientActiveTexture(GL_TEXTURE1);
                    SetTexCoordPointer(stream->size, stream->type, stream->stride, stream->pointer);
                    pointerArraysCurrentState |= EVF_TEXCOORD1;
                    break;
                default:
                    break;
            };
        };
        
        uint32 difference = pointerArraysCurrentState ^ pointerArraysRendererState;
        
        if (difference & EVF_VERTEX)
        {
            if (DEBUG)Logger::Debug("!shader EnableVertexArray: %d", (pointerArraysCurrentState & EVF_VERTEX) != 0);

            EnableVertexArray((pointerArraysCurrentState & EVF_VERTEX) != 0);
        }
        if (difference & EVF_NORMAL)
        {
            if (DEBUG)Logger::Debug("!shader EnableNormalArray: %d", (pointerArraysCurrentState & EVF_NORMAL) != 0);

            EnableNormalArray((pointerArraysCurrentState & EVF_NORMAL) != 0);
        }
        if (difference & EVF_TEXCOORD0)
        {
            if (DEBUG)Logger::Debug("!shader EnableTextureCoordArray-0: %d", (pointerArraysCurrentState & EVF_TEXCOORD0) != 0);

            EnableTextureCoordArray((pointerArraysCurrentState & EVF_TEXCOORD0) != 0, 0);
        }
        if (difference & EVF_TEXCOORD1)
        {
            if (DEBUG)Logger::Debug("!shader EnableTextureCoordArray-1: %d", (pointerArraysCurrentState & EVF_TEXCOORD1) != 0);

            EnableTextureCoordArray((pointerArraysCurrentState & EVF_TEXCOORD1) != 0, 1);
        }
        pointerArraysRendererState = pointerArraysCurrentState;
        
    }
    else
    {
        if (GetRenderer() == Core::RENDERER_OPENGL)
        {
            if (oldVertexArrayEnabled)
            {
                EnableVertexArray(false);
            }
            if (oldNormalArrayEnabled)
            {
                EnableNormalArray(false);
            }
            if (oldTextureCoordArrayEnabled[0])
            {
                EnableTextureCoordArray(false, 0);
            }
            if (oldTextureCoordArrayEnabled[1])
            {
                EnableTextureCoordArray(false, 1);
            }
            pointerArraysRendererState = 0;
        }

        int32 currentEnabledAttribCount = 0;
        //glDisableVertexAttribArray(0);
        //glDisableVertexAttribArray(1);
        //pointerArraysCurrentState = 0;
        
        HWglBindBuffer(GL_ARRAY_BUFFER, currentRenderData->vboBuffer);
        HWglBindBuffer(GL_ELEMENT_ARRAY_BUFFER, currentRenderData->indexBuffer);
        
        
        int32 size = (int32)currentRenderData->streamArray.size();
        
        //DVASSERT(size >= shader->GetAttributeCount() && "Shader attribute count higher than model attribute count");
        if (size < shader->GetAttributeCount())
        {
            // Logger::Error("Shader attribute count higher than model attribute count");
        }
        
        for (int32 k = 0; k < size; ++k)
        {
            RenderDataStream * stream = currentRenderData->streamArray[k];
            GLboolean normalized = GL_FALSE;
            
            int32 attribIndex = shader->GetAttributeIndex(stream->formatMark);
            if (attribIndex != -1)
            {
				if(TYPE_UNSIGNED_BYTE == stream->type)
				{
					normalized = GL_TRUE;
				}
                RENDER_VERIFY(glVertexAttribPointer(attribIndex, stream->size, VERTEX_DATA_TYPE_TO_GL[stream->type], normalized, stream->stride, stream->pointer));
                if (DEBUG)Logger::Debug("shader glVertexAttribPointer: %d", attribIndex);

                if (attribIndex >= enabledAttribCount)  // enable only if it was not enabled on previous step
                {
                    RENDER_VERIFY(glEnableVertexAttribArray(attribIndex));
                    if (DEBUG)Logger::Debug("shader glEnableVertexAttribArray: %d", attribIndex);
                }
                if (attribIndex + 1 > currentEnabledAttribCount)
                    currentEnabledAttribCount = attribIndex + 1;    // count of enabled attributes
                
                //pointerArraysCurrentState |= stream->formatMark;
            }
        };
        
        for (int32 p = currentEnabledAttribCount; p < enabledAttribCount; ++p)
        {
            if (DEBUG)Logger::Debug("shader glDisableVertexAttribArray: %d", p);

            RENDER_VERIFY(glDisableVertexAttribArray(p));
        }
        enabledAttribCount = currentEnabledAttribCount;
        //pointerArraysRendererState = pointerArraysCurrentState;
    }
    RenderManager::Instance()->UnlockNonMain();
}


//void RenderManager::InitGL20()
//{
//    colorOnly = 0;
//    colorWithTexture = 0;
//    
//    
//    colorOnly = new Shader();
//    colorOnly->LoadFromYaml("~res:/Shaders/Default/fixed_func_color_only.shader");
//    colorWithTexture = new Shader();
//    colorWithTexture->LoadFromYaml("~res:/Shaders/Default/fixed_func_texture.shader");
//        
//    
//}
//
//void RenderManager::ReleaseGL20()
//{
//    SafeRelease(colorOnly);
//    SafeRelease(colorWithTexture);
//}
//




};

#endif // __DAVAENGINE_OPENGL__
