make distclean;
../launchConfigure.sh --enable-static;
make -j5 && make install;
