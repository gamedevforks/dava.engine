#pragma once

#include <TArc/Core/ClientModule.h>
#include <TArc/Core/FieldBinder.h>
#include <TArc/DataProcessing/DataWrapper.h>
#include <TArc/DataProcessing/DataListener.h>

#include <TArc/Utils/QtConnections.h>

class FileSystemCacheModule : public DAVA::TArc::ClientModule, public DAVA::TArc::DataListener
{
    void PostInit() override;
    void OnWindowClosed(const DAVA::TArc::WindowKey& key) override;
    void OnDataChanged(const DAVA::TArc::DataWrapper& wrapper, const DAVA::Vector<DAVA::Any>& fields);

    void CreateActions();
    void OnFindFile();

    std::unique_ptr<DAVA::TArc::FieldBinder> projectUiPathFieldBinder;
    DAVA::TArc::QtConnections connections;
    DAVA::TArc::DataWrapper projectDataWrapper;

    DAVA_VIRTUAL_REFLECTION(FileSystemCacheModule, DAVA::TArc::ClientModule);
};
