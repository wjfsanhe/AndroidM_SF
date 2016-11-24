/*
 * Copyright (C) 2011 The Android Open Source Project
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

#include <utils/Log.h>
#include <stdint.h>
#include <sys/types.h>
#include <binder/Parcel.h>
#include <binder/IInterface.h>
#include "IVrSDKService.h"

namespace android {
enum {
        CLIENT_RESUME = IBinder::FIRST_CALL_TRANSACTION,
        CLIENT_PAUSE,
        GET_VID,
	GET_SN = IBinder::FIRST_CALL_TRANSACTION + 6,
/*      GET_PID,
        GET_MCU_VERSION,
        GET_BLE_VERSION,
        GET_SN,
        START_UPGRADE,
        GET_UPGRADE_PROGRESS,
        IS_UPGRADING,
        GET_SKIN_LIST,
        LOAD_ONE_SKIN,*/
	ADD_SOCKET_TARGET = IBinder::FIRST_CALL_TRANSACTION + 18, 
	REMOVE_SOCKET_TARGET = IBinder::FIRST_CALL_TRANSACTION + 19,
	SOCKET_HEARTBEAT  = IBinder::FIRST_CALL_TRANSACTION + 20,
 
};
//const  String16 IVrSDKService::mToken("com.baofeng.mojing.service.MojingSDKAIDLService");
const  String16 IVrSDKService::mToken("com.example.wjf.serviceengine.IRemoteService");

class BpVrSDKService:public BpInterface<IVrSDKService>
{
public:
        BpVrSDKService(const sp<IBinder>&impl)
        :BpInterface<IVrSDKService>(impl)
        {}
        virtual void clientResume(const String16& params,const sp<IServiceResponse>& serviceResponse){
                Parcel data, reply;
                data.writeInterfaceToken(IVrSDKService::mToken);
                data.writeString16(params);
                data.writeStrongBinder(serviceResponse->asBinder(serviceResponse));
                if (remote()->transact(CLIENT_RESUME, data, &reply) != NO_ERROR) {
                        ALOGD("CLIENT_RESUME  error \n");
                        return ;
                }
        }
        virtual void clientPause(const String16& params)
        {
                Parcel data, reply;
                data.writeInterfaceToken(IVrSDKService::mToken);
                data.writeString16(params);
                if (remote()->transact(CLIENT_PAUSE, data, &reply) != NO_ERROR) {
                        ALOGD("clientPause  error \n");
                        return ;
                }
        }
	virtual String16 getSN()
	{
		Parcel data, reply;
		data.writeInterfaceToken(IVrSDKService::mToken);
		if (remote()->transact(GET_SN, data, &reply) != NO_ERROR) {
			ALOGD("get SN  error \n");
			return String16("");
		}
		int32_t err = reply.readExceptionCode();
		if (err < 0) {
			ALOGD("get SN caught exception %d\n", err);
			return String16("");
		}
		return reply.readString16();
	}
	virtual int getVID()
	{
		Parcel data, reply;
		data.writeInterfaceToken(IVrSDKService::mToken);
		if (remote()->transact(GET_VID, data, &reply) != NO_ERROR) {
			ALOGD("getVID  error \n");
			return -1;
		}
		int32_t err = reply.readExceptionCode();
		if (err < 0) {
			ALOGD("getVID caught exception %d\n", err);
			return -1;
		}
		return reply.readInt32();
	}

	virtual void addSocketTarget(int32_t port){
		Parcel data,reply ;
		data.writeInterfaceToken(IVrSDKService::mToken);
		data.writeInt32(port);
		if (remote()->transact(ADD_SOCKET_TARGET , data, &reply) != NO_ERROR) {
			ALOGD("Add socket target  error \n");
			return ;
		}
		int32_t err = reply.readExceptionCode();

		if (err < 0) {
			ALOGD("add socket target error,  caught exception %d\n", err);
			return ;
		}

	}

	virtual void removeSocketTarget(int32_t port){
		Parcel data,reply ;
		data.writeInterfaceToken(IVrSDKService::mToken);
		data.writeInt32(port);
		if (remote()->transact(REMOVE_SOCKET_TARGET , data, &reply) != NO_ERROR) {
			ALOGD("remove socket target  error \n");
			return ;
		}
		int32_t err = reply.readExceptionCode();

		if (err < 0) {
			ALOGD("remove socket target error,  caught exception %d\n", err);
			return ;
		}

	}
	virtual void socketHeartbeat(int32_t port){
		Parcel data,reply ;
		data.writeInterfaceToken(IVrSDKService::mToken);
		data.writeInt32(port);
		if (remote()->transact(SOCKET_HEARTBEAT , data, &reply) != NO_ERROR) {
			ALOGD("SOCKET_HEARTBEAT  error \n");
			return ;
		}
		int32_t err = reply.readExceptionCode();

		if (err < 0) {
			ALOGD("SOCKET_HEARTBEAT error,  caught exception %d\n", err);
			return ;
		}

	}
	
	virtual bool isBinderAlive() {
		return remote()->isBinderAlive();
	}

};


IMPLEMENT_META_INTERFACE(VrSDKService, "com.baofeng.mojing.service.MojingSDKAIDLService");

}

