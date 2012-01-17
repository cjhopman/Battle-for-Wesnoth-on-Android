LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := flac

LOCAL_CFLAGS := \
	-DHAVE_CONFIG_H \
	-I$(LOCAL_PATH)/include \
	-I$(LOCAL_PATH)/src/libFLAC \
	-I$(LOCAL_PATH)/src/libFLAC/include \
	$(NULL)

LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include

LOCAL_CPP_EXTENSION := .cpp

SRC_FILES :=  \
	ogg_decoder_aspect.c \
	ogg_encoder_aspect.c \
	ogg_helper.c \
	ogg_mapping.c \
	bitmath.c \
	bitreader.c \
	bitwriter.c \
	cpu.c \
	crc.c \
	fixed.c \
	float.c \
	format.c \
	lpc.c \
	md5.c \
	memory.c \
	metadata_iterators.c \
	metadata_object.c \
	stream_decoder.c \
	stream_encoder.c \
	stream_encoder_framing.c \
	window.c \
	$(NULL)

LOCAL_SRC_FILES := $(addprefix src/libFLAC/, $(SRC_FILES))

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

