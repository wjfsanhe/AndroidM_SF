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
#include "IServiceResponse.h"

namespace android {
enum {
        SENSOR_SUCCESS = IBinder::FIRST_CALL_TRANSACTION,
        SENSOR_FAILED,
        SENSOR_STARTED,
        SENSOR_STOPPED,
        EVENT,
};
//IBinder::FLAG_ONEWAY asynchronous call
class BpServiceResponse:public BpInterface<IServiceResponse>
{
public:
        BpServiceResponse(const sp<IBinder>&impl)
        :BpInterface<IServiceResponse>(impl){}
        virtual void sensorSuccess(const String16& params){
                Parcel data,reply ;
                data.writeInterfaceToken(IServiceResponse::getInterfaceDescriptor());
                data.writeString16(params);
                remote()->transact(SENSOR_SUCCESS, data, &reply, IBinder::FLAG_ONEWAY);
                int32_t err = reply.readExceptionCode();
                if(err<0){
                        ALOGD("sensor success return %d\n",err);
                }
        }
        virtual void sensorFailed(const String16& params){
                Parcel data,reply ;
                data.writeInterfaceToken(IServiceResponse::getInterfaceDescriptor());
                data.writeString16(params);
                remote()->transact(SENSOR_FAILED, data, &reply, IBinder::FLAG_ONEWAY);
                int32_t err = reply.readExceptionCode();
                if(err<0){
                        ALOGD("sensor failed return %d\n",err);
                }
        }
        virtual void sensorStarted(const String16& params){
                Parcel data,reply ;
                data.writeInterfaceToken(IServiceResponse::getInterfaceDescriptor());
                data.writeString16(params);
                remote()->transact(SENSOR_STARTED, data, &reply, IBinder::FLAG_ONEWAY);
                int32_t err = reply.readExceptionCode();
                if(err<0){
                        ALOGD("sensor started return %d\n",err);
                }
        }
        virtual void sensorStopped(){
                Parcel data,reply ;
                data.writeInterfaceToken(IServiceResponse::getInterfaceDescriptor());
                remote()->transact(SENSOR_STOPPED, data, &reply, IBinder::FLAG_ONEWAY);
                int32_t err = reply.readExceptionCode();
                if(err<0){
                        ALOGD("sensor started return %d\n",err);
                }
        }
        virtual void event(const String16& msg){
                Parcel data,reply ;
                data.writeInterfaceToken(IServiceResponse::getInterfaceDescriptor());
                data.writeString16(msg);
                remote()->transact(EVENT, data, &reply, IBinder::FLAG_ONEWAY);
                int32_t err = reply.readExceptionCode();
                if(err<0){
                        ALOGD("sensor started return %d\n",err);
                }
        }
};
IMPLEMENT_META_INTERFACE(ServiceResponse, "com.baofeng.mojing.service.IServiceResponse")
status_t BnServiceResponse::onTransact(uint32_t code,const Parcel& data,Parcel* reply,uint32_t flags){
        switch(code){
        case SENSOR_SUCCESS:{
                        CHECK_INTERFACE(IServiceResponse, data, reply);
                        String16 para = data.readString16();
                        sensorSuccess(para);
                        reply->writeInt32(0);
                        return NO_ERROR;
                } break;
        case SENSOR_FAILED:{
                        CHECK_INTERFACE(IServiceResponse, data, reply);
                        String16 para = data.readString16();
                        sensorFailed(para);
                        reply->writeInt32(0);
                        return NO_ERROR;
                }break;
                case SENSOR_STARTED:{
                        CHECK_INTERFACE(IServiceResponse, data, reply);
                        String16 para = data.readString16();
                        sensorStarted(para);
                        reply->writeInt32(0);
                        return NO_ERROR;
                }break;
                case SENSOR_STOPPED:{
                        CHECK_INTERFACE(IServiceResponse, data, reply);
                        sensorStopped();
                        reply->writeInt32(0);
                        return NO_ERROR;
                }break;
                case EVENT:{
                        CHECK_INTERFACE(IServiceResponse, data, reply);
                        String16 para = data.readString16();
                        event(para);
                        reply->writeInt32(0);
                        return NO_ERROR;
                }break;
        }
        return BBinder::onTransact(code,data,reply,flags) ;
}


}

