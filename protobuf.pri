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
