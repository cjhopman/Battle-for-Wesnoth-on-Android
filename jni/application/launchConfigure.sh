# Set here your own NDK path if needed
PLATFORMVER=android-8
export NDK=/tmp/$PLATFORMVER
export PATH=$NDK/bin:$PATH
echo 'configuring'

IFS=''

GCCVER=4.6.0

TARGET_LIBGCC=`arm-linux-androideabi-gcc -mthumb -print-libgcc-file-name`
TARGET_LIBSTDC__=`arm-linux-androideabi-g++ -mthumb -print-file-name=libstdc++.a`
TARGET_LIBSUPC__=`arm-linux-androideabi-g++ -mthumb -print-file-name=libsupc++.a`

LOCAL_PATH=`dirname $0`
LOCAL_PATH=`cd $LOCAL_PATH && pwd`

INCLUDE_FLAGS=" \
	-isystem$NDK/include \
	-I$LOCAL_PATH/../SDL-1.2/include \
	-I$LOCAL_PATH/../SDL_mixer/include \
	-I$LOCAL_PATH/../SDL_net/include \
	-I$LOCAL_PATH/../SDL_ttf/include \
	-I$LOCAL_PATH/../SDL_image/include \
	"

OPT_FLAGS=" \
	-fno-strict-aliasing \
	-Os \
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
	"

ASS_FLAGS=" \
	"

COMP_FLAGS=" \
	-mthumb \
	"



echo `which arm-linux-androideabi-gcc`
CFLAGS_ANDROID="$PP_FLAGS $INCLUDE_FLAGS $OPT_FLAGS $OPT_FLAGS_RELEASE $WARN_FLAGS $ASS_FLAGS $COMP_FLAGS"

echo $CFLAGS_ANDROID

STATIC_LINK_LIBS=" \
	$TARGET_LIBGCC \
	$TARGET_LIBSTDC__ \
	$TARGET_LIBSUPC__ \
	"
LINK_FLAGS=" \
	-L$NDK/lib \
	-L$LOCAL_PATH/../../libs/armeabi/ \
	"
echo $LINK_FLAGS

LINK_LIBS=" \
	$TARGET_LIBSTDC__ \
	-lsupc++ \
	"

SYS_LIBS=" "

echo $@
env \
CFLAGS="$CFLAGS_ANDROID  $CFLAGS" \
CXXFLAGS="$CFLAGS_ANDROID $CXXFLAGS" \
CPPFLAGS="$CFLAGS_ANDROID $CPPFLAGS" \
LIBS="$SYS_LIBS $LIBS $LINK_LIBS" \
LDFLAGS="$LINK_FLAGS $LDFLAGS $SYS_LIBS" \
CC="arm-linux-androideabi-gcc" \
CXX="arm-linux-androideabi-g++" \
RANLIB="arm-linux-androideabi-ranlib" \
LD="arm-linux-androideabi-ld" \
CXXLD="arm-linux-androideabi-ld" \
AR="arm-linux-androideabi-ar" \
NM="arm-linux-androideabi-nm" \
AS="arm-linux-androideabi-as" \
STRIP="arm-linux-androideabi-strip" \
PKG_CONFIG_PATH="$NDK/lib/pkgconfig" \
PKG_CONFIG_LIBDIR="$NDK/lib" \
SDL_CONFIG="$LOCAL_PATH/sdl-config" \
./configure --host=arm-linux-androideabi --build=i686-linux-gnu --prefix="$NDK/" $@

