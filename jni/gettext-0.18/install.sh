make distclean;
env LDFLAGS="-lc" ../launchConfigure.sh --disable-threads;
make -j5 && make install;
