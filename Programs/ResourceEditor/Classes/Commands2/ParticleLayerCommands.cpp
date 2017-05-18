#include "Commands2/ParticleLayerCommands.h"
#include "Commands2/RECommandIDs.h"

#include "FileSystem/FilePath.h"
#include "Particles/ParticleLayer.h"

using namespace DAVA;

CommandChangeLayerMaterialProperties::CommandChangeLayerMaterialProperties(ParticleLayer* layer_, const FilePath& spritePath, eBlending blending, bool enableFog, bool enableBlending)
    : RECommand(CMDID_PARTICLE_LAYER_CHANGED_MATERIAL_VALUES, "Change Layer properties")
    , layer(layer_)
{
    newParams.spritePath = spritePath;
    newParams.blending = blending;
    newParams.enableFog = enableFog;
    newParams.enableBlending = enableBlending;

    DVASSERT(layer != nullptr);
    if (layer != nullptr)
    {
        oldParams.spritePath = layer->spritePath;
        oldParams.blending = layer->blending;
        oldParams.enableFog = layer->enableFog;
        oldParams.enableBlending = layer->enableFrameBlend;
    }
}

void CommandChangeLayerMaterialProperties::Redo()
{
    ApplyParams(newParams);
}

void CommandChangeLayerMaterialProperties::Undo()
{
    ApplyParams(oldParams);
}

void CommandChangeLayerMaterialProperties::ApplyParams(const CommandChangeLayerMaterialProperties::LayerParams& params)
{
    if (layer != nullptr)
    {
        layer->SetSprite(params.spritePath);
        layer->blending = params.blending;
        layer->enableFog = params.enableFog;
        layer->enableFrameBlend = params.enableBlending;
    }
}

DAVA::ParticleLayer* CommandChangeLayerMaterialProperties::GetLayer() const
{
    return layer;
}

CommandChangeFlowProperties::CommandChangeFlowProperties(ParticleLayer* layer_, CommandChangeFlowProperties::FlowParams&& params)
    :RECommand(CMDID_PARTICLE_LAYER_CHANGED_FLOW_VALUES, "Change Flow Properties")
    , layer(layer_)
    , newParams(params)
{
    DVASSERT(layer != nullptr);
    if (layer != nullptr)
    {
        oldParams.spritePath = layer->spritePath;
        oldParams.enableFlow = layer->enableFlow;
        oldParams.flowSpeed = layer->flowSpeed;
        oldParams.flowSpeedVariation = layer->flowSpeedVariation;
        oldParams.flowSpeedOverLife = layer->flowSpeedOverLife;
        oldParams.flowOffset = layer->flowOffset;
        oldParams.flowOffsetVariation = layer->flowOffsetOverLife;
        oldParams.flowOffsetOverLife = layer->flowOffsetOverLife;
    }
}

void CommandChangeFlowProperties::Undo()
{
    ApplyParams(oldParams);
}

void CommandChangeFlowProperties::Redo()
{
    ApplyParams(newParams);
}

DAVA::ParticleLayer* CommandChangeFlowProperties::GetLayer() const
{
    return layer;
}

void CommandChangeFlowProperties::ApplyParams(FlowParams& params)
{
    if (layer != nullptr)
    {
        layer->enableFlow = params.enableFlow;
        layer->SetFlowmap(params.spritePath);
        PropertyLineHelper::SetValueLine(layer->flowSpeed, params.flowSpeed);
        PropertyLineHelper::SetValueLine(layer->flowSpeedVariation , params.flowSpeedVariation);
        PropertyLineHelper::SetValueLine(layer->flowSpeedOverLife, params.flowSpeedOverLife);
        PropertyLineHelper::SetValueLine(layer->flowOffset, params.flowOffset);
        PropertyLineHelper::SetValueLine(layer->flowOffsetVariation, params.flowOffsetVariation);
        PropertyLineHelper::SetValueLine(layer->flowOffsetOverLife, params.flowOffsetOverLife);
    }
}

