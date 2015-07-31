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

#ifndef __BASE_TEST_H__
#define __BASE_TEST_H__

#include "Infrastructure/Screen/BaseScreen.h"
#include "Infrastructure/Utils/ControlHelpers.h"
#include "MemoryManager/MemoryProfiler.h"
#include "TeamCityTestsOutput.h"

class BaseTest : public BaseScreen
{
public:
    
    struct FrameInfo
    {
        FrameInfo() : delta(0.0f) {}
        FrameInfo(float32 _delta) : delta(_delta) {}

        float32 delta;
    };

    struct TestParams
    {
        TestParams() 
            :   targetTime(0)
            ,   startTime(0)
            ,   endTime(0)
            ,   targetFramesCount(0)
            ,   targetFrameDelta(0.0f)
            ,   frameForDebug(0)
            ,   maxDelta(0.0f)  
            {} 

        int32 targetTime;
        
        int32 startTime;
        int32 endTime;
        
        int32 targetFramesCount;
        float32 targetFrameDelta;

        int32 frameForDebug;

        // stop test logic when frameDelta greater than maxDelta
        float32 maxDelta;
    };
    
    BaseTest(const String& testName, const TestParams& testParams);
    
    void OnStart() override;
    void OnFinish() override;
    
    void BeginFrame() override;
    void EndFrame() override;
    void SystemUpdate(float32 timeElapsed) override;
    
    void ShowUI(bool visible);
    bool IsUIVisible() const;
    
    const String& GetName() const;
    
    bool IsFinished() const override;
    
    void SetParams(const TestParams& testParams);
    const TestParams& GetParams() const;
    
    float32 GetOverallTestTime() const;
    uint64 GetElapsedTime() const;
    uint32 GetFrameNumber() const;
    
    Scene* GetScene() const;
    const Vector<FrameInfo>& GetFramesInfo() const;
    
    static const float32 FRAME_OFFSET;
    
protected:
    
    virtual ~BaseTest() {};

    void LoadResources() override;
    void UnloadResources() override;

    virtual void CreateUI();
    virtual void UpdateUI();
    
    size_t GetAllocatedMemory();
    DAVA::UIControl* GetUIRoot() const;
    
    virtual void PerformTestLogic(float32 timeElapsed) = 0;
    
private:
    
    Vector<FrameInfo> frames;
    String testName;
    
    TestParams testParams;
    
    uint32 frameNumber;
    uint64 startTime;
    uint64 elapsedTime;
    
    float32 overallTestTime;
    
    float32 minDelta;
    float32 maxDelta;
    float32 currentFrameDelta;
    
    Scene* scene;
    UI3DView* sceneView;
    
    DAVA::UIControl* uiRoot;
    
    UIStaticText* testNameText;
    UIStaticText* maxFPSText;
    UIStaticText* minFPSText;
    UIStaticText* fpsText;
    UIStaticText* testTimeText;
    UIStaticText* elapsedTimeText;
    UIStaticText* framesRenderedText;
    
    size_t maxAllocatedMemory;
};

inline const Vector<BaseTest::FrameInfo>& BaseTest::GetFramesInfo() const
{
    return frames;
}

inline Scene* BaseTest::GetScene() const
{
    return scene;
}

inline bool BaseTest::IsFinished() const
{
    if (testParams.targetFramesCount > 0 && frameNumber >= (testParams.targetFramesCount + FRAME_OFFSET))
    {
        return true;
    }
    if (testParams.targetTime > 0 && (overallTestTime * 1000) >= testParams.targetTime)
    {
        return true;
    }
    
    return false;
}

inline const String& BaseTest::GetName() const
{
    return testName;
}

inline uint64 BaseTest::GetElapsedTime() const
{
    return elapsedTime;
}

inline float32 BaseTest::GetOverallTestTime() const
{
    return overallTestTime;
}

inline uint32 BaseTest::GetFrameNumber() const
{
    return frameNumber;
}

inline void BaseTest::SetParams(const TestParams& _testParams)
{
    DVASSERT_MSG(frameNumber == 0, "Can't set params after test started");
    
    this->testParams = _testParams;
}

inline const BaseTest::TestParams& BaseTest::GetParams() const
{
    return testParams;
}

inline DAVA::UIControl* BaseTest::GetUIRoot() const
{
    return uiRoot;
}

inline void BaseTest::ShowUI(bool visible)
{
    GetUIRoot()->SetVisible(visible);
}

inline bool BaseTest::IsUIVisible() const
{
    return GetUIRoot()->GetVisible();
}

#endif