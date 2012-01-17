LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES	:=			\
	gatomicarray.c			\
	gbinding.c			\
	gboxed.c			\
	gclosure.c			\
	genums.c			\
	gobject.c			\
	gparam.c			\
	gparamspecs.c			\
	gsignal.c			\
	gsourceclosure.c		\
	gtype.c				\
	gtypemodule.c			\
	gtypeplugin.c			\
	gvalue.c			\
	gvaluearray.c			\
	gvaluetransform.c		\
	gvaluetypes.c \
	$(NULL)

LOCAL_MODULE := gobject

LOCAL_C_INCLUDES :=			\
	$(LOCAL_PATH)/android		\
	$(LOCAL_PATH)/android/gobject	\
	$(LOCAL_PATH)/android-internal	\
	$(GLIB_TOP)/android-internal	\
	$(NULL)

LOCAL_CFLAGS :=				\
	-DHAVE_CONFIG_H			\
	-DG_LOG_DOMAIN=\"GLib-GObject\"	\
	-DGOBJECT_COMPILATION		\
	-DG_DISABLE_CONST_RETURNS	\
	-DG_DISABLE_DEPRECATED  \
	$(NULL)

LOCAL_LIBRARIES := \
	glib \
	gthread \
	$(NULL)

LOCAL_SHARED_LIBRARIES := $(filter-out $(APP_AVAILABLE_STATIC_LIBS), $(LOCAL_LIBRARIES))
LOCAL_STATIC_LIBRARIES := $(filter $(APP_AVAILABLE_STATIC_LIBS), $(LOCAL_LIBRARIES))

LOCAL_EXPORT_C_INCLUDES := \
	$(LOCAL_PATH) \
	$(LOCAL_PATH)/android \
	$(NULL)

ifeq (,$(findstring $(LOCAL_MODULE), $(APP_AVAILABLE_STATIC_LIBS)))
include $(BUILD_SHARED_LIBRARY)
else
include $(BUILD_STATIC_LIBRARY)
endif

