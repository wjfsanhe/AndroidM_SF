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

#ifndef ANDROID_ISERVICERESPONSE_H
#define ANDROID_ISERVICERESPONSE_H

#include <utils/Errors.h>
#include <utils/RefBase.h>
#include <binder/IInterface.h>
#include <utils/String8.h>
#include <utils/String16.h>

namespace android {

// ----------------------------------------------------------------------------

// must be kept in sync with interface defined in IServiceResponse.aidl
class ServiceResponse : public virtual RefBase
{
public:
        ServiceResponse(){}
        virtual ~ServiceResponse(){}
        //callback interface
        virtual void sensorSuccess(const String16& params)=0;
        virtual void sensorFailed(const String16& params)=0;
        virtual void sensorStarted(const String16& params)=0;
        virtual void sensorStopped()=0;
        virtual void event(const String16& data)=0;
};
class IServiceResponse : public ServiceResponse,public IInterface
{
public:
        DECLARE_META_INTERFACE(ServiceResponse)
};

class BnServiceResponse:public BnInterface<IServiceResponse>
{
public:
        virtual status_t onTransact(uint32_t code, const Parcel& data,
        Parcel* reply, uint32_t flags = 0);
};
class BaseServiceResponse:public BnServiceResponse
{
public:
        virtual void sensorSuccess(const String16& params);
        virtual void sensorFailed(const String16& params);
        virtual void sensorStarted(const String16& params);
        virtual void sensorStopped();
        virtual void event(const String16& data);
};

// ----------------------------------------------------------------------------

}; // namespace android

#endif // ANDROID_ISERVICERESPONSE_H


