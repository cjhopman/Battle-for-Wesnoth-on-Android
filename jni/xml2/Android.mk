LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := xml2

LOCAL_C_INCLUDES := $(LOCAL_PATH) $(LOCAL_PATH)/include 
LOCAL_CFLAGS := \
	$(NULL)

LOCAL_CPP_EXTENSION := .cpp

LOCAL_EXPORT_C_INCLUDES := \
	$(LOCAL_PATH)/include \
	$(NULL)

LOCAL_SRC_FILES := \
	SAX.c entities.c encoding.c error.c \
	parserInternals.c parser.c tree.c hash.c list.c xmlIO.c \
	xmlmemory.c uri.c valid.c xlink.c HTMLparser.c HTMLtree.c \
	debugXML.c xpath.c xpointer.c xinclude.c nanohttp.c nanoftp.c \
	DOCBparser.c catalog.c globals.c threads.c c14n.c xmlstring.c \
	xmlregexp.c xmlschemas.c xmlschemastypes.c xmlunicode.c \
	xmlreader.c relaxng.c dict.c SAX2.c xmlwriter.c legacy.c \
	chvalid.c pattern.c xmlsave.c xmlmodule.c schematron.c \
	triostr.c trio.c trionan.c \
	$(NULL)


LOCAL_LIBRARIES := SDL-$(SDL_VERSION)

LOCAL_SHARED_LIBRARIES := $(filter-out $(APP_AVAILABLE_STATIC_LIBS), $(LOCAL_LIBRARIES))
LOCAL_STATIC_LIBRARIES := $(filter $(APP_AVAILABLE_STATIC_LIBS), $(LOCAL_LIBRARIES))

LOCAL_LDLIBS := \
	-lz \
	-ldl \
	$(NULL)

ifeq (,$(findstring $(LOCAL_MODULE), $(APP_AVAILABLE_STATIC_LIBS)))
include $(BUILD_SHARED_LIBRARY)
else
include $(BUILD_STATIC_LIBRARY)
endif

