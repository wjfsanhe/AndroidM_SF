#ifndef ANDROID_III_VR_SERVICE_H
#define  ANDROID_III_VR_SERVICE_H
#include "IVrSDKService.h"
#include <binder/Parcel.h>
#include <utils/threads.h>
//#include "../SurfaceFlinger.h"
#include "IServiceConnection.h"
// ---------------------------------------------------------------------------
namespace android {
// ---------------------------------------------------------------------------

class SurfaceFlinger;
class BnServiceConnection;
class VrService : public Thread {
    //method group
    public:
	VrService(sp<SurfaceFlinger> sur);
	int32_t setServerPort(int32_t port);
    private:
	~VrService();
        virtual bool        threadLoop();
        virtual void        onFirstRef();
	void triggerHeartbeat(void);
	int32_t processVrService(void);
    class Connection : public BnServiceConnection {
	public:
	
	virtual void onServiceConnected(String16 name, sp<IBinder> service);
	private:
	virtual void binderDied(const wp<IBinder>& who);
    }; 
    //property group
    private:
	int32_t mPort ;
	int32_t mState;
	sp<Connection> mConnectionCallback;
	sp<IVrSDKService>  mVrSDKService; 
        sp<SurfaceFlinger> mSF;


};


// ---------------------------------------------------------------------------

}; // namespace android

// ---------------------------------------------------------------------------

#endif
