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
 *  @author   wangjf
 *  @version  1.0
 *  @date     2016/09/01
 *
 *
 */

#ifndef ANDROID_IVRSDKSERVICE_H
#define ANDROID_IVRSDKSERVICE_H

#include <utils/Errors.h>
#include <binder/IInterface.h>
#include <utils/String8.h>
#include <utils/String16.h>
#include "IServiceResponse.h"

namespace android {

// ----------------------------------------------------------------------------

// must be kept in sync with interface defined in I.aidl
class IVrSDKService : public IInterface
{
public:
    DECLARE_META_INTERFACE(VrSDKService);

    virtual void clientResume(const String16& params,const sp<IServiceResponse> serviceResponse){} ;
    virtual void clientPause(const String16& params)=0;
    virtual int32_t  getVID()=0;
    virtual void addSocketTarget(int32_t port)=0;
    virtual void removeSocketTarget(int32_t port)=0;
    virtual void socketHeartbeat(int32_t port)=0;
    //virtual int32_t  getPID()=0;
    //virtual int getMCUVersion()=0;
    //virtual int getBLEVersion()=0;
    virtual String16 getSN()=0;
    //virtual bool startUpgrade(String16 path)=0;
    //virtual float getUpgradeProgress()=0;
    //virtual bool  isUpgrading()=0;
    //virtual String16 getSkinList()=0;
    //virtual int32_t loadOneSkin(String16 skinName)=0;
    virtual bool isBinderAlive() = 0;
public:
    static const String16 mToken;
};

//const  String16 IVrSDKService::mToken("com.baofeng.mojing.service.MojingSDKAIDLService");

// ----------------------------------------------------------------------------
class BnVrSDKService : public BnInterface<IVrSDKService>
{
public:
    virtual status_t    onTransact( uint32_t code,
                                    const Parcel& data,
                                    Parcel* reply,
                                    uint32_t flags = 0);
};


}; // namespace android

#endif // ANDROID_IVRSDKSERVICE_H
