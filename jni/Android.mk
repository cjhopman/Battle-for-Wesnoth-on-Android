# If SDL_Mixer should link to libMAD
SDL_MIXER_USE_LIBMAD :=
ifneq ($(strip $(filter mad, $(COMPILED_LIBRARIES))),)
SDL_MIXER_USE_LIBMAD := 1
endif

ifneq ($(findstring -crystax,$(TARGET_CC)),)
CRYSTAX_TOOLCHAIN=1
endif

NDK_MODULE_PATH := /tmp/android-8/:

include $(call all-subdir-makefiles)
