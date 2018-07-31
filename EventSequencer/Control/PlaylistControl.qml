import QtQuick 2.0

Item {
    AudioControl {
        id: audioControl
    }
    property bool willAcceptResource: true
    property Component stripPropertiesComponent: audioControl.stripPropertiesComponent
}
