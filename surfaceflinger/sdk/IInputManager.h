#ifndef ANDROID_IINPUTMANAGER_H_
#define ANDROID_IINPUTMANAGER_H_

#include <utils/Errors.h>
#include <binder/IInterface.h>
#include <input/Input.h>

namespace android {

// ----------------------------------------------------------------------------

// must be kept in sync with interface defined in IInputManager.aidl
class IInputManager : public IInterface
{
public:
    DECLARE_META_INTERFACE(InputManager);

    // Gets input device information.
    //InputDevice getInputDevice(int deviceId);
    //int[] getInputDeviceIds();

    // Reports whether the hardware supports the given keys; returns true if successful
    //boolean hasKeys(int deviceId, int sourceMask, in int[] keyCodes, out boolean[] keyExists);

    // Temporarily changes the pointer speed.
    //void tryPointerSpeed(int speed);

    // Injects an input event into the system.  To inject into windows owned by other
    // applications, the caller must have the INJECT_EVENTS permission.
    virtual bool injectInputEvent(const MotionEvent *ev, int mode) =0;

    // Calibrate input device position
    //TouchCalibration getTouchCalibrationForInputDevice(String inputDeviceDescriptor, int rotation);
    //void setTouchCalibrationForInputDevice(String inputDeviceDescriptor, int rotation,
    //        in TouchCalibration calibration);

    // Keyboard layouts configuration.
    //KeyboardLayout[] getKeyboardLayouts();
    //KeyboardLayout getKeyboardLayout(String keyboardLayoutDescriptor);
    //String getCurrentKeyboardLayoutForInputDevice(in InputDeviceIdentifier identifier);
    //void setCurrentKeyboardLayoutForInputDevice(in InputDeviceIdentifier identifier,
    //        String keyboardLayoutDescriptor);
    //String[] getKeyboardLayoutsForInputDevice(in InputDeviceIdentifier identifier);
    //void addKeyboardLayoutForInputDevice(in InputDeviceIdentifier identifier,
    //        String keyboardLayoutDescriptor);
    //void removeKeyboardLayoutForInputDevice(in InputDeviceIdentifier identifier,
    //        String keyboardLayoutDescriptor);

    // Registers an input devices changed listener.
    //void registerInputDevicesChangedListener(IInputDevicesChangedListener listener);

    // Input device vibrator control.
    //void vibrate(int deviceId, in long[] pattern, int repeat, IBinder token);
    //void cancelVibrate(int deviceId, IBinder token);
    virtual bool isBinderAlive() = 0;
};

// ----------------------------------------------------------------------------

} // namespace android

#endif // ANDROID_IINPUTMANAGER_H_
