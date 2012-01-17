LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := freetype

LOCAL_CFLAGS := \
	-DFT_CONFIG_OPTION_SYSTEM_ZLIB \
	-DFT_CONFIG_CONFIG_H="<ftconfig.h>" \
	-DHAVE_CONFIG_H \
	-DFT_CONFIG_OPTION_SYSTEM_ZLIB \
	-DFT_CONFIG_CONFIG_H="<ftconfig.h>" \
	-DFT2_BUILD_LIBRARY \
	-DFT_CONFIG_MODULES_H="<ftmodule.h>" \
	-I$(LOCAL_PATH)/include \
	-I$(LOCAL_PATH)/include/freetype/config \
	$(NULL)

LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include

LOCAL_CPP_EXTENSION := .cpp

SRC_FILES :=  \
	base/ftsystem.c \
	base/ftdebug.c \
	base/ftinit.c \
	base/ftbase.c \
	base/ftbbox.c \
	base/ftbdf.c \
	base/ftbitmap.c \
	base/ftcid.c \
	base/ftfstype.c \
	base/ftgasp.c \
	base/ftglyph.c \
	base/ftgxval.c \
	base/ftlcdfil.c \
	base/ftmm.c \
	base/ftotval.c \
	base/ftpatent.c \
	base/ftpfr.c \
	base/ftstroke.c \
	base/ftsynth.c \
	base/fttype1.c \
	base/ftwinfnt.c \
	base/ftxf86.c \
	truetype/truetype.c \
	type1/type1.c \
	cff/cff.c \
	cid/type1cid.c \
	pfr/pfr.c \
	type42/type42.c \
	winfonts/winfnt.c \
	pcf/pcf.c \
	bdf/bdf.c \
	sfnt/sfnt.c \
	autofit/autofit.c \
	pshinter/pshinter.c \
	raster/raster.c \
	smooth/smooth.c \
	cache/ftcache.c \
	gzip/ftgzip.c \
	lzw/ftlzw.c \
	psaux/psaux.c \
	psnames/psnames.c \
	$(NULL)

LOCAL_SRC_FILES := $(addprefix src/, $(SRC_FILES))

LOCAL_LIBRARIES := \
	$(NULL)


LOCAL_SHARED_LIBRARIES := $(filter-out $(APP_AVAILABLE_STATIC_LIBS), $(LOCAL_LIBRARIES))
LOCAL_STATIC_LIBRARIES := $(filter $(APP_AVAILABLE_STATIC_LIBS), $(LOCAL_LIBRARIES))

LOCAL_LDLIBS := -lz

ifeq (,$(findstring $(LOCAL_MODULE), $(APP_AVAILABLE_STATIC_LIBS)))
include $(BUILD_SHARED_LIBRARY)
else
include $(BUILD_STATIC_LIBRARY)
endif

