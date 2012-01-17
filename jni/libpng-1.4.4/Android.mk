LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := png

LOCAL_CFLAGS := \
	-DHAVE_CONFIG_H \
	-DPNG_CONFIGURE_LIBPNG \
	$(NULL)

LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include

LOCAL_CPP_EXTENSION := .cpp

SRC_FILES :=  \
	png.c \
	pngset.c \
	pngget.c \
	pngrutil.c \
	pngtrans.c \
	pngwutil.c \
	pngread.c \
	pngrio.c \
	pngwio.c \
	pngwrite.c \
	pngrtran.c \
	pngwtran.c \
	pngmem.c \
	pngerror.c \
	pngpread.c \
	$(NULL)

LOCAL_SRC_FILES := $(addprefix , $(SRC_FILES))

LOCAL_LIBRARIES := \
	$(NULL)


LOCAL_SHARED_LIBRARIES := $(filter-out $(APP_AVAILABLE_STATIC_LIBS), $(LOCAL_LIBRARIES))
LOCAL_STATIC_LIBRARIES := $(filter $(APP_AVAILABLE_STATIC_LIBS), $(LOCAL_LIBRARIES))

LOCAL_LDLIBS :=  \
	-lz \
	$(NULL)

ifeq (,$(findstring $(LOCAL_MODULE), $(APP_AVAILABLE_STATIC_LIBS)))
include $(BUILD_SHARED_LIBRARY)
else
include $(BUILD_STATIC_LIBRARY)
endif

