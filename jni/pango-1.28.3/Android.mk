LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := opentype

LOCAL_CFLAGS := \
	-DHAVE_CONFIG_H \
	-DHAVE_GLIB \
	-DG_LOG_DOMAIN=\"Pango\" \
	-DPANGO_ENABLE_BACKEND \
	-DPANGO_ENABLE_ENGINE \
	-DSYSCONFDIR=\".\" \
	-DLIBDIR=\".\" \
	-DG_DISABLE_CAST_CHECKS \
	-I$(LOCAL_PATH)/pango/opentype


LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/pango/opentype

LOCAL_CPP_EXTENSION := .cc

SRC_FILES :=  \
	hb-blob.c \
	hb-buffer.c \
	hb-font.cc \
	hb-glib.c \
	hb-shape.c \
	hb-unicode.c \
	hb-ot-layout.cc \


LOCAL_SRC_FILES := $(addprefix pango/opentype/, $(SRC_FILES))

LOCAL_LIBRARIES := \
	gobject \
	gmodule \
	gthread \
	glib \
	pcre \
	iconv \
	intl \


LOCAL_SHARED_LIBRARIES := $(filter-out $(APP_AVAILABLE_STATIC_LIBS), $(LOCAL_LIBRARIES))
LOCAL_STATIC_LIBRARIES := $(filter $(APP_AVAILABLE_STATIC_LIBS), $(LOCAL_LIBRARIES))

LOCAL_LDLIBS := 

ifeq (,$(findstring $(LOCAL_MODULE), $(APP_AVAILABLE_STATIC_LIBS)))
include $(BUILD_SHARED_LIBRARY)
else
include $(BUILD_STATIC_LIBRARY)
endif

include $(CLEAR_VARS)

LOCAL_MODULE := mini-fribidi

LOCAL_CFLAGS := \
	-DHAVE_CONFIG_H \
	-DG_LOG_DOMAIN=\"Pango\" \
	-DPANGO_ENABLE_BACKEND \
	-DPANGO_ENABLE_ENGINE \
	-DSYSCONFDIR=\".\" \
	-DLIBDIR=\".\" \
	-DG_DISABLE_CAST_CHECKS \
	-I$(LOCAL_PATH)/pango/mini-fribidi


LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/pango/mini-fribidi

LOCAL_CPP_EXTENSION := .cpp

SRC_FILES :=  \
	fribidi.c		\
	fribidi_char_type.c 	\
	fribidi_types.c 	\


LOCAL_SRC_FILES := $(addprefix pango/mini-fribidi/, $(SRC_FILES))

LOCAL_LIBRARIES := \
	glib

LOCAL_SHARED_LIBRARIES := $(filter-out $(APP_AVAILABLE_STATIC_LIBS), $(LOCAL_LIBRARIES))
LOCAL_STATIC_LIBRARIES := $(filter $(APP_AVAILABLE_STATIC_LIBS), $(LOCAL_LIBRARIES))

LOCAL_LDLIBS := 

ifeq (,$(findstring $(LOCAL_MODULE), $(APP_AVAILABLE_STATIC_LIBS)))
include $(BUILD_SHARED_LIBRARY)
else
include $(BUILD_STATIC_LIBRARY)
endif

include $(CLEAR_VARS)

LOCAL_MODULE := pangocairo

LOCAL_CFLAGS := \
	-DHAVE_CONFIG_H \
	-DG_LOG_DOMAIN=\"Pango\" \
	-DPANGO_ENABLE_BACKEND \
	-DPANGO_ENABLE_ENGINE \
	-DSYSCONFDIR=\".\" \
	-DLIBDIR=\".\" \
	-DG_DISABLE_CAST_CHECKS \
	-DPANGO_MODULE_PREFIX=_pango_basic_fc \
	-I$(LOCAL_PATH)/pango \
	-I$(LOCAL_PATH) \
	$(NULL)


LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include

LOCAL_CPP_EXTENSION := .cpp

PANGO_SRC_FILES :=  \
	break.c					\
	ellipsize.c				\
	fonts.c					\
	glyphstring.c				\
	modules.c				\
	pango-attributes.c			\
	pango-bidi-type.c			\
	pango-color.c				\
	pango-context.c				\
	pango-coverage.c			\
	pango-engine.c				\
	pango-fontmap.c				\
	pango-fontset.c				\
	pango-glyph-item.c			\
	pango-gravity.c				\
	pango-item.c				\
	pango-language.c			\
	pango-layout.c				\
	pango-markup.c				\
	pango-matrix.c				\
	pango-renderer.c			\
	pango-script.c				\
	pango-tabs.c				\
	pango-utils.c				\
	reorder-items.c				\
	shape.c					\
	pango-enum-types.c			\
	module-defs-lang.c \
	$(NULL)

PANGO_FC_SRC_FILES :=  \
	pangofc-font.c	   	\
	pangofc-fontmap.c	\
	pangofc-decoder.c	\
	pangoft2.c \
	module-defs-fc.c	\
	pangoft2-fontmap.c	\
	pangoft2-render.c	\
	pango-ot-buffer.c	\
	pango-ot-info.c		\
	pango-ot-ruleset.c      \
	pango-ot-tag.c \
	$(NULL)

PANGO_CAIRO_SRC_FILES :=  \
 	pangocairo-context.c pangocairo-font.c \
	pangocairo-fontmap.c pangocairo-render.c \
	pangocairo-fcfont.c pangocairo-fcfontmap.c \


LOCAL_SRC_FILES := \
	$(addprefix pango/, $(PANGO_SRC_FILES)) \
	$(addprefix pango/, $(PANGO_FC_SRC_FILES)) \
	$(addprefix pango/, $(PANGO_CAIRO_SRC_FILES)) \
	modules/basic/basic-fc.c \
	$(NULL)

LOCAL_LIBRARIES := \
	cairo \
	freetype \
	fontconfig \
	opentype \
	mini-fribidi \
	gobject \
	gmodule \
	gthread \
	glib \
	pcre \
	intl \
	iconv \


LOCAL_SHARED_LIBRARIES := $(filter-out $(APP_AVAILABLE_STATIC_LIBS), $(LOCAL_LIBRARIES))
LOCAL_STATIC_LIBRARIES := $(filter $(APP_AVAILABLE_STATIC_LIBS), $(LOCAL_LIBRARIES))

LOCAL_LDLIBS := 

ifeq (,$(findstring $(LOCAL_MODULE), $(APP_AVAILABLE_STATIC_LIBS)))
include $(BUILD_SHARED_LIBRARY)
else
include $(BUILD_STATIC_LIBRARY)
endif

