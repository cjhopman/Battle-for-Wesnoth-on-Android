LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES :=			\
	gmodule.c    

LOCAL_MODULE := gmodule

LOCAL_C_INCLUDES :=			\
	$(LOCAL_PATH)/android-internal	\
	$(GLIB_TOP)/android-internal	\
	$(NULL)

LOCAL_CFLAGS :=				\
	-DHAVE_CONFIG_H			\
	-DG_LOG_DOMAIN=\"GModule\"	\
	-DG_DISABLE_DEPRECATED  \
	$(NULL)

LOCAL_LDLIBS :=				\
	-ldl \
	$(NULL)

LOCAL_LIBRARIES := \
	glib \
	$(NULL)

LOCAL_SHARED_LIBRARIES := $(filter-out $(APP_AVAILABLE_STATIC_LIBS), $(LOCAL_LIBRARIES))
LOCAL_STATIC_LIBRARIES := $(filter $(APP_AVAILABLE_STATIC_LIBS), $(LOCAL_LIBRARIES))

LOCAL_EXPORT_C_INCLUDES := \
	$(LOCAL_PATH) \
	$(NULL)

ifeq (,$(findstring $(LOCAL_MODULE), $(APP_AVAILABLE_STATIC_LIBS)))
include $(BUILD_SHARED_LIBRARY)
else
include $(BUILD_STATIC_LIBRARY)
endif

