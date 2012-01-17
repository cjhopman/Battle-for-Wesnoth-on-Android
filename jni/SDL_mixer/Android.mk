LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := SDL_mixer

LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include

LOCAL_CFLAGS := \
	-I$(LOCAL_PATH) \
	-I$(LOCAL_PATH)/include \
	-I$(LOCAL_PATH)/timidity \
	-DWAV_MUSIC \
	-DOGG_USE_TREMOR \
	-DOGG_MUSIC \
	-DFLAC_MUSIC \
	-DMOD_MUSIC \
	-DMID_MUSIC \
	-DUSE_TIMIDITY_MIDI \
	-DOGG_USE_TREMOR \
	$(NULL)

LOCAL_CPP_EXTENSION := .cpp

LOCAL_SRC_FILES := $(notdir $(wildcard $(LOCAL_PATH)/*.c)) $(addprefix timidity/, $(notdir $(wildcard $(LOCAL_PATH)/timidity/*.c)))

LOCAL_LIBRARIES := \
	SDL-$(SDL_VERSION) \
	flac \
	mikmod \
	tremor \
	ogg \
	vorbis \
	$(NULL)

LOCAL_LDLIBS := -llog

LOCAL_SHARED_LIBRARIES := $(filter-out $(APP_AVAILABLE_STATIC_LIBS), $(LOCAL_LIBRARIES))
LOCAL_STATIC_LIBRARIES := $(filter $(APP_AVAILABLE_STATIC_LIBS), $(LOCAL_LIBRARIES))

ifeq (,$(findstring $(LOCAL_MODULE), $(APP_AVAILABLE_STATIC_LIBS)))
include $(BUILD_SHARED_LIBRARY)
else
include $(BUILD_STATIC_LIBRARY)
endif

