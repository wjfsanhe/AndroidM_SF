#include "HeadTrackInputEventThread.h"
#include <utils/Log.h>
#include <binder/Parcel.h>
#include <time.h>

#ifdef DIRECT_INPUTMANAGER
#include <input/Input.h>
#include <binder/IServiceManager.h>
#else

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <cutils/sockets.h>
#include <cutils/log.h>
#include <utils/String16.h>
#include <utils/String8.h>

#endif

namespace android {

#define TAG "HeadTrackInputEventThread"

#define  MATRIX_VIEW_SIZE  (sizeof(float)*16)

void HeadTrackInputEventThread::beginTracker() {
    Mutex::Autolock lock(mMutex);
    mStart = true;
    mLayers.clear();
}
void HeadTrackInputEventThread::trackerSetting(int z,
    const Mesh::VertexArray<vec3> &positions, const Rect &rect, const Transform &transform)
{
    Mutex::Autolock lock(mMutex);
    ssize_t postion = mLayers.add();
    HeadTrackerPlan& plan = mLayers.editItemAt(postion);

    plan.mLayerZ = z;

    plan.mPosition3 = Vector3f(positions[0].x, positions[0].y, positions[0].z);
    plan.mPosition0 = Vector3f(positions[1].x, positions[1].y, positions[1].z);
    plan.mPosition1 = Vector3f(positions[2].x, positions[2].y, positions[2].z);
    plan.mPosition2 = Vector3f(positions[3].x, positions[3].y, positions[3].z);

    plan.mRect = rect;
    plan.mTransform = transform;
/*
    vec2 vec = transform.transform(rect.left,  rect.top);
    plan.mWinRect.left = vec.y;
    plan.mWinRect.top = vec.x;
    vec = transform.transform(rect.right,  rect.bottom);
    plan.mWinRect.right = vec.y;
    plan.mWinRect.bottom = vec.x;
*/
    {
        //vec2 _vec0 = transform.transform(rect.left,  rect.top);
        vec2 _vec1 = transform.transform(rect.left,  rect.bottom);
        //vec2 _vec2 = transform.transform(rect.right, rect.bottom);
        vec2 _vec3 = transform.transform(rect.right, rect.top);
        /*
        ALOGD("tracker(%d %d %d %d) (%f)(%f) (%f)(%f) (%f)(%f) (%f)(%f)",
            rect.left,  rect.top,
            rect.right, rect.bottom,
            _vec0.x, _vec0.y,
            _vec1.x, _vec1.y,
            _vec2.x, _vec2.y,
            _vec3.x, _vec3.y);
        */
        plan.mWinRect.left = _vec3.y;
        plan.mWinRect.top = _vec3.x;

        plan.mWinRect.right = _vec1.y;
        plan.mWinRect.bottom = _vec1.x;
    }

}

static Matrix4f transformMatrix4f(0, 1, 0, 0,
                         -1, 0, 0, 0,
                          0, 0, 1, 0,
                          0, 0, 0, 1);

static Matrix3f transformMatrix3f(0, 1, 0,
                         -1, 0, 0,
                          0, 0, 1);

void HeadTrackInputEventThread::trackerOrign(int z,
    const Mesh::VertexArray<vec3> &positions, const Rect &rect, const Transform &transform)
{
    Mutex::Autolock lock(mMutex);
    ssize_t postion = mLayers.add();
    HeadTrackerPlan& plan = mLayers.editItemAt(postion);

    plan.mLayerZ = z;

    plan.mPosition3 = transformMatrix4f.Transform(Vector3f(positions[0].x, positions[0].y, positions[0].z));
    plan.mPosition0 = transformMatrix4f.Transform(Vector3f(positions[1].x, positions[1].y, positions[1].z));
    plan.mPosition1 = transformMatrix4f.Transform(Vector3f(positions[2].x, positions[2].y, positions[2].z));
    plan.mPosition2 = transformMatrix4f.Transform(Vector3f(positions[3].x, positions[3].y, positions[3].z));

    plan.mRect = rect;
    plan.mTransform = transform;
/*
    vec2 vec = transform.transform(rect.left,  rect.top);
    plan.mWinRect.left = vec.y;
    plan.mWinRect.top = vec.x;
    vec = transform.transform(rect.right,  rect.bottom);
    plan.mWinRect.right = vec.y;
    plan.mWinRect.bottom = vec.x;
*/
    {
        //vec2 _vec0 = transform.transform(rect.left,  rect.top);
        vec2 _vec1 = transform.transform(rect.left,  rect.bottom);
        //vec2 _vec2 = transform.transform(rect.right, rect.bottom);
        vec2 _vec3 = transform.transform(rect.right, rect.top);
        /*
        ALOGD("tracker(%d %d %d %d) (%f)(%f) (%f)(%f) (%f)(%f) (%f)(%f)",
            rect.left,  rect.top,
            rect.right, rect.bottom,
            _vec0.x, _vec0.y,
            _vec1.x, _vec1.y,
            _vec2.x, _vec2.y,
            _vec3.x, _vec3.y);
        */
        plan.mWinRect.left = _vec3.y;
        plan.mWinRect.top = _vec3.x;

        plan.mWinRect.right = _vec1.y;
        plan.mWinRect.bottom = _vec1.x;
    }

}

void HeadTrackInputEventThread::endTracker(const float pfViewMatrix[]) {
    Mutex::Autolock lock(mMutex);
    memcpy(&(mDirectionData.M[0][0]), pfViewMatrix, MATRIX_VIEW_SIZE);
    mStart = false;
    mCond.signal();
}

bool HeadTrackInputEventThread::threadLoop() {
	{
    	Mutex::Autolock lock(mMutex);
        status_t err = mCond.wait(mMutex);
        if (err != NO_ERROR) {
            ALOGW("error waiting for new events: %s (%d)",
                strerror(-err), err);
            return false;
        }
        if (mStart) {
            ALOGW("HA HA had start new Loop;");
            return true;
        }

        memcpy(&(mDirection.M[0][0]), &(mDirectionData.M[0][0]), MATRIX_VIEW_SIZE);
        mTrackerLayers.clear();
        mTrackerLayers.appendVector(mLayers);
    }
	computeHeadTrackerPlan();
    return true;
}

void HeadTrackInputEventThread::onFirstRef() {
#ifndef DIRECT_INPUTMANAGER
    mVirtualInput = -1;
    setUpInput();
#endif
    mStart = false;
    run("HeadTrackInputEventThread", PRIORITY_URGENT_DISPLAY + PRIORITY_MORE_FAVORABLE);
}

void HeadTrackInputEventThread::onLastStrongRef(const void* /*id*/) {
#ifdef DIRECT_INPUTMANAGER
    releaseInputManager();
#else
    shutDownInput();
#endif
}

void HeadTrackInputEventThread::computeHeadTrackerPlan() {
    Vector3f ray = mDirection.Transform(Vector3f(0, 0, -1));
    HeadTrackerPlan *result = NULL;

    //ALOGI("computeHeadTrackerPlan");

    for (HeadTrackerPlan *plan = mTrackerLayers.begin(); plan != mTrackerLayers.end(); plan ++) {
        if (! plan->intersection(ray)) {
            //plan->dump("no intersection");
            continue;
        }

        //plan->dump("plan");
        // TODO
        if (NULL == result) {
            result = plan;
            //result->dump("result");
            continue;
        }
        int planLength = int(plan->mInterPosition0.Length());
        int resultLength = int(result->mInterPosition0.Length());
        if (planLength == resultLength) {
            if (plan->mLayerZ > result->mLayerZ) {
                result = plan;
                //result->dump("result");
            }
        } else if (planLength < resultLength) {
            result = plan;
            //result->dump("result");
        }
    }
    if (NULL != result) {
        tracker(result);
    }
}



bool HeadTrackerPlan::intersection(const Vector3f &ray) {
    /*
    p3-------p2
     |       |
     |       |
    p0-------p1
    */

    Vector3f &leftTop = mPosition3;
    Vector3f &topRight = mPosition2;
    Vector3f &bottomLeft = mPosition0;
    Vector3f &bottomRight = mPosition1;

    Vector3f xAxis = (topRight - leftTop);//.Normalized();
    Vector3f yAxis = (bottomLeft - leftTop);//.Normalized();

    Vector3f normalize = (xAxis.Cross(yAxis)).Normalized();
    if (0 == ray.Dot(normalize)) {
        return false;
    }
    float scale = (leftTop.Dot(normalize) / ray.Dot(normalize));
    if (scale < 0) {
        return false;
    }
    mInterPosition0 =  scale * ray;

    Vector3f rAxis = (mInterPosition0 - leftTop);
    float scalex = rAxis.Dot(xAxis)/xAxis.LengthSq();
    float scaley = rAxis.Dot(yAxis)/yAxis.LengthSq();
    if (scalex > 0 && scaley > 0 && scalex < 1 && scaley < 1) {
        mScalex = scalex;
        mScaley = scaley;

        //mDistance = leftTop.Dot(normalize);
        return true;
    }
    return false;
}

void HeadTrackerPlan::dump(const char *tag) {
    ALOGD("(%s)mLayerZ(%d) 0(%f %f %f) 1(%f %f %f) 2(%f %f %f) 3(%f %f %f) i(%f %f %f) sx(%f) sy(%f)",
        tag,
        mLayerZ,
        mPosition0.x, mPosition0.y, mPosition0.z,
        mPosition1.x, mPosition1.y, mPosition1.z,
        mPosition2.x, mPosition2.y, mPosition2.z,
        mPosition3.x, mPosition3.y, mPosition3.z,
        mInterPosition0.x, mInterPosition0.y, mInterPosition0.z,
        mScalex, mScaley);
}

#ifndef DIRECT_INPUTMANAGER
#define HOVER_ENTER 0
#define HOVER_MOVE  1
#define HOVER_EXIT  2
#define HOVER_MOVE_TEST1 3
#define HOVER_MOVE_TEST2 4

#define CMD_STR_LEN 1024
#define TIME_STAMP_LEN 18
#endif

void HeadTrackInputEventThread::tracker(const HeadTrackerPlan *result) {

    int asixX = result->mWinRect.left + result->mScalex * (result->mWinRect.right - result->mWinRect.left);
    int asixY = result->mWinRect.top + result->mScaley * (result->mWinRect.bottom - result->mWinRect.top);
#ifdef DIRECT_INPUTMANAGER
    injectEvent(result->mLayerZ, asixX, asixY);
#else
    char cmd_param[CMD_STR_LEN] = { '\0' };
    char time_buf[TIME_STAMP_LEN] = { '\0' };
    time_t aclock;
    time(&aclock);

    strftime(time_buf,TIME_STAMP_LEN,"%Y%m%d|%H:%M:%S", localtime(&aclock));
    int len = sprintf(cmd_param, "%d,%d,%d,%d,%s\n", result->mLayerZ, asixX, asixY, HOVER_MOVE, time_buf);
    sendInput(cmd_param, len);
#endif
}

#ifdef DIRECT_INPUTMANAGER
void HeadTrackInputEventThread::injectEvent(int layer, int asixX, int asixY) {

	//ALOGW("tracker %d %d can send to input manager,Layer=%d", asixX, asixY,layer);
    setupInputManager();
    if (mInputManager == NULL) {
        ALOGW("tracker %d %d can't send to input manager", asixX, asixY);
        return;
    }


    nsecs_t now = systemTime(SYSTEM_TIME_MONOTONIC);

    PointerProperties pointerProperties[1];
    pointerProperties[0].clear();
    pointerProperties[0].id = 0;
    pointerProperties[0].toolType = 0;

    PointerCoords pointerCoords[1];
	pointerCoords[0].clear();
    pointerCoords[0].setAxisValue(AMOTION_EVENT_AXIS_X, asixX);
    pointerCoords[0].setAxisValue(AMOTION_EVENT_AXIS_Y, asixY);

    MotionEvent motionEvent;
    motionEvent.initialize(
            -1, //deviceId,
            0x00002000 | 0x00000002, //source, 0x00002000 | SOURCE_CLASS_POINTER
            7, //action, ACTION_HOVER_MOVE
			0,//action
            0, //flags,
            0, // edgeFlags,
            layer, // metaState,
            0, // buttonState,
            0, // xOffset,
            0, // yOffset,
            1.0, // xPrecision,
            1.0, // yPrecision,
            0, //downTime,
            now, // eventTime,
            1, // pointerCount,
            pointerProperties, // pointerProperties,
            pointerCoords); // pointerCoords

    /* Waits for the input event to be completely processed. */
    // INPUT_EVENT_INJECTION_SYNC_WAIT_FOR_FINISHED = 2,
    status_t result = mInputManager->injectInputEvent(&motionEvent, 2);
}

void HeadTrackInputEventThread::setupInputManager() {
    if (mInputManager == NULL || !mInputManager->isBinderAlive()) {
        const String16 serviceName("input");

        sp<IBinder> binder = defaultServiceManager()->getService(serviceName);
        if (binder == NULL) {
            ALOGW("can't getService  input");
            return;
        }
        mInputManager = interface_cast<IInputManager>(binder);
    }
}

void HeadTrackInputEventThread::releaseInputManager() {
    mInputManager = NULL;
}

#else // DIRECT_INPUTMANAGER ###############################

#ifndef TEMP_FAILURE_RETRY
#define TEMP_FAILURE_RETRY(exp) ({         \
    typeof (exp) _rc;                      \
    do {                                   \
        _rc = (exp);                       \
    } while (_rc == -1 && errno == EINTR); \
    _rc; })
