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

#include "DAVAEngine.h"
#include "UnitTests/UnitTests.h"
#include "Render/Image/LibJpegHelper.h"

using namespace DAVA;

DAVA_TESTCLASS(LoadImageTest)
{
  DAVA_TEST(JpegExifTest)
  {
  LibJpegHelper helper;

Vector<Image*> set;

ScopedPtr<File> imgFile(File::Create("~res:/TestData/LoadImageTest/EXIF.jpg", File::OPEN | File::READ));

eErrorCode res = helper.ReadFile(imgFile, set, 0);
TEST_VERIFY(eErrorCode::SUCCESS == res);

for (auto item : set)
{
    SafeRelease(item);
}
set.clear();
}

DAVA_TEST(TgaTest)
{
    // array of pixels in format R,G,B,A. It is the expected contents of image.data buffer after loading of either 10x10_rgba8888.tga or 10x10_rgba8888_norle.tga files
    static const Array<uint8, 4 * 10 * 10> tga10x10data = {
        0xf3, 0x00, 0x12, 0xff, 0xf3, 0x00, 0x12, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xf3, 0x00, 0x12, 0xff, 0xf3, 0x00, 0x12, 0xff, 0xf3, 0x00, 0x12, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf3, 0x00, 0x12, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0xff, 0x24, 0xff, 0x00, 0xff, 0x24, 0xff,
        0x00, 0xff, 0x24, 0xff, 0x12, 0x00, 0xff, 0xff, 0x12, 0x00, 0xff, 0xff, 0x12, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0x00, 0xff, 0x24, 0xff, 0x00, 0xff, 0x24, 0xb9, 0x00, 0xff, 0x24, 0xb9, 0x12, 0x00, 0xff, 0xb9, 0x12, 0x00, 0xff, 0xb9, 0x12, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0xff, 0x24, 0xff, 0x00, 0xff, 0x24, 0xb9, 0x00, 0xff, 0x24, 0xb9, 0x12, 0x00, 0xff, 0xb9, 0x12, 0x00, 0xff, 0xb9, 0x12, 0x00, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x12, 0x00, 0xff, 0xff, 0x12, 0x00, 0xff, 0xb9, 0x12, 0x00, 0xff, 0xb9, 0x00, 0xff, 0x24, 0xb9,
        0x00, 0xff, 0x24, 0xb9, 0x00, 0xff, 0x24, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x12, 0x00, 0xff, 0xff, 0x12, 0x00, 0xff, 0xb9,
        0x12, 0x00, 0xff, 0xb9, 0x00, 0xff, 0x24, 0xb9, 0x00, 0xff, 0x24, 0xb9, 0x00, 0xff, 0x24, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0x12, 0x00, 0xff, 0xff, 0x12, 0x00, 0xff, 0xff, 0x12, 0x00, 0xff, 0xff, 0x00, 0xff, 0x24, 0xff, 0x00, 0xff, 0x24, 0xff, 0x00, 0xff, 0x24, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xf3, 0x00, 0x12, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xf3, 0x00, 0x12, 0xff, 0xf3, 0x00, 0x12, 0xff, 0xf3, 0x00, 0x12, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf3, 0x00, 0x12, 0xff, 0xf3, 0x00, 0x12, 0xff
    };

    DAVA::Vector<DAVA::Image*> imgSet;
    auto ClearImgSet = [&imgSet]()
    {
        for (auto image : imgSet)
        {
            image->Release();
        }
        imgSet.clear();
    };

    TEST_VERIFY(DAVA::ImageSystem::Instance()->Load("~res:/TestData/LoadImageTest/rgb888_rle_topleft.tga", imgSet) == DAVA::eErrorCode::SUCCESS);
    TEST_VERIFY(imgSet[0]->GetPixelFormat() == PixelFormat::FORMAT_RGB888);
    ClearImgSet();

    TEST_VERIFY(DAVA::ImageSystem::Instance()->Load("~res:/TestData/LoadImageTest/rgba8888_rle_bottomleft.tga", imgSet) == DAVA::eErrorCode::SUCCESS);
    TEST_VERIFY(imgSet[0]->GetPixelFormat() == PixelFormat::FORMAT_RGBA8888);
    ClearImgSet();

    TEST_VERIFY(DAVA::ImageSystem::Instance()->Load("~res:/TestData/LoadImageTest/a8_norle_bottomleft.tga", imgSet) == DAVA::eErrorCode::SUCCESS);
    TEST_VERIFY(imgSet[0]->GetPixelFormat() == PixelFormat::FORMAT_A8);
    ClearImgSet();

    TEST_VERIFY(DAVA::ImageSystem::Instance()->Load("~res:/TestData/LoadImageTest/10x10_rgba8888.tga", imgSet) == DAVA::eErrorCode::SUCCESS);
    TEST_VERIFY(imgSet[0]->GetPixelFormat() == PixelFormat::FORMAT_RGBA8888);
    TEST_VERIFY(imgSet[0]->dataSize == 4 * 10 * 10);
    TEST_VERIFY(Memcmp(imgSet[0]->data, &tga10x10data, tga10x10data.size()) == 0);
    ClearImgSet();

    TEST_VERIFY(DAVA::ImageSystem::Instance()->Load("~res:/TestData/LoadImageTest/10x10_rgba8888_norle.tga", imgSet) == DAVA::eErrorCode::SUCCESS);
    TEST_VERIFY(imgSet[0]->GetPixelFormat() == PixelFormat::FORMAT_RGBA8888);
    TEST_VERIFY(imgSet[0]->dataSize == 4 * 10 * 10);
    TEST_VERIFY(Memcmp(imgSet[0]->data, &tga10x10data, tga10x10data.size()) == 0);
    ClearImgSet();
}

DAVA_TEST(WebPTest)
{
    DAVA::Vector<DAVA::Image*> imgSet;
    auto ClearImgSet = [&imgSet]()
    {
        for (auto image : imgSet)
        {
            image->Release();
        }
        imgSet.clear();
    };

    TEST_VERIFY(DAVA::ImageSystem::Instance()->Load("~res:/TestData/LoadImageTest/rgb888.webp", imgSet) == DAVA::eErrorCode::SUCCESS);
    TEST_VERIFY(imgSet[0]->GetPixelFormat() == PixelFormat::FORMAT_RGB888);
    ClearImgSet();

    TEST_VERIFY(DAVA::ImageSystem::Instance()->Load("~res:/TestData/LoadImageTest/rgba8888.webp", imgSet) == DAVA::eErrorCode::SUCCESS);
    TEST_VERIFY(imgSet[0]->GetPixelFormat() == PixelFormat::FORMAT_RGBA8888);
    ClearImgSet();
}
}
;
