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

#include "VisibilityCheckComponent.h"
#include "Scene3D/Entity.h"
#include "Render/Texture.h"
#include "Utils/Random.h"

using namespace DAVA;

REGISTER_CLASS(VisibilityCheckComponent)

VisibilityCheckComponent::VisibilityCheckComponent()
{
}

Component* VisibilityCheckComponent::Clone(Entity* toEntity)
{
    auto visibilityCheckComponent = new VisibilityCheckComponent();
    visibilityCheckComponent->SetEntity(toEntity);

    return visibilityCheckComponent;
}

float32 VisibilityCheckComponent::GetRadius() const
{
    return radius;
}

void VisibilityCheckComponent::SetRadius(float32 r)
{
    radius = r;
    InvalidatePointSet();
}

float32 VisibilityCheckComponent::GetDistanceBetweenPoints() const
{
    return distanceBetweenPoints;
}

void VisibilityCheckComponent::SetDistanceBetweenPoints(float32 d)
{
    distanceBetweenPoints = d;
    InvalidatePointSet();
}

bool VisibilityCheckComponent::IsPointSetValid() const
{
    return (shouldBuildPointSet == false);
}

void VisibilityCheckComponent::InvalidatePointSet()
{
    shouldBuildPointSet = true;
}

namespace VCCHelper
{
inline float32 RandomFloat(float32 lower, float32 upper)
{
    return lower + (upper - lower) * (static_cast<float32>(rand()) / static_cast<float>(RAND_MAX));
}
}

void VisibilityCheckComponent::BuildPointSet()
{
    auto polar = [](float32 angle, float32 distance) -> Vector3 {
        return Vector3(std::cos(angle) * distance, std::sin(angle) * distance, 0.0f);
    };

    auto canIncludePoint = [this](const Vector3& pt) -> bool {
        float32 distanceFromCenter = pt.x * pt.x + pt.y * pt.y;
        if (distanceFromCenter > radius * radius)
            return false;

        for (const auto& e : points)
        {
            float32 dx = e.x - pt.x;
            float32 dy = e.y - pt.y;
            if (dx * dx + dy * dy < distanceBetweenPoints * distanceBetweenPoints)
                return false;
        }

        return true;
    };

    auto generateAroundPoint = [this, &canIncludePoint, &polar](const Vector3& src) -> bool {
        const uint32 maxAttempts = 36;
        float32 angle = VCCHelper::RandomFloat(-PI, PI);
        float32 da = 2.0f * PI / static_cast<float>(maxAttempts);
        uint32 attempts = 0;
        Vector3 newPoint;
        bool canInclude = false;
        do
        {
            newPoint = src + polar(angle, 2.0f * distanceBetweenPoints);
            canInclude = canIncludePoint(newPoint);
            angle += da;
        } while ((++attempts < maxAttempts) && !canInclude);

        if (canInclude)
        {
            points.push_back(newPoint);
        }

        return canInclude;
    };

    points.clear();
    float32 totalSquare = radius * radius;
    float32 smallSquare = distanceBetweenPoints * distanceBetweenPoints;
    uint32 pointsToGenerate = 2 * static_cast<uint32>(totalSquare / smallSquare);
    points.reserve(pointsToGenerate);

    if (pointsToGenerate < 2)
    {
        points.emplace_back(0.0f, 0.0f, 0.0f);
    }
    else
    {
        auto lastPoint = polar(VCCHelper::RandomFloat(-PI, +PI), radius - distanceBetweenPoints);
        points.push_back(lastPoint);

        bool canGenerate = true;
        while (canGenerate)
        {
            canGenerate = false;
            for (int32 i = static_cast<int32>(points.size()) - 1; i >= 0; --i)
            {
                if (generateAroundPoint(points.at(i)))
                {
                    canGenerate = true;
                    break;
                }
            }
        }
    }

    if (verticalVariance > 0.0f)
    {
        for (auto& p : points)
        {
            p.z = VCCHelper::RandomFloat(-verticalVariance, verticalVariance);
        }
    }

    shouldBuildPointSet = false;
}

const Vector<Vector3>& VisibilityCheckComponent::GetPoints() const
{
    DVASSERT(points.size() > 0);
    return points;
}

const Color& VisibilityCheckComponent::GetColor() const
{
    return color;
}

void VisibilityCheckComponent::SetColor(const Color& clr)
{
    DVASSERT(!points.empty());
    color = clr;
    color.a = 1.0f;
    isValid = false;
}

