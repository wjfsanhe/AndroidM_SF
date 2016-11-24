LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
        IServiceResponse.cpp \
        IVrSDKService.cpp \
        TrackerThread.cpp \
	VrService.cpp

LOCAL_SHARED_LIBRARIES := \
        libutils \
        libcutils \
        libbinder \
        libmdktracker

LOCAL_MODULE:= libSDKService
LOCAL_MODULE_TAGS := SDKService

include $(BUILD_SHARED_LIBRARY)
