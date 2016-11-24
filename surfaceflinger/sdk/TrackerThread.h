#ifndef  TRACKER_THREAD_H
#define TRACKER_THREAD_H
#include <stdint.h>
#include <sys/types.h>

#include <utils/String8.h>
#include <utils/Errors.h>
#include <utils/threads.h>
#include <utils/SortedVector.h>
#include "MojingAPI.h"

// ---------------------------------------------------------------------------
namespace android {
// ---------------------------------------------------------------------------

class SurfaceFlinger;

class TrackerThread : public Thread{
    public:
        //TrackerThread();
        TrackerThread(sp<SurfaceFlinger> sur);
        uint64_t  getLastHeadView(float* pfViewMatrix);
        uint64_t  getLastHeadQuarternion(float *w, float *x, float *y, float *z);
        uint32_t    getPredictionHeadView(float* pfViewMatrix, double time);
        void            setLuciferMode(bool mMode);
	int32_t  getServerPort(void);
	int32_t  setServerPort(int32_t port);
	int32_t  startThread(void);
    private:
        virtual bool        threadLoop();
        virtual void        onFirstRef();
    private:
        float mDefViewMatrix[16];
        float mCurViewMatrix[16];
        float mCurPredictViewMatrix[16];
        bool  mServiceConnectted;
        bool  mLuciferMode;
        sp<SurfaceFlinger> mSur;

};


// ---------------------------------------------------------------------------

}; // namespace android

// ---------------------------------------------------------------------------
#endif

