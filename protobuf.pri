#unix: CONFIG += link_pkgconfig
#unix: PKGCONFIG += protobuf
android: PROTOBUF_THING = android
!android: PROTOBUF_THING = desktop
INCLUDEPATH += /home/sarah/protobuf-cross-compile/$$PROTOBUF_THING/include
LIBS += -L/home/sarah/protobuf-cross-compile/$$PROTOBUF_THING/lib -lprotobuf

contains(ANDROID_TARGET_ARCH,armeabi-v7a) {
    ANDROID_EXTRA_LIBS = \
        $$PWD/../protobuf-cross-compile/android/lib/libprotobuf.so
}

# Compile/install x86 version:
# 
# [sarah@slappy protobuf]$ ./configure --prefix=/home/sarah/protobuf-cross-compile/desktop
# [sarah@slappy protobuf]$ make -j4 install
# 
# $ /home/sarah/Android/android-ndk-r10e/build/tools/make-standalone-toolchain.sh --stl=gnustl --arch=arm --platform=android-16 --install-dir=/home/sarah/protobuf-cross-compile/android-toolchain
# 
# ................................................................................
# 
# # Add the standalone toolchain to the search path.
# export PATH=$PATH:/home/sarah/protobuf-cross-compile/android-toolchain/bin
# 
# # Tell configure what tools to use.
# target_host=arm-linux-androideabi
# export AR=$target_host-ar
# export AS=$target_host-gcc
# export CC=$target_host-gcc
# export CXX=$target_host-g++
# export LD=$target_host-ld
# export STRIP=$target_host-strip
# 
# # Tell configure what flags Android requires.
# export CFLAGS="-fPIE -fPIC"
# export LDFLAGS="-pie -llog"     # <-- -llog is needed for the protoc compiler
# 
# ................................................................................
# 
# ./configure --host=arm-linux-androideabi --with-protoc=/home/sarah/protobuf-cross-compile/desktop/bin/protoc --prefix=/home/sarah/protobuf-cross-compile/android
# [sarah@slappy protobuf]$ make -j4 install