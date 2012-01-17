LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := fontconfig

LOCAL_CFLAGS := \
	-DHAVE_CONFIG_H \
	-DFC_CACHEDIR=\"fonts\" \
	-DFONTCONFIG_PATH=\"fonts\" \
	$(NULL)

LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/fontconfig

LOCAL_CPP_EXTENSION := .cpp

SRC_FILES :=  \
	fcatomic.c \
	fcblanks.c \
	fccache.c \
	fccfg.c \
	fccharset.c \
	fcdbg.c \
	fcdefault.c \
	fcdir.c \
	fcformat.c \
	fcfreetype.c \
	fcfs.c \
	fcinit.c \
	fclang.c \
	fclist.c \
	fcmatch.c \
	fcmatrix.c \
	fcname.c \
	fcpat.c \
	fcserialize.c \
	fcstr.c \
	fcxml.c \
	ftglue.c \
	$(NULL)

LOCAL_SRC_FILES := $(addprefix src/, $(SRC_FILES))

LOCAL_LIBRARIES := \
	freetype \
	iconv \
	xml2 \
	$(NULL)


LOCAL_SHARED_LIBRARIES := $(filter-out $(APP_AVAILABLE_STATIC_LIBS), $(LOCAL_LIBRARIES))
LOCAL_STATIC_LIBRARIES := $(filter $(APP_AVAILABLE_STATIC_LIBS), $(LOCAL_LIBRARIES))

LOCAL_LDLIBS := -lz -llog

ifeq (,$(findstring $(LOCAL_MODULE), $(APP_AVAILABLE_STATIC_LIBS)))
include $(BUILD_SHARED_LIBRARY)
else
include $(BUILD_STATIC_LIBRARY)
endif

