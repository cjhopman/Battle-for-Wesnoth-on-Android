make distclean;
../launchConfigure.sh --disable-arm-simd --disable-arm-neon;
make -j5 && make install;
