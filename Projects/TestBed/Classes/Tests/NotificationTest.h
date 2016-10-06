#ifndef __NOTIFICATION_SCREEN_H__
#define __NOTIFICATION_SCREEN_H__

#include "Infrastructure/BaseScreen.h"

using namespace DAVA;

class TestBed;
class NotificationScreen : public BaseScreen, public TrackedObject
{
public:
    NotificationScreen(TestBed& app);

protected:
    ~NotificationScreen()
    {
    }

public:
    void LoadResources() override;
    void UnloadResources() override;
    void Draw(const UIGeometricData& geometricData) override;

    void UpdateNotification();

private:
    void Update(float32 timeElapsed);

    void OnNotifyText(BaseObject* obj, void* data, void* callerData);
    void OnNotifyTextDelayed(BaseObject* obj, void* data, void* callerData);
    void OnNotifyCancelDelayed(BaseObject* obj, void* data, void* callerData);
    void OnHideText(BaseObject* obj, void* data, void* callerData);
    void OnNotifyProgress(BaseObject* obj, void* data, void* callerData);
    void OnHideProgress(BaseObject* obj, void* data, void* callerData);

    void OnNotificationTextPressed(BaseObject* obj, void* data, void* callerData);
    void OnNotificationProgressPressed(BaseObject* obj, void* data, void* callerData);

private:
    UIButton* showNotificationText;
    UIButton* showNotificationTextDelayed;
    UIButton* cancelDelayedNotifications;
    UIButton* hideNotificationText;
    UIButton* showNotificationProgress;
    UIButton* hideNotificationProgress;

    LocalNotificationProgress* notificationProgress;
    LocalNotificationText* notificationText;

    uint32 progress;
};

#endif
