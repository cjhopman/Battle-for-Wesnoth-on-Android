LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := pixman

LOCAL_CFLAGS := \
	-DHAVE_CONFIG_H \
	-DPIXMAN_NO_TLS \
	-D_USE_MATH_DEFINES \
	-I$(LOCAL_PATH)/pixman \
	$(NULL)

LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/pixman

LOCAL_CPP_EXTENSION := .cpp

SRC_FILES :=  \
	pixman-access.c				\
	pixman-access-accessors.c		\
	pixman-cpu.c				\
	pixman-gradient-walker.c		\
	pixman-region16.c			\
	pixman-region32.c			\
	pixman-image.c				\
	pixman-implementation.c			\
	pixman-combine32.c			\
	pixman-combine64.c			\
	pixman-general.c			\
	pixman.c				\
	pixman-fast-path.c			\
	pixman-solid-fill.c			\
	pixman-conical-gradient.c		\
	pixman-linear-gradient.c		\
	pixman-radial-gradient.c		\
	pixman-bits-image.c			\
	pixman-utils.c				\
	pixman-edge.c				\
	pixman-edge-accessors.c			\
	pixman-trap.c				\
	pixman-timer.c				\
	pixman-matrix.c \
	$(NULL)

LOCAL_SRC_FILES := $(addprefix pixman/, $(SRC_FILES))

LOCAL_LIBRARIES := \
	$(NULL)


LOCAL_SHARED_LIBRARIES := $(filter-out $(APP_AVAILABLE_STATIC_LIBS), $(LOCAL_LIBRARIES))
LOCAL_STATIC_LIBRARIES := $(filter $(APP_AVAILABLE_STATIC_LIBS), $(LOCAL_LIBRARIES))

LOCAL_LDLIBS :=

ifeq (,$(findstring $(LOCAL_MODULE), $(APP_AVAILABLE_STATIC_LIBS)))
include $(BUILD_SHARED_LIBRARY)
else
include $(BUILD_STATIC_LIBRARY)
endif

