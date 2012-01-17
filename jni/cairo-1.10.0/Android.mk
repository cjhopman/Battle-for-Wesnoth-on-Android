LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := cairo 

LOCAL_CFLAGS := \
	-DHAVE_CONFIG_H \
	-D_REENTRANT \
	-DCAIRO_NO_MUTEX=1 \
	-I$(LOCAL_PATH)/src \
	$(NULL)

LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/src

LOCAL_CPP_EXTENSION := .cpp

SRC_FILES :=  \
	cairo-analysis-surface.c \
	cairo-arc.c \
	cairo-array.c \
	cairo-atomic.c \
	cairo-base64-stream.c \
	cairo-base85-stream.c \
	cairo-bentley-ottmann.c \
	cairo-bentley-ottmann-rectangular.c \
	cairo-bentley-ottmann-rectilinear.c \
	cairo-botor-scan-converter.c \
	cairo-boxes.c \
	cairo.c \
	cairo-cache.c \
	cairo-clip.c \
	cairo-color.c \
	cairo-composite-rectangles.c \
	cairo-debug.c \
	cairo-device.c \
	cairo-fixed.c \
	cairo-font-face.c \
	cairo-font-face-twin.c \
	cairo-font-face-twin-data.c \
	cairo-font-options.c \
	cairo-freelist.c \
	cairo-freed-pool.c \
	cairo-gstate.c \
	cairo-hash.c \
	cairo-hull.c \
	cairo-image-info.c \
	cairo-image-surface.c \
	cairo-lzw.c \
	cairo-matrix.c \
	cairo-recording-surface.c \
	cairo-misc.c \
	cairo-mutex.c \
	cairo-observer.c \
	cairo-output-stream.c \
	cairo-paginated-surface.c \
	cairo-path-bounds.c \
	cairo-path.c \
	cairo-path-fill.c \
	cairo-path-fixed.c \
	cairo-path-in-fill.c \
	cairo-path-stroke.c \
	cairo-pattern.c \
	cairo-pen.c \
	cairo-polygon.c \
	cairo-rectangle.c \
	cairo-rectangular-scan-converter.c \
	cairo-region.c \
	cairo-rtree.c \
	cairo-scaled-font.c \
	cairo-slope.c \
	cairo-spans.c \
	cairo-spline.c \
	cairo-stroke-style.c \
	cairo-surface.c \
	cairo-surface-fallback.c \
	cairo-surface-clipper.c \
	cairo-surface-offset.c \
	cairo-surface-snapshot.c \
	cairo-surface-subsurface.c \
	cairo-surface-wrapper.c \
	cairo-system.c \
	cairo-tor-scan-converter.c \
	cairo-toy-font-face.c \
	cairo-traps.c \
	cairo-unicode.c \
	cairo-user-font.c \
	cairo-version.c \
	cairo-wideint.c \
	cairo-cff-subset.c \
	\
	cairo-scaled-font-subsets.c \
	cairo-truetype-subset.c \
	cairo-type1-fallback.c \
	cairo-type1-subset.c \
	cairo-type3-glyph-surface.c \
	\
	cairo-pdf-operators.c \
	\
	cairo-png.c \
	\
	cairo-ft-font.c \
	\
	cairo-ps-surface.c \
	\
	cairo-svg-surface.c \
	\
	$(NULL)

	#cairo-xml-surface.c \

LOCAL_SRC_FILES := $(addprefix src/, $(SRC_FILES))

LOCAL_LIBRARIES := \
	gobject \
	glib \
	pixman \
	fontconfig \
	freetype \
	png \
	intl \
	iconv \
	xml2 \
	$(NULL)


LOCAL_SHARED_LIBRARIES := $(filter-out $(APP_AVAILABLE_STATIC_LIBS), $(LOCAL_LIBRARIES))
LOCAL_STATIC_LIBRARIES := $(filter $(APP_AVAILABLE_STATIC_LIBS), $(LOCAL_LIBRARIES))

LOCAL_LDLIBS := -lz

ifeq (,$(findstring $(LOCAL_MODULE), $(APP_AVAILABLE_STATIC_LIBS)))
include $(BUILD_SHARED_LIBRARY)
else
include $(BUILD_STATIC_LIBRARY)
endif

