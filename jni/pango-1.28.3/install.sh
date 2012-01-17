make distclean;
../launchConfigure.sh --without-x --enable-static;
make -j5 && make install;
