include(gtest_dependency.pri)

TEMPLATE = app
QT += multimedia
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG += thread

HEADERS += \
        tst_testcasename.h \
    tst_framesandseconds.h \
    tst_collatenonoverlappingsegments.h \
    tst_concatiodevice.h \
    tst_visualpositionmanager.h \
    tst_channelindex.h \
    printto_channelindex.h \
    tst_managedresources.h \
    tst_samplemodifyingiodevice.h \
    printto_qstring.h \
    tst_endianmodifyingiodevice.h

SOURCES += \
        main.cpp

unix:!macx: LIBS += -L$$OUT_PWD/../eventsequencerlib/ -leventsequencerlib

INCLUDEPATH += $$PWD/../eventsequencerlib
DEPENDPATH += $$PWD/../eventsequencerlib

unix:!macx: PRE_TARGETDEPS += $$OUT_PWD/../eventsequencerlib/libeventsequencerlib.a

include(../crosscompilelibs.pri)
