#-------------------------------------------------
#
# Project created by QtCreator 2018-06-28T02:13:11
#
#-------------------------------------------------

#QT       -= gui

TARGET = eventsequencerlib
TEMPLATE = lib
CONFIG += staticlib

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    strip.cpp \
    document.cpp \
    eventsequencer.pb.cc \
    framesandseconds.cpp \
    waitfor.cpp \
    waitforhost.cpp \
    channel/badclockchannel.cpp \
    channel/badjschannel.cpp \
    channel/textchannel.cpp \
    channel/channeltype.cpp \
    channel/iclockrole.cpp \
    channel/channelbase.cpp \
    channel/channelfactory.cpp \
    stripext/badjsstripext.cpp \
    stripext/textstripext.cpp \
    constrainedmetricsfontutil.cpp \
    channel/badaudiochannel.cpp \
    stripext/badaudiostripext.cpp \
    channel/labelchannel.cpp \
    stripext/labelstripext.cpp \
    channel/ipb.cpp \
    channel/nodatapb.cpp


HEADERS += \
    strip.h \
    document.h \
    eventsequencer.pb.h \
    framesandseconds.h \
    waitfor.h \
    waitforhost.h \
    channel/badclockchannel.h \
    channel/badjschannel.h \
    channel/textchannel.h \
    channel/channeltype.h \
    channel/iclockrole.h \
    channel/channelbase.h \
    channel/channelfactory.h \
    stripext/badjsstripext.h \
    stripext/textstripext.h \
    constrainedmetricsfontutil.h \
    channel/badaudiochannel.h \
    stripext/badaudiostripext.h \
    channel/labelchannel.h \
    stripext/labelstripext.h \
    channel/ipb.h \
    channel/nodatapb.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

unix: CONFIG += link_pkgconfig
unix: PKGCONFIG += protobuf
