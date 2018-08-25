#-------------------------------------------------
#
# Project created by QtCreator 2018-06-28T02:13:11
#
#-------------------------------------------------

#QT       -= gui
QT += multimedia

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
    channel/labelchannel.cpp \
    stripext/labelstripext.cpp \
    wordwrappedtexttrack.cpp \
    showmultiplelinesononeline.cpp \
    channel/collatechannel.cpp \
    channel/collatechannelrefreshevent.cpp \
    collatenonoverlappingsegments.cpp \
    saferfilereplacement.cpp \
    managedresources.cpp \
    stripext/audiostripext.cpp \
    stripext/playliststripext.cpp \
    channel/audiochannel.cpp \
    channel/playlistchannel.cpp \
    session.cpp \
    sessionaudio.cpp \
    aufileheader.cpp \
    recordercontrol.cpp \
    audioformatholder.cpp \
    documentstripsonchannel.cpp \
    audiocontrol.cpp \
    playercontrol.cpp \
    concatiodevice.cpp \
    channelindex.cpp \
    channel/spanchannel.cpp \
    visualpositionmanager.cpp \
    channelindexfactory.cpp \
    collides.cpp \
    channel/docfillchannel.cpp \
    countchannelindex.cpp \
    errorreportingcontext.cpp \
    conditionalerror.cpp \
    qmlimmediatedestructor.cpp \
    samplemodifyingiodevice.cpp \
    endianmodifyingiodevice.cpp \
    playable/stripslist.cpp \
    playable/singleurl.cpp \
    playable/tone.cpp \
    playable/playablebase.cpp \
    watchforstripsintersectingrange.cpp \
    managedresourcereport.cpp


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
    channel/labelchannel.h \
    stripext/labelstripext.h \
    wordwrappedtexttrack.h \
    showmultiplelinesononeline.h \
    channel/collatechannel.h \
    channel/collatechannelrefreshevent.h \
    collatenonoverlappingsegments.h \
    saferfilereplacement.h \
    managedresources.h \
    stripext/audiostripext.h \
    stripext/playliststripext.h \
    channel/audiochannel.h \
    channel/playlistchannel.h \
    session.h \
    sessionaudio.h \
    aufileheader.h \
    recordercontrol.h \
    audioformatholder.h \
    documentstripsonchannel.h \
    audiocontrol.h \
    playercontrol.h \
    concatiodevice.h \
    channelindex.h \
    channel/spanchannel.h \
    visualpositionmanager.h \
    channelindexfactory.h \
    collides.h \
    channel/docfillchannel.h \
    countchannelindex.h \
    errorreportingcontext.h \
    conditionalerror.h \
    qmlimmediatedestructor.h \
    samplemodifyingiodevice.h \
    endianmodifyingiodevice.h \
    playable/stripslist.h \
    playable/singleurl.h \
    playable/tone.h \
    playable/playablebase.h \
    watchforstripsintersectingrange.h \
    managedresourcereport.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

include(../protobuf.pri)
