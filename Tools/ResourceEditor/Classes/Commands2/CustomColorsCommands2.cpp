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


#include "Commands2/CustomColorsCommands2.h"
#include "../Qt/Scene/System/LandscapeEditorDrawSystem/CustomColorsProxy.h"
#include "../Qt/Scene/SceneEditor2.h"
#include "../Qt/Scene/SceneSignals.h"
#include "../Qt/Main/QtUtils.h"

ModifyCustomColorsCommand::ModifyCustomColorsCommand(Image* originalImage, Image* currentImage,
                                                     CustomColorsProxy* _customColorsProxy, const Rect& _updatedRect)
    : Command2(CMDID_CUSTOM_COLORS_MODIFY, "Custom Colors Modification")
    , texture(nullptr)
{
    const Vector2 topLeft(floorf(_updatedRect.x), floorf(_updatedRect.y));
    const Vector2 bottomRight(ceilf(_updatedRect.x + _updatedRect.dx), ceilf(_updatedRect.y + _updatedRect.dy));

    updatedRect = Rect(topLeft, bottomRight - topLeft);

    customColorsProxy = SafeRetain(_customColorsProxy);

    undoImage = Image::CopyImageRegion(originalImage, updatedRect);
    redoImage = Image::CopyImageRegion(currentImage, updatedRect);
}

ModifyCustomColorsCommand::~ModifyCustomColorsCommand()
{
	SafeRelease(undoImage);
	SafeRelease(redoImage);
	SafeRelease(customColorsProxy);
    SafeRelease(texture);
}

void ModifyCustomColorsCommand::Undo()
{
	ApplyImage(undoImage);
	customColorsProxy->DecrementChanges();
}

void ModifyCustomColorsCommand::Redo()
{
	ApplyImage(redoImage);
	customColorsProxy->IncrementChanges();
}

void ModifyCustomColorsCommand::ApplyImage(DAVA::Image *image)
{
    SafeRelease(texture);

    Texture* customColorsTarget = customColorsProxy->GetTexture();
    texture = Texture::CreateFromData(image->GetPixelFormat(), image->GetData(),
                                      image->GetWidth(), image->GetHeight(), false);

    RenderSystem2D::RenderTargetPassDescriptor desc;
    desc.target = customColorsTarget;
    desc.shouldClear = false;
    desc.shouldTransformVirtualToPhysical = false;
    RenderSystem2D::Instance()->BeginRenderTargetPass(desc);
    RenderSystem2D::Instance()->DrawTexture(texture, customColorsProxy->GetBrushMaterial(), Color::White, updatedRect);
    RenderSystem2D::Instance()->EndRenderTargetPass();

    customColorsProxy->UpdateRect(updatedRect);
}

Entity* ModifyCustomColorsCommand::GetEntity() const
{
    return nullptr;
}
