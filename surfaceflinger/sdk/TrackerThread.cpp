#include <stdint.h>
#include <sys/types.h>

#include <cutils/compiler.h>

#include <utils/Errors.h>
#include <utils/String8.h>
#include <utils/Trace.h>
#include <cutils/properties.h>

#include "TrackerThread.h"
#include "../SurfaceFlinger.h"
#include "../sfLog.h"


#define  MATRIX_VIEW_SIZE  (sizeof(float)*16)
#define  SDCARD_FILE    "/sdcard/MojingSDK/a"


// ---------------------------------------------------------------------------
namespace android {
// ---------------------------------------------------------------------------
#define TAG  "TrackerThread"
        static uint32_t getTrackerInterval(){
                char value[PROPERTY_VALUE_MAX];
                property_get("sf.tracker", value, "16700");
                //ALOGD("sf.service: %s",value);
                return  atoi(value);
        }
        static char* getSDCardFile(){
                static char fileName[PROPERTY_VALUE_MAX];
                 property_get("sf.file", fileName, "/sdcard/MojingSDK/mjsensor/VrService");
                 return fileName;
        }
        static int32_t testCreateSDDir(){
                char mmapRefPath[256] = { 0 };
                strcpy(mmapRefPath, getSDCardFile());
                mkdir("/sdcard/MojingSDK", 0777);
                mkdir("/sdcard/MojingSDK/mjsensor", 0777);

                int mmap_fd;
                if ((mmap_fd = open((const char*)&mmapRefPath, O_CREAT | O_RDWR, 0777)) < 0)
                {
                        SF_LOG(TAG, "Open error(%d). path=%s\n", errno, (char *)&mmapRefPath);
                        return false;
                }else{
                        SF_LOG(TAG,"SF create sdcard file success");
                }
                return 0;

        }
        static void dumpMatrixData(float* pfViewMatrix){
                String8  list;
                if(pfViewMatrix) {
                        list.clear();
                        list.append("GOT\n");
                        for(int j=0;j<4;j++) {
                                list.appendFormat("<%s-%d>",TAG,j);
                                for(int i=0;i<4;i++){
                                        list.appendFormat("%f ",pfViewMatrix[(j<<2) +i]);
                                }
                                list.append("\n");
                        }
                        SF_LOG(TAG,"%s",list.string());
                }
        }

        TrackerThread::TrackerThread(sp<SurfaceFlinger> sur)
                :mServiceConnectted(false),
                 mLuciferMode(false)
        {
                mSur = sur;
                memset(mDefViewMatrix,0,sizeof(mDefViewMatrix));
                mDefViewMatrix[0]=1.0;
                mDefViewMatrix[5]=1.0;
                mDefViewMatrix[10]=1.0;
                mDefViewMatrix[15]=1.0;
                memcpy(mCurViewMatrix,mDefViewMatrix,MATRIX_VIEW_SIZE) ;
                memcpy(mCurPredictViewMatrix,mDefViewMatrix,MATRIX_VIEW_SIZE) ;
        }


        uint64_t  TrackerThread::getLastHeadView(float* pfViewMatrix){
                //if tracker is not start  ,mCurViewMatrix will not change.

                if(!mSur->isBootFinished()){
                        if(pfViewMatrix){
                                memcpy(pfViewMatrix,mDefViewMatrix,MATRIX_VIEW_SIZE) ;
                        }
                        return 1;
                }
                uint64_t  ret = MojingSDK_getLastHeadView(mCurViewMatrix);
                if(pfViewMatrix) {
                        memcpy(pfViewMatrix,mCurViewMatrix,MATRIX_VIEW_SIZE) ;
                        dumpMatrixData(pfViewMatrix);
                }else {
                        SF_LOG(TAG,"buffer MatrixView could not be null");
                }

                if(ret==0) SF_LOG(TAG,"Tracker or service not started");
                return  ret ;
        }

        uint64_t  TrackerThread::getLastHeadQuarternion(float *w, float *x, float *y, float *z){
                //if tracker is not start  ,mCurViewMatrix will not change.
                uint64_t ret =-1 ;

                if(!mSur->isBootFinished()){
                        *w=1;
                        *x=0;
                        *y=0;
                        *z=0;
                        return ret;
                }

                if( w!=NULL && x!=NULL && y!=NULL && z!=NULL){
                        MojingSDK_getLastHeadQuarternion(w,x,y,z);
                        SF_LOG(TAG,"get last Quarternion:(w,x,y,z):(%f,%f,%f,%f)",*w,*x,*y,*z);
                        ret = 0;
                }
                return  ret ;
        }
        void  TrackerThread::setLuciferMode(bool mMode){
                mLuciferMode = mMode;
        }

        uint32_t  TrackerThread::getPredictionHeadView(float* pfViewMatrix,double time){
                //if tracker is not start  ,mCurViewMatrix will not change.
                uint32_t  ret = MojingSDK_getPredictionHeadView(mCurPredictViewMatrix,time);
                if(pfViewMatrix) {
                        memcpy(pfViewMatrix,mCurPredictViewMatrix,MATRIX_VIEW_SIZE) ;
                }else {
                        SF_LOG(TAG,"buffer MatrixView could not be null");
                }

                //if(ret==0) SF_LOG(TAG,"Tracker or service not started");
                return  ret ;
        }
        void TrackerThread::onFirstRef() {
                run("TrackerThread", PRIORITY_URGENT_DISPLAY + PRIORITY_MORE_FAVORABLE);
        }
        int32_t TrackerThread::startThread() {
                run("TrackerThread", PRIORITY_URGENT_DISPLAY + PRIORITY_MORE_FAVORABLE);
		return 0;
        }
	int32_t  TrackerThread::getServerPort(void){
               return  MojingSDK_GetSocketPort();
      	}
	int32_t  TrackerThread::setServerPort(int32_t port){
               return  MojingSDK_SetSocketPort(port);
      	}
        bool TrackerThread::threadLoop() {
                //surfaceFlinger hope  get message
                if(!mLuciferMode)
                {
                        usleep(10000);
                        return true;
                }
                String8  list;
                float  testViewMatrix[16];
                //if Mojing Tracker not started,Then quick check tracker.
                if(!mServiceConnectted)
                {
                        if(!MojingSDK_StartTracker("com.baofeng.mj"))
                        {
                                testCreateSDDir();
                                SF_LOG(TAG,"start tracker failed, waiting....");
                                usleep(5000);
                                return true ;
                        }
                }
                mServiceConnectted=true;
                //usleep(getTrackerInterval());

                if (getLastHeadView(testViewMatrix)) {
                        dumpMatrixData(testViewMatrix);
                }else{
                        mServiceConnectted=false;
                }
                mSur->setViewMatrix(testViewMatrix);
                mSur->repaintEverything();

                return true;
        }



// ---------------------------------------------------------------------------

}; // namespace android



