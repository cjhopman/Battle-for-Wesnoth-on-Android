LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := intl 

LOCAL_CFLAGS := \
	-DLOCALEDIR=\"\" \
	-DLOCALE_ALIAS_PATH=\"\" \
	-DLIBDIR=\"\" \
	-DBUILDING_LIBINTL \
	-DBUILDING_DLL \
	-DIN_LIBINTL \
	-DENABLE_RELOCATABLE=1 \
	-DIN_LIBRARY \
	-DINSTALLDIR=\"\" \
	-DNO_XMALLOC \
	-Dset_relocation_prefix=libintl_set_relocation_prefix \
	-Drelocate=libintl_relocate \
	-DDEPENDS_ON_LIBICONV=1 \
	-DHAVE_CONFIG_H \
	-I$(LOCAL_PATH)/gettext-runtime/intl \
	-I$(LOCAL_PATH)/gettext-runtime \
	$(NULL)

LOCAL_EXPORT_C_INCLUDES := \
	$(LOCAL_PATH)/gettext-runtime/intl \
	$(NULL)

LOCAL_CPP_EXTENSION := .cpp

SRC_FILES :=  \
	bindtextdom.c \
	dcgettext.c \
	dgettext.c \
	gettext.c \
	finddomain.c \
	hash-string.c \
	loadmsgcat.c \
	localealias.c \
	textdomain.c \
	l10nflist.c \
	explodename.c \
	dcigettext.c \
	dcngettext.c \
	dngettext.c \
	ngettext.c \
	plural.c \
	plural-exp.c \
	localcharset.c \
	threadlib.c \
	lock.c \
	relocatable.c \
	langprefs.c \
	localename.c \
	log.c \
	printf.c \
	setlocale.c \
	version.c \
	osdep.c \
	intl-compat.c \
	$(NULL)

LOCAL_SRC_FILES := $(addprefix gettext-runtime/intl/, $(SRC_FILES))

LOCAL_LIBRARIES := \
	iconv \
	$(NULL)


LOCAL_SHARED_LIBRARIES := $(filter-out $(APP_AVAILABLE_STATIC_LIBS), $(LOCAL_LIBRARIES))
LOCAL_STATIC_LIBRARIES := $(filter $(APP_AVAILABLE_STATIC_LIBS), $(LOCAL_LIBRARIES))

LOCAL_LDLIBS := 

ifeq (,$(findstring $(LOCAL_MODULE), $(APP_AVAILABLE_STATIC_LIBS)))
include $(BUILD_SHARED_LIBRARY)
else
include $(BUILD_STATIC_LIBRARY)
endif

