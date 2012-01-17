LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
	pcre_compile.c \
	pcre_chartables.c \
	pcre_config.c \
	pcre_dfa_exec.c \
	pcre_exec.c \
	pcre_fullinfo.c \
	pcre_get.c \
	pcre_globals.c \
	pcre_newline.c \
	pcre_ord2utf8.c \
	pcre_study.c \
	pcre_tables.c \
	pcre_try_flipped.c \
	pcre_ucp_searchfuncs.c \
	pcre_xclass.c \
	$(NULL)

LOCAL_MODULE:= pcre

LOCAL_C_INCLUDES :=				\
	$(GLIB_TOP)/android-internal		\
	$(GLIB_TOP)/glib/android-internal	\
	$(GLIB_TOP)/glib \
	$(GLIB_TOP) \
	$(GLIB_TOP)/android \
	$(NULL)

LOCAL_CFLAGS := \
    -DG_LOG_DOMAIN=\"GLib-GRegex\" \
    -DSUPPORT_UCP \
    -DSUPPORT_UTF8 \
    -DNEWLINE=-1 \
    -DMATCH_LIMIT=10000000 \
    -DMATCH_LIMIT_RECURSION=10000000 \
    -DMAX_NAME_SIZE=32 \
    -DMAX_NAME_COUNT=10000 \
    -DMAX_DUPLENGTH=30000 \
    -DLINK_SIZE=2 \
    -DPCRE_STATIC \
    -DPOSIX_MALLOC_THRESHOLD=10 \
	$(NULL)

LOCAL_LIBRARIES := \
	$(NULL)

LOCAL_SHARED_LIBRARIES := $(filter-out $(APP_AVAILABLE_STATIC_LIBS), $(LOCAL_LIBRARIES))
LOCAL_STATIC_LIBRARIES := $(filter $(APP_AVAILABLE_STATIC_LIBS), $(LOCAL_LIBRARIES))

LOCAL_EXPORT_C_INCLUDES := \
	$(LOCAL_PATH) \
	$(NULL)

LOCAL_LDLIBS := 

ifeq (,$(findstring $(LOCAL_MODULE), $(APP_AVAILABLE_STATIC_LIBS)))
include $(BUILD_SHARED_LIBRARY)
else
include $(BUILD_STATIC_LIBRARY)
endif

