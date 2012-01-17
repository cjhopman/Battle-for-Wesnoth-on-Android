LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
APP_STL = gnustl_static

LOCAL_MODULE := gif

LOCAL_CFLAGS := 

LOCAL_CPP_EXTENSION := .cpp

LOCAL_SRC_FILES := $(wildcard $(LOCAL_PATH)/*.c)

LOCAL_STATIC_LIBRARIES :=

LOCAL_SHARED_LIBRARIES := 

LOCAL_LDLIBS := 

ifeq (,$(findstring $(LOCAL_MODULE), $(APP_AVAILABLE_STATIC_LIBS)))
include $(BUILD_SHARED_LIBRARY)
else
include $(BUILD_STATIC_LIBRARY)
endif

