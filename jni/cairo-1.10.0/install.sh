make distclean;
../launchConfigure.sh --disable-xlib;
make -j5 && make install;
