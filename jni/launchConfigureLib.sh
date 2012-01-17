#!/bin/sh

# Set here your own NDK path if needed
export PATH=$PATH:/opt/android-ndk-r4-crystax/
echo 'configuring'

IFS=''

NDK=`which ndk-build`
NDK=`dirname $NDK`
GCCVER=4.4.0
PLATFORMVER=android-8
export PATH=$NDK/build/prebuilt/linux-x86/arm-eabi-$GCCVER/bin:$PATH \
LOCAL_PATH=`dirname $0`
LOCAL_PATH=`cd $LOCAL_PATH && pwd`
export NDKROOT="$NDK/build/platforms/$PLATFORMVER/arch-arm/usr"

TARGET_LIBGCC=`arm-eabi-gcc -mthumb-interwork -print-libgcc-file-name`
TARGET_LIBSTDC__=`arm-eabi-g++ -mthumb-interwork -print-file-name=libstdc++.a`
TARGET_LIBSUPC__=`arm-eabi-g++ -mthumb-interwork -print-file-name=libsupc++.a`

INCLUDE_FLAGS=" \
	-I$NDKROOT/include \
	"

OPT_FLAGS=" \
	-Os \
	-funswitch-loops \
	"

OPT_FLAGS_RELEASE=" \
	-fomit-frame-pointer \
	"

WARN_FLAGS=" \
	-Wall \
	-Wno-unused \
	-Wno-multichar \
	"

PP_FLAGS=" \
	-D__ARM_ARCH_5__ \
	-D__ARM_ARCH_5T__ \
	-D__ARM_ARCH_5E__ \
	-D__ARM_ARCH_5TE__ \
	-DANDROID \
	-DNDEBUG \
	"

ASS_FLAGS=" \
	-Wa,--noexecstack \
	"

COMP_FLAGS=" \
	-mtune=xscale \
	-msoft-float \
	-mthumb \
	-fpic \
	-fexceptions \
	-frtti \
	-funwind-tables \
	-fstack-protector \
	-ffunction-sections \
	-fmessage-length=0 \
	-fno-short-enums \
	"


CFLAGS_ANDROID="$PP_FLAGS $INCLUDE_FLAGS $OPT_FLAGS $OPT_FLAGS_RELEASE $WARN_FLAGS $COMP_FLAGS $ASS_FLAGS"

#echo $CFLAGS_ANDROID

LINK_FLAGS=" \
	-nostdlib \
	-Wl,-z,noexecstack \
	-Wl,-shared \
	-Wl,-Bsymbolic \
	-Wl,-rpath-link=$NDKROOT/lib \
	-L$NDKROOT/lib \
	"
LINK_LIBS=" \
	-lc \
	-lm \
	"

SYS_LIBS=" $TARGET_LIBSTDC__ $TARGET_LIBSUPC__ $TARGET_LIBGCC "

env \
PATH=$NDK/build/platforms/$PLATFORMVER/arch-arm/usr/bin:$NDK/build/prebuilt/linux-x86/arm-eabi-$GCCVER/bin:$LOCAL_PATH:$PATH \
CFLAGS="$CFLAGS_ANDROID  $CFLAGS" \
CXXFLAGS="$CFLAGS_ANDROID $CXXFLAGS" \
CPPFLAGS="$CFLAGS_ANDROID $CPPFLAGS" \
LDFLAGS="$LINK_FLAGS $LDFLAGS $SYS_LIBS" \
LIBS="$SYS_LIBS $LINK_LIBS $LIBS" \
CC="$NDK/build/prebuilt/linux-x86/arm-eabi-$GCCVER/bin/arm-eabi-gcc" \
CXX="$NDK/build/prebuilt/linux-x86/arm-eabi-$GCCVER/bin/arm-eabi-g++" \
RANLIB="$NDK/build/prebuilt/linux-x86/arm-eabi-$GCCVER/bin/arm-eabi-ranlib" \
LD="$NDK/build/prebuilt/linux-x86/arm-eabi-$GCCVER/bin/arm-eabi-ld" \
CXXLD="$NDK/build/prebuilt/linux-x86/arm-eabi-$GCCVER/bin/arm-eabi-ld" \
AR="$NDK/build/prebuilt/linux-x86/arm-eabi-$GCCVER/bin/arm-eabi-ar" \
CPP="$NDK/build/prebuilt/linux-x86/arm-eabi-$GCCVER/bin/arm-eabi-cpp $CFLAGS" \
NM="$NDK/build/prebuilt/linux-x86/arm-eabi-$GCCVER/bin/arm-eabi-nm" \
AS="$NDK/build/prebuilt/linux-x86/arm-eabi-$GCCVER/bin/arm-eabi-as" \
STRIP="$NDK/build/prebuilt/linux-x86/arm-eabi-$GCCVER/bin/arm-eabi-strip" \
PKG_CONFIG_PATH="$NDK/build/platforms/$PLATFORMVER/arch-arm/usr/lib/pkgconfig" \
PKG_CONFIG_LIBDIR="$NDK/build/platforms/$PLATFORMVER/arch-arm/usr/lib" \
./configure --host=arm-linux-eabi --build=i686-linux-gnu --prefix="$NDK/build/platforms/$PLATFORMVER/arch-arm/usr/" $@

