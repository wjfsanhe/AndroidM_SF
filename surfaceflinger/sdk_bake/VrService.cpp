#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <cutils/sockets.h>
#include <cutils/log.h>
#include <utils/String16.h>
#include <utils/String8.h>
#include <binder/IServiceManager.h>
#include <utils/Log.h>
#include <sys/socket.h>
#include <binder/Parcel.h>
#include <time.h>
#include "VrService.h"
#include "IActivityManager.h"
#include "SurfaceFlinger.h"
#include "sfLog.h"
#include "TrackerThread.h"
//#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
//#include <linux/if_packet.h>
//#include <linux/if_ether.h>
#include <errno.h>

namespace android {
#define TAG   "VRSDKService"
#define S_START  (-2)
#define S_INIT   (-1)
enum {
	S_SERVICE_PEEKED = 0,
	S_RUNNING ,
	S_NEED_RESTART ,
};      
	void VrService::Connection::onServiceConnected(String16 name, sp<IBinder> service){
		SF_LOG(TAG,"bind service : service connected be called!!!!!!!!!!!!......") ;
	}
	void VrService::Connection::binderDied(const wp<IBinder>&who){
		SF_LOG(TAG,"paired binder died") ;
	}


	VrService::VrService(sp<SurfaceFlinger> sur){
		mSF=sur;
		mPort= 0xa5a5a5a5;
		//mState = S_INIT;
		mState = S_START;
		mConnectionCallback= new Connection();	
	}
	VrService::~VrService(){
	}
	
        void VrService::onFirstRef() {
                run("VrService", PRIORITY_URGENT_DISPLAY + PRIORITY_MORE_FAVORABLE);
        }
	
	
        bool VrService::threadLoop() {

		processVrService() ;

		switch (mState) {
			case S_START:
			usleep(1000000);
			break;
			case S_INIT:
			SF_LOG(TAG,"start Service ---- ") ;
			usleep(100000);
			break;
			case S_NEED_RESTART:
			SF_LOG(TAG,"restart Service  ") ;
			usleep(1000000) ;
			break ;
			case S_RUNNING:
			SF_LOG(TAG,"trigger heartBeat  ") ;
			usleep(5000000) ;
			break ;
			case S_SERVICE_PEEKED:
			SF_LOG(TAG,"peek service success  ") ;
			usleep(100000) ;
			break ;
			
		}
		/*
		int fd = socket(AF_INET, SOCK_STREAM, 0);
                if (fd < 0 ){
			SF_LOG(TAG,"create socket fail  %d ,%s ", errno,strerror(errno));
		}else{
			SF_LOG(TAG,"create socket ok  %d ", fd);
		}	
		usleep(1000000);*/
                return true;
        }
	void VrService::triggerHeartbeat(void){
		if (mVrSDKService != NULL) {
			mVrSDKService->socketHeartbeat(mPort); 
		}
	}
	int32_t VrService::setServerPort(int32_t port){
		if (mVrSDKService != NULL) {
			SF_LOG(TAG,"set ServerPort %d  ",port) ;
			mVrSDKService->addSocketTarget(port); 
			return 0;
		}
		return -1;
	}
//we start system service here
int32_t  VrService::processVrService(){

	switch (mState){
		case S_START:
		case  S_INIT:
			{ 
				//String16 action("com.baofeng.mojing.service.MojingSDKAIDLService");
				//String16 packageName("com.baofeng.mj");
				String16 action("com.example.wjf.serviceengine.IRemoteService");
				String16 packageName("com.example.wjf.serviceengine");
				String16 component("com.example.wjf.serviceengine.RemoteService");
				String16 resolveType("NULL");
				sp<IServiceManager> sm = defaultServiceManager();  
				Vector<String16> services = sm->listServices();
			for ( int i = 0; i < services.size(); i++) {
					SF_LOG(TAG,"service %d: %s",i,String8(services[i]).string());
				}
				sp<IBinder> binder = sm->getService(String16("activity"));  
				//we use ams to peek SDKAIDLService.

				if (binder == NULL) {
					ALOGW("can't getService  activityManager");
					return mState ;
				}

				sp<IActivityManager>  ams = IActivityManager::asInterface(binder);

				SF_LOG(TAG,"Got activityManager service success2 ");
				if (mState == S_INIT){
					binder = ams->peekService(action,resolveType,packageName,component);
					if ( binder == NULL ){
						SF_LOG(TAG,"Got VRSDK service fail ");
						//mState=S_START ;
						mState=S_INIT ;
						return mState ;
					}else{
						sp<IVrSDKService> sdkService= IVrSDKService::asInterface(binder);
						SF_LOG(TAG,"Got VRSDK service success ");
						SF_LOG(TAG,"SDK service version 0x%x ",sdkService->getVID());
						//SF_LOG(TAG,"SDK service SN %s ",String8(sdkService->getSN()).string());
						mVrSDKService = sdkService ;
						if(mSF != NULL){
							SF_LOG(TAG,"Got server port %d from service ",mSF->getServerPort()); 
						}
						mState=S_SERVICE_PEEKED ;
					}
				}else if(mState == S_START){
					//String16 componentRet=ams->startService(action,resolveType,packageName,component);
					int32_t ret=ams->bindService(action,resolveType,packageName,component,mConnectionCallback);
					if (ret < 0){
						SF_LOG(TAG,"bind  service fail return :%d",ret); 
					}else{
						SF_LOG(TAG,"bind service success :%d",ret); 
						mState=S_INIT ;
					}

				}
			}
			break ;

		case S_SERVICE_PEEKED:
			{
				int32_t port = 16384 ;//mSF->getServerPort();
				if(port != 0 ){
					SF_LOG(TAG,"Got server port %d ,ok ", port);
					mPort=port;
					setServerPort(port);
					mState = S_RUNNING ;		
				}else{
					SF_LOG(TAG,"Got server port %d ,failed ", port);
				}
				mSF->mTrackerThread->startThread();
				mSF->mTrackerThread->setServerPort(port);
				/*int fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
				if (fd < 0)
				{
					SF_LOG(TAG,"create socket fail  %d ,%s ", errno,strerror(errno));
				}else{
					SF_LOG(TAG,"create socket success  %d ", fd);
				}*/
			}
			break ;
		case S_RUNNING :
			triggerHeartbeat();
			break ;
		case S_NEED_RESTART:
			//we restart service here.
			SF_LOG(TAG,"restart service ------- "); 
			break ;
		default :
			break;
	}

	//detect if service loss.
	if (mVrSDKService!=NULL && !mVrSDKService->isBinderAlive()){
		SF_LOG(TAG,"Need restart service "); 
		mVrSDKService = NULL;
		mPort=0xa5a5a5a5;
		mState=S_NEED_RESTART ;
	}

	return mState ;

}

}
