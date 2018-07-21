include(gtest_dependency.pri)

TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG += thread

HEADERS += \
        tst_testcasename.h \
    tst_strip.h \
    tst_framesandseconds.h \
    tst_collatenonoverlappingsegments.h \
    tst_mapgenerate.h \
    tst_merge.h \
    tst_groupby.h

SOURCES += \
        main.cpp

unix:!macx: LIBS += -L$$OUT_PWD/../eventsequencerlib/ -leventsequencerlib

INCLUDEPATH += $$PWD/../eventsequencerlib
DEPENDPATH += $$PWD/../eventsequencerlib

unix:!macx: PRE_TARGETDEPS += $$OUT_PWD/../eventsequencerlib/libeventsequencerlib.a

unix: CONFIG += link_pkgconfig
unix: PKGCONFIG += protobuf
