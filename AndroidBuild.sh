#!/bin/sh

LOCAL_PATH=`dirname $0`
LOCAL_PATH=`cd $LOCAL_PATH && pwd`

../setEnvironment.sh make -j5 && cp -f scummvm/scummvm libapplication.so