Color VisibilityCheckComponent::GetNormalizedColor() const
{
    Color normalizedColor = color;
    if (shouldNormalizeColor)
    {
        float32 fpoints = static_cast<float>(points.size());
        normalizedColor.r = (color.r > 0.0f) ? std::max(1.0f / 255.0f, color.r / fpoints) : 0.0f;
        normalizedColor.g = (color.g > 0.0f) ? std::max(1.0f / 255.0f, color.g / fpoints) : 0.0f;
        normalizedColor.b = (color.b > 0.0f) ? std::max(1.0f / 255.0f, color.b / fpoints) : 0.0f;
    }
    return normalizedColor;
}

bool VisibilityCheckComponent::IsValid() const
{
    return isValid && !shouldBuildPointSet;
}

void VisibilityCheckComponent::SetValid()
{
    isValid = true;
}

float32 VisibilityCheckComponent::GetUpAngle() const
{
    return upAngle;
}

void VisibilityCheckComponent::SetUpAngle(float32 value)
{
    upAngle = std::max(0.0f, std::min(90.0f, value));
    isValid = false;
}

float32 VisibilityCheckComponent::GetDownAngle() const
{
    return downAngle;
}

void VisibilityCheckComponent::SetDownAngle(float32 value)
{
    downAngle = std::max(0.0f, std::min(90.0f, value));
    isValid = false;
}

bool VisibilityCheckComponent::IsEnabled() const
{
    return isEnabled;
}

void VisibilityCheckComponent::SetEnabled(bool value)
{
    isEnabled = value;
}

bool VisibilityCheckComponent::ShoouldNormalizeColor() const
{
    return shouldNormalizeColor;
}

void VisibilityCheckComponent::SetShoouldNormalizeColor(bool value)
{
    shouldNormalizeColor = value;
}

float32 VisibilityCheckComponent::GetVerticalVariance() const
{
    return verticalVariance;
}

void VisibilityCheckComponent::SetVerticalVariance(float32 value)
{
    verticalVariance = std::max(0.0f, value);
    shouldBuildPointSet = true;
}

float32 VisibilityCheckComponent::GetMaximumDistance() const
{
    return maximumDistance;
}

void VisibilityCheckComponent::SetMaximumDistance(float32 value)
{
    maximumDistance = std::max(0.0f, value);
}

bool VisibilityCheckComponent::ShouldPlaceOnLandscape() const
{
    return shouldPlaceOnLandscape;
}

void VisibilityCheckComponent::SetShouldPlaceOnLandscape(bool value)
{
    shouldPlaceOnLandscape = value;
    isValid = false;
}

float32 VisibilityCheckComponent::GetHeightAboveLandscape() const
{
    return heightAboveLandscape;
}

void VisibilityCheckComponent::SetHeightAboveLandscape(float32 value)
{
    heightAboveLandscape = value;
    isValid = false;
}

void VisibilityCheckComponent::Serialize(DAVA::KeyedArchive* archive, DAVA::SerializationContext* serializationContext)
{
    Component::Serialize(archive, serializationContext);

    archive->SetColor("vsc.color", color);
    archive->SetFloat("vsc.radius", radius);
    archive->SetFloat("vsc.distanceBetweenPoints", distanceBetweenPoints);
    archive->SetFloat("vsc.upAngle", upAngle);
    archive->SetFloat("vsc.downAngle", downAngle);
    archive->SetFloat("vsc.verticalVariance", verticalVariance);
    archive->SetFloat("vsc.maximumDistance", maximumDistance);
    archive->SetFloat("vsc.heightAboveLandscape", heightAboveLandscape);
    archive->SetBool("vsc.isEnabled", isEnabled);
    archive->SetBool("vsc.shouldNormalizeColor", shouldNormalizeColor);
    archive->SetBool("vsc.shouldPlaceOnLandscape", shouldPlaceOnLandscape);
}

void VisibilityCheckComponent::Deserialize(DAVA::KeyedArchive* archive, DAVA::SerializationContext* serializationContext)
{
    isValid = false;
    shouldBuildPointSet = true;
    color = archive->GetColor("vsc.color");
    radius = archive->GetFloat("vsc.radius");
    distanceBetweenPoints = archive->GetFloat("vsc.distanceBetweenPoints");
    upAngle = archive->GetFloat("vsc.upAngle");
    downAngle = archive->GetFloat("vsc.downAngle");
    verticalVariance = archive->GetFloat("vsc.verticalVariance");
    maximumDistance = archive->GetFloat("vsc.maximumDistance");
    heightAboveLandscape = archive->GetFloat("vsc.heightAboveLandscape");
    isEnabled = archive->GetBool("vsc.isEnabled");
    shouldNormalizeColor = archive->GetBool("vsc.shouldNormalizeColor");
    shouldPlaceOnLandscape = archive->GetBool("vsc.shouldPlaceOnLandscape");
}
