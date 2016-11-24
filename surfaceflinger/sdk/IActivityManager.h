#ifndef ANDROID_IACTIVITYMANAGER_H_
#define ANDROID_IACTIVITYMANAGER_H_

#include <utils/Errors.h>
#include <binder/IInterface.h>

namespace android {

// ----------------------------------------------------------------------------
class IServiceConnection;
// must be kept in sync with interface defined in IInputManager.aidl
class IActivityManager : public IInterface
{
public:
    DECLARE_META_INTERFACE(ActivityManager);
    virtual String16  startService(String16& action, String16& resolvedType, String16& callingPackage,String16& component)=0;
    virtual int32_t bindService(String16& action, String16& resolvedType, String16& callingPackage,String16& component,sp<IServiceConnection> connection)=0;
    virtual sp<IBinder>  peekService(String16& action, String16& resolvedType, String16& callingPackage,String16& component)=0;
           

    virtual bool isBinderAlive() = 0;
};

// ----------------------------------------------------------------------------

} // namespace android

#endif // ANDROID_IACTIVITYMANAGER_H_
