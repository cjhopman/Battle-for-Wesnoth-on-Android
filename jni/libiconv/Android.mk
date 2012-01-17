LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := iconv

DEFS := \
	-DHAVE_CONFIG_H \
	-DEXEEXT=\"\" \
	-DINSTALLDIR=\"\" \
	-DLOCALEDIR=\"\" \
	-DLIBDIR=\"\" \
	-DBUILDING_LIBICONV \
	-DBUILDING_DLL \
	-DENABLE_RELOCATABLE=1 \
	-DIN_LIBRARY \
	-DINSTALLDIR=\"\" \
	-DNO_XMALLOC \
	-Dset_relocation_prefix=libiconv_set_relocation_prefix \
	-Drelocate=libiconv_relocate \
	$(NULL)

LOCAL_CFLAGS :=  \
	-I$(LOCAL_PATH)/include \
	-I$(LOCAL_PATH)/lib \
	-I$(LOCAL_PATH)/srclib \
	-I$(LOCAL_PATH)/lib \
	-I$(LOCAL_PATH)/src \
	$(DEFS) \
	$(NULL)

LOCAL_EXPORT_C_INCLUDES := \
	$(LOCAL_PATH)/include \
	$(NULL)

LOCAL_SRCS := \
	lib/iconv.c \
	lib/relocatable.c \
	libcharset/lib/localcharset.c \
	$(NULL)

LOCAL_CPP_EXTENSION := .cpp

# Note this "simple" makefile var substitution, you can find even more complex examples in different Android projects
LOCAL_SRC_FILES := $(foreach F, $(LOCAL_SRCS), $(addprefix $(dir $(F)),$(notdir $(wildcard $(LOCAL_PATH)/$(F)))))

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