#endif

#define  SOCKET_NAME    "socket_virtual_input"

const char* serverResponseStart = "ServerResponse:Start";
const char* serverResponseEnd = "ServerResponse:End";

void HeadTrackInputEventThread::setUpInput() {
    int num_bytes = 0;
    int fd = socket_local_client(SOCKET_NAME, ANDROID_SOCKET_NAMESPACE_ABSTRACT, SOCK_STREAM);
    if (fd < 0) //check open failed
    {
        ALOGE("Failed to open socket: %s", SOCKET_NAME);
        return;
    }
    //send client request start
    String8 str("ClientRequest:Start\n");
    num_bytes = TEMP_FAILURE_RETRY(send(fd, str, str.size(), 0));
    if (num_bytes > 0) {
        ALOGI("Client send start cmd success");
    }
    //recv server response start
    char buf[64] = { '\0' };
    num_bytes = TEMP_FAILURE_RETRY(recv(fd, buf, 64, 0));
    if (num_bytes > 0) {
        //ALOGI("Client recv Server start cmd success");
    }
    if (strncmp(buf, serverResponseStart, strlen(serverResponseStart)) == 0) {
        //ALOGI("accept start response from Server,Success");
        mVirtualInput = fd;
        return;
    }
    ALOGW("don\'t accept data from Server,Failure");
}

