LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := vorbis

LOCAL_CFLAGS := \
	-DHAVE_CONFIG_H \
	-DUSE_MEMORY_H \
	-D_REENTRANT \
	-ffast-math \
	-fsigned-char \
	-I$(LOCAL_PATH)/lib \
	-I$(LOCAL_PATH)/include \
	$(NULL)

LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include

LOCAL_CPP_EXTENSION := .cpp

SRC_FILES :=  \
	mdct.c \
	smallft.c \
	block.c \
	envelope.c \
	window.c \
	lsp.c \
	lpc.c \
	analysis.c \
	synthesis.c \
	psy.c \
	info.c \
	floor1.c \
	floor0.c\
	res0.c \
	mapping0.c \
	registry.c \
	codebook.c \
	sharedbook.c\
	lookup.c \
	bitrate.c\
	$(NULL)

LOCAL_SRC_FILES := $(addprefix lib/, $(SRC_FILES))

LOCAL_LIBRARIES := \
	ogg \
	$(NULL)


LOCAL_SHARED_LIBRARIES := $(filter-out $(APP_AVAILABLE_STATIC_LIBS), $(LOCAL_LIBRARIES))
LOCAL_STATIC_LIBRARIES := $(filter $(APP_AVAILABLE_STATIC_LIBS), $(LOCAL_LIBRARIES))

LOCAL_LDLIBS := 

ifeq (,$(findstring $(LOCAL_MODULE), $(APP_AVAILABLE_STATIC_LIBS)))
include $(BUILD_SHARED_LIBRARY)
else
include $(BUILD_STATIC_LIBRARY)
endif

