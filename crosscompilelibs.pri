# XXX Yeah, you can see this was for protobuf first, with mp3lame added after.
android {
    contains(ANDROID_TARGET_ARCH,armeabi-v7a) {
        PROTOBUF_THING = android
    }
    !contains(ANDROID_TARGET_ARCH,armeabi-v7a) {
        PROTOBUF_THING = android-x86
    }

    ANDROID_EXTRA_LIBS = \
        $$PWD/../protobuf-cross-compile/$$PROTOBUF_THING/lib/libprotobuf.so \
        $$PWD/../protobuf-cross-compile/$$PROTOBUF_THING/lib/libmp3lame.so
}
!android: PROTOBUF_THING = desktop

INCLUDEPATH += /home/sarah/protobuf-cross-compile/$$PROTOBUF_THING/include
LIBS += -L/home/sarah/protobuf-cross-compile/$$PROTOBUF_THING/lib -lprotobuf -lmp3lame
# XXX on desktop it does pick up the system one.

########################################################################
# BUILD INSTRUCTIONS
########################################################################

# PROTOBUF:

# First, compile/install x86 version:
# 
# [sarah@slappy protobuf]$ ./configure --prefix=/home/sarah/protobuf-cross-compile/desktop
# [sarah@slappy protobuf]$ make -j4 install
# 
# $ /home/sarah/Android/android-ndk-r10e/build/tools/make-standalone-toolchain.sh --toolchain=arm-linux-androideabi-4.9 --stl=gnustl --arch=arm --platform=android-16 --install-dir=/home/sarah/protobuf-cross-compile/android-toolchain
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
# You'll have to hack the configure script to use shared gnustl.
#     
#     # PORTME: override above test on systems where it is broken
#     case $host_os in
#     interix[3-9]*)
#       # Interix 3.5 installs completely hosed .la files for C++, so rather than
#       # hack all around it, let's just trust "g++" to DTRT.
#       predep_objects_CXX=
#       postdep_objects_CXX=
#       postdeps_CXX=
#       ;;
#     esac
# >>> postdeps_CXX=`echo "${postdeps_CXX}" | $SED 's/-lstdc++/-lgnustl_shared/'`
#
# And then confirm:
# $ readelf  --dynamic libprotobuf.so
# ...
#  0x00000001 (NEEDED)                     Shared library: [libgnustl_shared.so]
#
# ................................................................................
# 
# ./configure --host=arm-linux-androideabi --with-protoc=/home/sarah/protobuf-cross-compile/desktop/bin/protoc --prefix=/home/sarah/protobuf-cross-compile/android
# [sarah@slappy protobuf]$ make -j4 install

# LAME:

# DESKTOP
# 
# [sarah@slappy lame-3.100]$ ./configure --prefix=/home/sarah/protobuf-cross-compile/desktop --disable-analyzer-hooks --disable-decoder
# [sarah@slappy lame-3.100]$ make -j4
# [sarah@slappy lame-3.100]$ make install
# 
# ANDROID
# 
# Use the exports from the protobuf section, and then:
# 
# [sarah@slappy lame-3.100]$ ./configure --host=arm-linux-androideabi --prefix=/home/sarah/protobuf-cross-compile/android --disable-analyzer-hooks --disable-decoder --disable-frontend
# [sarah@slappy lame-3.100]$ make -j4
# [sarah@slappy lame-3.100]$ make install

########################################################################
# GO BACK AND COMPILE FOR ANDROID x86 for the emulator
########################################################################

# /home/sarah/Android/android-ndk-r10e/build/tools/make-standalone-toolchain.sh --toolchain=x86-4.9 --stl=gnustl --arch=x86 --platform=android-16 --install-dir=/home/sarah/protobuf-cross-compile/android-toolchain-x86

# Protobuf:

# # Add the standalone toolchain to the search path.
# export PATH=$PATH:/home/sarah/protobuf-cross-compile/android-toolchain-x86/bin
# 
# # Tell configure what tools to use.
# target_host=i686-linux-android
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

# ...

# ./configure --host=i686-linux-android --with-protoc=/home/sarah/protobuf-cross-compile/desktop/bin/protoc --prefix=/home/sarah/protobuf-cross-compile/android-x86

# Lame:

# ./configure --host=i686-linux-android --prefix=/home/sarah/protobuf-cross-compile/android-x86 --disable-analyzer-hooks --disable-decoder --disable-frontend
