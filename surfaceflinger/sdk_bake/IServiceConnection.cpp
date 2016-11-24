/*
 * Copyright 2014 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <binder/Parcel.h>
#include <utils/Log.h>
#include "IServiceConnection.h"
#include "sfLog.h"
namespace android {
enum {
	TRANSACTION_connected = IBinder::FIRST_CALL_TRANSACTION + 0,
};
#define  TAG   "BnServiceConnection"


class BpServiceConnection:public BpInterface<IServiceConnection>
{
public:
        BpServiceConnection(const sp<IBinder>&impl)
        :BpInterface<IServiceConnection>(impl){}
	virtual void onServiceConnected(String16 name, sp<IBinder> service){
	}
};

IMPLEMENT_META_INTERFACE(ServiceConnection, "android.app.IServiceConnection")

status_t BnServiceConnection::onTransact(uint32_t code,const Parcel& data,Parcel* reply,uint32_t flags){
        switch(code){
        	case TRANSACTION_connected:
		SF_LOG(TAG,"service connected be called");
		
               	CHECK_INTERFACE(IServiceConnection, data, reply);
		//check component name 
		String16  packageName("null") ;
		String16  className("null") ;
		if ((0!=data.readInt32())) {
			//read package
			packageName=data.readString16();
			className=data.readString16();
		}
	
		SF_LOG(TAG,"service connected from: %s %s",String8(packageName).string() , String8(className).string());
		sp<IBinder> binder = reply->readStrongBinder();
		onServiceConnected(packageName+className,binder);
		break;
              
        }
        return BBinder::onTransact(code,data,reply,flags) ;
}
}

