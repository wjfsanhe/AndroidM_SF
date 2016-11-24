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
 *  @date     2016/11/18
 *
 *
 */

#ifndef ANDROID_ISERVICE_CONNECTION_H
#define ANDROID_ISERVICE_CONNECTION_H 

#include <utils/Errors.h>
#include <utils/RefBase.h>
#include <binder/IInterface.h>
#include <utils/String8.h>
#include <utils/String16.h>

namespace android {

// ----------------------------------------------------------------------------

class ServiceConnection : public virtual RefBase
{
public:
	ServiceConnection(){}
	virtual ~ServiceConnection(){}
	/**
	 * Called when a connection to the Service has been established, with
	 * the {@link android.os.IBinder} of the communication channel to the
	 * Service.
	 *
	 * @param name The concrete component name of the service that has
	 * been connected.
	 *
	 * @param service The IBinder of the Service's communication channel,
	 * which you can now make calls on.
	 */
	virtual void onServiceConnected(String16 name, sp<IBinder> service)=0;
};
class IServiceConnection : public ServiceConnection,public IInterface
{
public:
        DECLARE_META_INTERFACE(ServiceConnection)
};

class BnServiceConnection:public BnInterface<IServiceConnection>,private IBinder::DeathRecipient
{
	public:
		virtual status_t onTransact(uint32_t code, const Parcel& data,
				Parcel* reply, uint32_t flags = 0);
	private:
		// This is required by the IBinder::DeathRecipient interface
		// Bp ternimal can linktoDeath.
		virtual void binderDied(const wp<IBinder>& who)=0;
};

// ----------------------------------------------------------------------------

}; // namespace android

#endif // ANDROIDOID_ISERVICE_CONNECTION_H_


