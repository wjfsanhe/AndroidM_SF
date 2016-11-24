#ifndef _MOJING_API_H
#define _MOJING_API_H
bool MojingSDK_StartTracker(const char* pPackageName);
void MojingSDK_StopTracker(void);
void MojingSDK_ResetTracker(void);
void MojingSDK_ResetSensorOrientation(void);
void MojingSDK_ResetSensorOrientation2(void);
uint64_t  MojingSDK_getLastHeadView(float* pfViewMatrix);
int32_t MojingSDK_getPredictionHeadView(float* pfViewMatrix, double time);
void MojingSDK_getLastHeadEulerAngles(float* pfEulerAngles);
void MojingSDK_getLastHeadQuarternion(float *w, float *x, float *y, float *z);
int MojingSDK_GetSocketPort();
int MojingSDK_SetSocketPort(int port);
#endif
