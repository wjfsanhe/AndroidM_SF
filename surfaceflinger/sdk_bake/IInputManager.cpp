
#include <binder/Parcel.h>

#include "IInputManager.h"

namespace android {

/**
copy from
common/obj/JAVA_LIBRARIES/framework_intermediates/src/core/java/android/hardware/input/IInputManager.java

static final int TRANSACTION_getInputDevice = (android.os.IBinder.FIRST_CALL_TRANSACTION + 0);
static final int TRANSACTION_getInputDeviceIds = (android.os.IBinder.FIRST_CALL_TRANSACTION + 1);
static final int TRANSACTION_hasKeys = (android.os.IBinder.FIRST_CALL_TRANSACTION + 2);
static final int TRANSACTION_tryPointerSpeed = (android.os.IBinder.FIRST_CALL_TRANSACTION + 3);
static final int TRANSACTION_injectInputEvent = (android.os.IBinder.FIRST_CALL_TRANSACTION + 4);
static final int TRANSACTION_getTouchCalibrationForInputDevice = (android.os.IBinder.FIRST_CALL_TRANSACTION + 5);
static final int TRANSACTION_setTouchCalibrationForInputDevice = (android.os.IBinder.FIRST_CALL_TRANSACTION + 6);
static final int TRANSACTION_getKeyboardLayouts = (android.os.IBinder.FIRST_CALL_TRANSACTION + 7);
static final int TRANSACTION_getKeyboardLayout = (android.os.IBinder.FIRST_CALL_TRANSACTION + 8);
static final int TRANSACTION_getCurrentKeyboardLayoutForInputDevice = (android.os.IBinder.FIRST_CALL_TRANSACTION + 9);
static final int TRANSACTION_setCurrentKeyboardLayoutForInputDevice = (android.os.IBinder.FIRST_CALL_TRANSACTION + 10);
static final int TRANSACTION_getKeyboardLayoutsForInputDevice = (android.os.IBinder.FIRST_CALL_TRANSACTION + 11);
static final int TRANSACTION_addKeyboardLayoutForInputDevice = (android.os.IBinder.FIRST_CALL_TRANSACTION + 12);
static final int TRANSACTION_removeKeyboardLayoutForInputDevice = (android.os.IBinder.FIRST_CALL_TRANSACTION + 13);
static final int TRANSACTION_registerInputDevicesChangedListener = (android.os.IBinder.FIRST_CALL_TRANSACTION + 14);
static final int TRANSACTION_vibrate = (android.os.IBinder.FIRST_CALL_TRANSACTION + 15);
static final int TRANSACTION_cancelVibrate = (android.os.IBinder.FIRST_CALL_TRANSACTION + 16);

*/
enum {
    INPUT_getInputDevice = IBinder::FIRST_CALL_TRANSACTION,
    INPUT_getInputDeviceIds,
    INPUT_hasKeys,
    INPUT_tryPointerSpeed,
    INPUT_injectInputEvent,
    INPUT_getTouchCalibrationForInputDevice,
    INPUT_setTouchCalibrationForInputDevice,
    INPUT_getKeyboardLayouts,
    INPUT_getKeyboardLayout,
    INPUT_getCurrentKeyboardLayoutForInputDevice,
    INPUT_setCurrentKeyboardLayoutForInputDevice,
    INPUT_getKeyboardLayoutsForInputDevice,
    INPUT_addKeyboardLayoutForInputDevice,
    INPUT_removeKeyboardLayoutForInputDevice,
    INPUT_registerInputDevicesChangedListener,
    INPUT_vibrate,
    INPUT_cancelVibrate,
};
// copy from InputEvnt.java
#define PARCEL_TOKEN_MOTION_EVENT 1

class BpInputManager : public BpInterface<IInputManager>
{
public:
    BpInputManager(const sp<IBinder>& impl)
        : BpInterface<IInputManager>(impl)
    {
    }

    virtual bool injectInputEvent(const MotionEvent *event, int mode) {

        Parcel data, reply;
        data.writeInterfaceToken(IInputManager::getInterfaceDescriptor());

        data.writeInt32(1);
        data.writeInt32(PARCEL_TOKEN_MOTION_EVENT);
        event->writeToParcel(&data);
        /* Waits for the input event to be completely processed. */
        // INPUT_EVENT_INJECTION_SYNC_WAIT_FOR_FINISHED = 2,
        data.writeInt32(2);

        int err = remote()->transact(INPUT_injectInputEvent, data, &reply);

        if (err != NO_ERROR) {

            static const String16& str = IInputManager::getInterfaceDescriptor();
            static const char16_t* data = str.string();
            char buffer[1024] = {'\0'};
            for (size_t i = 0; i < str.size(); i ++) {
                buffer[i] = char(data[i]);
            }

            ALOGD("injectEvent (%d)(%s) error(%d)(%p)", (int)str.size(), buffer, err, remote());
            return false;
        }
        bool res = reply.readInt32();
        if (!res) {
            ALOGD("injectInputEvent false");
        }
        return res;
    }

    virtual bool isBinderAlive() {
        return remote()->isBinderAlive();
    }
};

IMPLEMENT_META_INTERFACE(InputManager, "android.hardware.input.IInputManager");

// ----------------------------------------------------------------------------

}; // namespace android
