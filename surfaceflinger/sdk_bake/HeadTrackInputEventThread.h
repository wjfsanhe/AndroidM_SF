#ifndef __HEADTRACKINPUTEVENT_THREAD_H__
#define __HEADTRACKINPUTEVENT_THREAD_H__

#include <utils/threads.h>
#include <utils/Vector.h>
#include <ui/vec3.h>
#include "RenderEngine/sdk/Base/MojingMath.h"
#include "RenderEngine/Mesh.h"
#include "Transform.h"

#define DIRECT_INPUTMANAGER

#ifdef DIRECT_INPUTMANAGER
#include "IInputManager.h"
#endif

namespace android {

using namespace Baofeng::Mojing;

struct HeadTrackerPlan {

    int mLayerZ;

    Vector3f mPosition0, mPosition1, mPosition2, mPosition3;
    Rect mRect;
    Rect mWinRect;
    Transform mTransform;

    Vector3f mInterPosition0;

    float mScalex;
    float mScaley;
    //float mDistance;

    HeadTrackerPlan()
        : mLayerZ(-1)
        , mPosition0()
        , mPosition1()
        , mPosition2()
        , mPosition3()
        , mInterPosition0()
        , mScalex(0)
        , mScaley(0) {
        //, mDistance(-1) {

        }

    bool intersection(const Vector3f &ray);
    void dump(const char *tag);
};

class HeadTrackInputEventThread : public Thread {

public:

    void beginTracker();
    void trackerSetting(int z, const Mesh::VertexArray<vec3> &position, const Rect &rect, const Transform &transform);
    void trackerOrign(int z, const Mesh::VertexArray<vec3> &position, const Rect &rect, const Transform &transform);
    void endTracker(const float pfViewMatrix[]);

private:
    virtual bool threadLoop();
    virtual void onFirstRef();
    virtual void onLastStrongRef(const void* /*id*/);

    void computeHeadTrackerPlan();

    Matrix4f mDirectionData;
    Vector<HeadTrackerPlan> mLayers;

    Mutex mMutex;
    Condition mCond;
    bool     mStart;

    Matrix4f mDirection;
    Vector<HeadTrackerPlan> mTrackerLayers;

    void tracker(const HeadTrackerPlan *result);

#ifdef DIRECT_INPUTMANAGER
    void setupInputManager();
    void releaseInputManager();
    void injectEvent(int layer, int asixX, int asixY);
    sp<IInputManager> mInputManager;
#else
    void setUpInput();
    void shutDownInput();
    void sendInput(const char *cmd_param, int len);

    int   mVirtualInput;
#endif

};

} // namespace android

#endif