void HeadTrackInputEventThread::shutDownInput() {
    if (-1 == mVirtualInput)
        return;

    int num_bytes = 0;
    //send client request end
    String8 str("ClientRequest:End\n");
    num_bytes = TEMP_FAILURE_RETRY(send(mVirtualInput, str, str.size(), 0));
    if (num_bytes > 0) {
        //ALOGI("Client send end cmd success");
    }
    //recv server response end
    char buf[64] = { '\0' };
    num_bytes = TEMP_FAILURE_RETRY(recv(mVirtualInput, buf,64, 0));
    if (num_bytes > 0) {
        //ALOGI("Client recv Server end cmd success");
    }

    if(strncmp(buf, serverResponseEnd, strlen(serverResponseEnd)) == 0) {
        ALOGI("accept end response from Server,Success");
    }else{
        ALOGE("don\'t accept end response from Server,Failure");
    }

    close(mVirtualInput);
    mVirtualInput = -1;
}

//cmd_param:x,y,mode,timetracker
void HeadTrackInputEventThread::sendInput(const char *cmd_param, int len) {
    int num_bytes = 0;
    if(-1 == mVirtualInput){
        setUpInput();
        return;
    }
    //ALOGI("cmd_param:%s", cmd_param);

    num_bytes = TEMP_FAILURE_RETRY(send(mVirtualInput, cmd_param, len, 0));
    //ALOGI("send (%s)(%d)", cmd_param, num_bytes);
    if (num_bytes < 0) {
        //shutDownInput();
        ALOGW("num_bytes<0,len client send event error");
    }
}

#endif // DIRECT_INPUTMANAGER

} // namespace android
