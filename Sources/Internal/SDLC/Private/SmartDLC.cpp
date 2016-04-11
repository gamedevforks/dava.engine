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

#include "SDLC/SmartDLC.h"
#include "SDLC/Private/PacksDB.h"

namespace DAVA
{
SmartDlc::PackState::PackState(const String& name, Status state, float priority, float progress)
    : name(name)
    , state(state)
    , priority(priority)
    , downloadProgress(progress)
{
}

static bool PackCompare(const SmartDlc::PackState* lhs, SmartDlc::PackState* rhs)
{
    return lhs->priority < rhs->priority;
}

class SmartDlcImpl
{
public:
    uint32 GetPackIndex(const String& packName)
    {
        auto it = packsIndex.find(packName);
        if (it != end(packsIndex))
        {
            return it->second;
        }
        throw std::runtime_error("can't find pack with name: " + packName);
    }
    SmartDlc::PackState& GetPackState(const String& packName)
    {
        uint32 index = GetPackIndex(packName);
        return packs[index];
    }
    void AddToDownloadQueue(const String& packName)
    {
        uint32 index = GetPackIndex(packName);
        SmartDlc::PackState* packState = &packs[index];
        queue.push_back(packState);
        std::stable_sort(begin(queue), end(queue), PackCompare);

        if (!IsDownloading())
        {
            StartNextPackDownloading();
        }
    }
    void UpdateQueuePriority(const String& packName, float priority)
    {
        uint32 index = GetPackIndex(packName);
        SmartDlc::PackState* packState = &packs[index];
        auto it = std::find(begin(queue), end(queue), packState);
        if (it != end(queue))
        {
            packState->priority = priority;
            std::stable_sort(begin(queue), end(queue), PackCompare);
        }
    }
    void UpdateCurrentDownload()
    {
        if (currentDownload != nullptr)
        {
            DownloadManager* dm = DownloadManager::Instance();
            DownloadStatus status = DL_UNKNOWN;
            dm->GetStatus(downloadHandler, status);
            uint64 progress = 0;
            switch (status)
            {
            case DL_IN_PROGRESS:
                if (dm->GetProgress(downloadHandler, progress))
                {
                    uint64 total = 0;
                    if (dm->GetTotal(downloadHandler, total))
                    {
                        currentDownload->downloadProgress = static_cast<float>(progress) / total;
                        // fire event on update progress
                        sdlcPublic->onPackStateChanged.Emit(*currentDownload);
                    }
                }
                break;
            case DL_FINISHED:
            {
                // now mount archive
                // validate it
                FileSystem* fs = FileSystem::Instance();
                FilePath archivePath = packsDB + currentDownload->name;
                fs->Mount(archivePath, "Data");

                currentDownload->state = SmartDlc::PackState::Mounted;
                currentDownload = nullptr;
                downloadHandler = 0;
            }
            break;
            default:
                break;
            }
        }
    }
    bool IsDownloading() const
    {
        return currentDownload != nullptr;
    }
    void StartNextPackDownloading()
    {
        if (!queue.empty())
        {
            currentDownload = queue.front();
            queue.erase(std::remove(begin(queue), end(queue), currentDownload));

            // start downloading
            auto fullUrl = remotePacksUrl + currentDownload->name;

            FilePath archivePath = packsDB + currentDownload->name;

            DownloadManager* dm = DownloadManager::Instance();
            downloadHandler = dm->Download(fullUrl, archivePath);
        }
    }
    void MountDownloadedPacks()
    {
        FileSystem* fs = FileSystem::Instance();
        for (auto& pack : packs)
        {
            // mount pack
            if (pack.state == SmartDlc::PackState::Mounted)
            {
                FilePath archiveName = localPacksDir + pack.name;
                fs->Mount(archiveName, "Data");
            }
        }
    }

    FilePath packsDB;
    FilePath localPacksDir;
    String remotePacksUrl;
    bool isProcessingEnabled = false;
    SmartDlc* sdlcPublic = nullptr;
    UnorderedMap<String, uint32> packsIndex;
    Vector<SmartDlc::PackState> packs;
    Vector<SmartDlc::PackState*> queue;
    SmartDlc::PackState* currentDownload = nullptr;
    std::unique_ptr<PacksDB> packDB;
    uint32 downloadHandler = 0;
};

SmartDlc::SmartDlc(const FilePath& packsDB, const FilePath& localPacksDir, const String& remotePacksUrl)
{
    impl.reset(new SmartDlcImpl());
    impl->packsDB = packsDB;
    impl->localPacksDir = localPacksDir;
    impl->remotePacksUrl = remotePacksUrl;
    impl->sdlcPublic = this;

    // open DB and load packs state then mount all archives to FileSystem
    impl->packDB.reset(new PacksDB(packsDB));
    impl->packDB->GetAllPacksState(impl->packs);
    impl->MountDownloadedPacks();
}

SmartDlc::~SmartDlc() = default;

bool SmartDlc::IsProcessingEnabled() const
{
    return impl->isProcessingEnabled;
}

void SmartDlc::EnableProcessing()
{
    impl->isProcessingEnabled = true;
}

void SmartDlc::DisableProcessing()
{
    impl->isProcessingEnabled = false;
}

void SmartDlc::Update()
{
    // first check if something downloading
    if (impl->isProcessingEnabled)
    {
        impl->UpdateCurrentDownload();

        if (!impl->IsDownloading())
        {
            impl->StartNextPackDownloading();
        }
    }
}

const String& SmartDlc::FindPack(const FilePath& relativePathInPack) const
{
    return impl->packDB->FindPack(relativePathInPack);
}

const SmartDlc::PackState& SmartDlc::GetPackState(const String& packID) const
{
    return impl->GetPackState(packID);
}

const SmartDlc::PackState& SmartDlc::RequestPack(const String& packID, float priority)
{
    priority = std::max(0.f, priority);
    priority = std::min(1.f, priority);

    auto& packState = impl->GetPackState(packID);
    if (packState.state == PackState::NotRequested)
    {
        packState.state = PackState::Queued;
        packState.priority = priority;
        impl->AddToDownloadQueue(packID);
    }
    else
    {
        impl->UpdateQueuePriority(packID, priority);
    }
    return packState;
}

const Vector<SmartDlc::PackState*>& SmartDlc::GetAllState() const
{
    static Vector<SmartDlc::PackState*> allState;

    allState.clear();
    allState.reserve(impl->packs.size());

    for (auto& state : impl->packs)
    {
        allState.push_back(&state);
    }

    return allState;
}

void SmartDlc::DeletePack(const String& packID)
{
    auto& state = impl->GetPackState(packID);
    if (state.state == PackState::Mounted)
    {
        // first modify DB
        state.state = PackState::NotRequested;
        state.priority = 0.5f;
        state.downloadProgress = 0.f;

        impl->packDB->UpdatePackState(state);

        // now remove archive from filesystem
        FileSystem* fs = FileSystem::Instance();
        FilePath archivePath = impl->packsDB + packID;
        fs->Unmount(archivePath);
    }
}
} // end namespace DAVA