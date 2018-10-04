QT += quick
!android {
QT += widgets  # For FileDialog on Linux
}
QT += quickcontrols2 # For QQuickStyle
QT += multimedia
QT += sql
android {
QT += androidextras
}
CONFIG += c++14

android {
TARGET = DocFill
}

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        main.cpp \
    registerqmltypes.cpp

android {
RESOURCES += docfill.qrc
}
!android {
RESOURCES += qml.qrc
}

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    registerqmltypes.h

########################################################################
# Begin eventsequencerlib

unix:!macx: LIBS += -L$$OUT_PWD/../eventsequencerlib/ -leventsequencerlib

INCLUDEPATH += $$PWD/../eventsequencerlib
DEPENDPATH += $$PWD/../eventsequencerlib

unix:!macx: PRE_TARGETDEPS += $$OUT_PWD/../eventsequencerlib/libeventsequencerlib.a

# End eventsequencerlib
########################################################################

########################################################################
# Begin androidlib

android {
LIBS += -L$$OUT_PWD/../androidlib/ -landroidlib

INCLUDEPATH += $$PWD/../androidlib
DEPENDPATH += $$PWD/../androidlib

PRE_TARGETDEPS += $$OUT_PWD/../androidlib/libandroidlib.a
}

# End androidlib
########################################################################

########################################################################
# Begin batchservicelib

unix:!macx: LIBS += -L$$OUT_PWD/../batchservicelib/ -lbatchservicelib

INCLUDEPATH += $$PWD/../batchservicelib
DEPENDPATH += $$PWD/../batchservicelib

unix:!macx: PRE_TARGETDEPS += $$OUT_PWD/../batchservicelib/libbatchservicelib.a

# End batchservicelib
########################################################################


include(../crosscompilelibs.pri)

DISTFILES += \
    android/AndroidManifest.xml \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradlew \
    android/res/values/libs.xml \
    android/build.gradle \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew.bat \
    android/res/drawable-hdpi/ic_stat_docfill.png \
    android/res/drawable-mdpi/ic_stat_docfill.png \
    android/res/drawable-xhdpi/ic_stat_docfill.png \
    android/res/drawable-xxhdpi/ic_stat_docfill.png \
    android/src/com/gmail/doctorfill456/docfill/DfActivity.java \
    android/src/com/gmail/doctorfill456/docfill/DfService.java

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android
