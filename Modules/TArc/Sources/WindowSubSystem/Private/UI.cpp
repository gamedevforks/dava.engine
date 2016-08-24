#include "WindowSubSystem/UI.h"

#include <QUrl>

namespace DAVA
{
namespace TArc
{
WindowKey::WindowKey(const FastName& appID_)
    : appID(appID_)
{
}

const FastName& WindowKey::GetAppID() const
{
    return appID;
}

DockPanelInfo::DockPanelInfo()
    : actionPlacementInfo(DAVA::TArc::CreateMenuPoint("View/Dock"))
{
}

PanelKey::PanelKey(const QString& viewName_, const DockPanelInfo& info_)
    : PanelKey(DockPanel, viewName_, info_)
{
}

PanelKey::PanelKey(const QString& viewName_, const CentralPanelInfo& info_)
    : PanelKey(CentralPanel, viewName_, info_)
{
}

PanelKey::PanelKey(Type t, const QString& viewName_, const Any& info_)
    : type(t)
    , viewName(viewName_)
    , info(info_)
{
}

const QString& PanelKey::GetViewName() const
{
    return viewName;
}

PanelKey::Type PanelKey::GetType() const
{
    return type;
}

const Any& PanelKey::GetInfo() const
{
    return info;
}

ActionPlacementInfo::ActionPlacementInfo(const QUrl& url)
{
    AddPlacementPoint(url);
}

void ActionPlacementInfo::AddPlacementPoint(const QUrl& url)
{
    urls.emplace_back(url);
}

const Vector<QUrl>& ActionPlacementInfo::GetUrls() const
{
    return urls;
}
} // namespace TArc
} // namespace DAVA