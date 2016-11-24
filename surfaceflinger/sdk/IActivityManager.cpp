
#include <binder/Parcel.h>
#include <utils/String8.h> 
#include "IActivityManager.h"
#include "IServiceConnection.h"
#include "sfLog.h"
namespace android {

enum {
    START_SERVICE_TRANSACTION = IBinder::FIRST_CALL_TRANSACTION + 33,
    //we use extended interface!!!!! Never allow change it unless you know what you are doing .	
    BIND_SERVICE_TRANSACTION = IBinder::FIRST_CALL_TRANSACTION + 302, 
    PEEK_SERVICE_TRANSACTION = IBinder::FIRST_CALL_TRANSACTION + 84,
    
};

class BpActivityManager : public BpInterface<IActivityManager>
{
#define  TAG "BpActivityManager"
public:
    BpActivityManager(const sp<IBinder>& impl)
        : BpInterface<IActivityManager>(impl)
    {
    }
	virtual int32_t bindService(String16& action, String16& resolvedType, String16& callingPackage,String16& component,sp<IServiceConnection> connection){
		Parcel data, reply;
                
		data.writeInterfaceToken(String16("android.app.IActivityManager"));
		data.writeStrongBinder(NULL); //caller
		data.writeStrongBinder(NULL); //activity Token.

		/*start of intent */
		data.writeString16(action,action.size()); /* action */
		data.writeInt32(0); /* Uri - type no url */
		//data.writeString16(String16("baofeng_mojing"));
		data.writeString16(NULL, 0); /* type */
		data.writeInt32(0); /* flags */
		data.writeString16(callingPackage,callingPackage.size()); /* package name */
		data.writeString16(callingPackage,callingPackage.size()); /* package name */
		data.writeString16(component ,component.size()); /* package name */
		//data.writeString16(NULL, 0); /* ComponentName */
		data.writeInt32(0); /* source bound - size */
		data.writeInt32(0); /* Categories - size */
		data.writeInt32(0); /* selector - size */
		data.writeInt32(0); /* ClipData */
		data.writeInt32(getuid()); /*user hint*/
		data.writeInt32(0); /* bundle(extras) size */
		/* end of intent */
		
		
        	data.writeString16(resolvedType);
        	data.writeStrongBinder(IInterface::asBinder(connection));
		data.writeInt32(0x1); //BIND_AUTO_CREATE. CONTEXT.JAVA
		data.writeString16(callingPackage,callingPackage.size()); /* calling package */
                data.writeInt32(/*getuid()*/0); /*user id*/
		
		int err = remote()->transact(BIND_SERVICE_TRANSACTION,data,&reply);

                if (err != NO_ERROR) {
                        SF_LOG(TAG, "bind  service %s fail from package %s",String8(action).string(),
                                        String8(callingPackage).string());
                        return -1;
                }
                int32_t expCode=reply.readExceptionCode() ;
                if ( expCode != 0) {
                        SF_LOG(TAG, "bind service fail exception code %d",expCode);
                        return -1;
                }	
		
                SF_LOG(TAG, "bind service success !!!!");
		return 0;

	}
	virtual String16  startService(String16& action, String16& resolvedType, String16& callingPackage,String16& component) {
		Parcel data, reply;
		data.writeInterfaceToken(String16("android.app.IActivityManager"));
		data.writeStrongBinder(NULL);

		/*start of intent */
		data.writeString16(action,action.size()); /* action */
		data.writeInt32(0); /* Uri - type no url */
		//data.writeString16(String16("baofeng_mojing"));
		data.writeString16(NULL, 0); /* type */
		data.writeInt32(0); /* flags */
		data.writeString16(callingPackage,callingPackage.size()); /* package name */
		data.writeString16(callingPackage,callingPackage.size()); /* package name */
		data.writeString16(component ,component.size()); /* package name */
		//data.writeString16(NULL, 0); /* ComponentName */
		data.writeInt32(0); /* source bound - size */
		data.writeInt32(0); /* Categories - size */
		data.writeInt32(0); /* selector - size */
		data.writeInt32(0); /* ClipData */
		data.writeInt32(getuid()); /*user hint*/
		data.writeInt32(0); /* bundle(extras) size */
		/* end of intent */

		data.writeString16(resolvedType, resolvedType.size()); /* resolvedType */
		data.writeString16(callingPackage,callingPackage.size()); /* calling package */
		data.writeInt32(/*getuid()*/0); /*user id*/
		int err = remote()->transact(START_SERVICE_TRANSACTION,data,&reply);

		if (err != NO_ERROR) {
			SF_LOG(TAG, "start service %s fail from package %s",String8(action).string(),
					String8(callingPackage).string());
			return String16("start service error");
		}
		int32_t expCode=reply.readExceptionCode() ;
		if ( expCode != 0) {
			SF_LOG(TAG, "start service fail exception code %d",expCode);
			return String16("start service exception");
		}


		String16 packageName=data.readString16();
		String16 className=data.readString16();
		String16 Com(packageName+String16("/")+className) ;
		SF_LOG(TAG, "start service success %s",String8(Com).string());
		return Com;
	    
    }
    
    virtual sp<IBinder>  peekService(String16& action, String16& resolvedType, String16& callingPackage,String16& component){
	    Parcel data, reply;
	    
	    data.writeInterfaceToken(String16("android.app.IActivityManager"));
	    /*start of intent */
	    data.writeString16(action,action.size()); /* action */
	    data.writeInt32(0); /* Uri - type no url */
	    //data.writeString16(String16("baofeng_mojing"));
	    data.writeString16(NULL, 0); /* type */
	    data.writeInt32(0); /* flags */
	    data.writeString16(callingPackage,callingPackage.size()); /* package name */
	    data.writeString16(callingPackage,callingPackage.size()); /* package name */
	    data.writeString16(component ,component.size()); /* package name */
	    //data.writeString16(NULL, 0); /* ComponentName */
	    data.writeInt32(0); /* source bound - size */
	    data.writeInt32(0); /* Categories - size */
	    data.writeInt32(0); /* selector - size */
	    data.writeInt32(0); /* ClipData */
	    data.writeInt32(getuid()); /*user hint*/
	    data.writeInt32(0); /* bundle(extras) size */
	    /* end of intent */
	    data.writeString16(resolvedType, resolvedType.size()); /* resolvedType */
	    data.writeString16(callingPackage,callingPackage.size()); /* calling package */
	    int err = remote()->transact(PEEK_SERVICE_TRANSACTION,data,&reply);
	    if (err != NO_ERROR) {
		    SF_LOG(TAG, "peek service %s fail from package %s",String8(action).string(),
				    String8(callingPackage).string());
		    return NULL;
	    }
	    int32_t expCode=reply.readExceptionCode() ;
	    if ( expCode != 0) {
		    SF_LOG(TAG, "peek service fail exception code %d",expCode);
		    return NULL;
	    }
	    sp<IBinder> binder = reply.readStrongBinder();
	    SF_LOG(TAG, "peek service %s success from package %s",String8(action).string(),
			    String8(callingPackage).string());
	    return binder;
    }

    virtual bool isBinderAlive() {
        return remote()->isBinderAlive();
    }
};

IMPLEMENT_META_INTERFACE(ActivityManager, "android.app.IActivityManager");

// ----------------------------------------------------------------------------

}; // namespace android