CommandChangeNoiseProperties::CommandChangeNoiseProperties(DAVA::ParticleLayer* layer_, NoiseParams&& params)
    : RECommand(CMDID_PARTICLE_LAYER_CHANGED_NOISE_VALUES, "Change Noise Properties")
    , layer(layer_)
    , newParams(params)
{
    DVASSERT(layer != nullptr);
    if (layer != nullptr)
    {
        oldParams.noisePath = layer->noisePath;
        oldParams.enableNoise = layer->enableNoise;
        oldParams.noiseScale = layer->noiseScale;
        oldParams.noiseScaleVariation = layer->noiseScaleVariation;
        oldParams.noiseScaleOverLife = layer->noiseScaleOverLife;
        oldParams.enableNoiseScroll = layer->enableNoiseScroll;
        oldParams.noiseUScrollSpeed = layer->noiseUScrollSpeed;
        oldParams.noiseUScrollSpeedVariation = layer->noiseUScrollSpeedVariation;
        oldParams.noiseUScrollSpeedOverLife = layer->noiseUScrollSpeedOverLife;
        oldParams.noiseVScrollSpeed = layer->noiseVScrollSpeed;
        oldParams.noiseVScrollSpeedVariation = layer->noiseVScrollSpeedVariation;
        oldParams.noiseVScrollSpeedOverLife = layer->noiseVScrollSpeedOverLife;
    }
}

void CommandChangeNoiseProperties::Undo()
{
    ApplyParams(oldParams);
}

void CommandChangeNoiseProperties::Redo()
{
    ApplyParams(newParams);
}

DAVA::ParticleLayer* CommandChangeNoiseProperties::GetLayer() const
{
    return layer;
}

void CommandChangeNoiseProperties::ApplyParams(NoiseParams& params)
{
    if (layer != nullptr)
    {
        layer->enableNoise = params.enableNoise;
        layer->enableNoiseScroll = params.enableNoiseScroll;
        layer->SetNoise(params.noisePath);
        PropertyLineHelper::SetValueLine(layer->noiseScale, params.noiseScale);
        PropertyLineHelper::SetValueLine(layer->noiseScaleVariation, params.noiseScaleVariation);
        PropertyLineHelper::SetValueLine(layer->noiseScaleOverLife, params.noiseScaleOverLife);
        PropertyLineHelper::SetValueLine(layer->noiseUScrollSpeed, params.noiseUScrollSpeed);
        PropertyLineHelper::SetValueLine(layer->noiseUScrollSpeedVariation, params.noiseUScrollSpeedVariation);
        PropertyLineHelper::SetValueLine(layer->noiseUScrollSpeedOverLife, params.noiseUScrollSpeedOverLife);
        PropertyLineHelper::SetValueLine(layer->noiseVScrollSpeed, params.noiseVScrollSpeed);
        PropertyLineHelper::SetValueLine(layer->noiseVScrollSpeedVariation, params.noiseVScrollSpeedVariation);
        PropertyLineHelper::SetValueLine(layer->noiseVScrollSpeedOverLife, params.noiseVScrollSpeedOverLife);
    }
}

CommandChangeFresnelToAlphaProperties::CommandChangeFresnelToAlphaProperties(DAVA::ParticleLayer* layer_, FresnelToAlphaParams&& params)
    : RECommand(CMDID_PARTICLE_LAYER_CHANGED_FRES_TO_ALPHA_VALUES, "Change Fresnel to Alpha Properties")
    , layer(layer_)
    , newParams(params)
{
    DVASSERT(layer != nullptr);
    if (layer != nullptr)
    {
        oldParams.useFresnelToAlpha = layer->useFresnelToAlpha;
        oldParams.fresnelToAlphaPower = layer->fresnelToAlphaPower;
        oldParams.fresnelToAlphaBias = layer->fresnelToAlphaBias;
    }
}

void CommandChangeFresnelToAlphaProperties::Undo()
{
    ApplyParams(oldParams);
}

void CommandChangeFresnelToAlphaProperties::Redo()
{
    ApplyParams(newParams);
}

DAVA::ParticleLayer* CommandChangeFresnelToAlphaProperties::GetLayer() const
{
    return layer;
}

void CommandChangeFresnelToAlphaProperties::ApplyParams(FresnelToAlphaParams& params)
{
    if (layer != nullptr)
    {
        layer->useFresnelToAlpha = params.useFresnelToAlpha;
        layer->fresnelToAlphaBias = params.fresnelToAlphaBias;
        layer->fresnelToAlphaPower = params.fresnelToAlphaPower;
    }
}
