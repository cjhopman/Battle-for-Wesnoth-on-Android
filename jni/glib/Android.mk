# This file is the top android makefile for all sub-modules.

LOCAL_PATH := $(call my-dir)

GLIB_TOP := $(LOCAL_PATH)

include $(CLEAR_VARS)

include $(GLIB_TOP)/glib/Android.mk
include $(GLIB_TOP)/gthread/Android.mk
include $(GLIB_TOP)/gobject/Android.mk
include $(GLIB_TOP)/gmodule/Android.mk
#include $(GLIB_TOP)/gio/Android.mk
#include $(GLIB_TOP)/tests/Android.mk

