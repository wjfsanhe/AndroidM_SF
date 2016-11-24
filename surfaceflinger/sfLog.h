#ifndef  SF_LOG_H
#define SF_LOG_H
#include <cutils/properties.h>
#include <stdint.h>
#include <stdio.h>

namespace android {

static bool isDebugEnable(){
                char value[PROPERTY_VALUE_MAX];
                property_get("sf.debug", value, "0");
                //ALOGD("sf.service: %s",value);
                return (atoi(value) > 0)?true:false;
 }
#define SF_LOG(TAG ,format, x...)                                \
{                                                              \
  if(isDebugEnable())   \
  ALOGD("<%s> " format "\n",TAG,##x);    \
}

}
#endif
