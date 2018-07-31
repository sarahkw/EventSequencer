import QtQuick 2.0

Item {
    AudioControl {
        id: audioControl
    }
    property Component stripPropertiesComponent: audioControl.stripPropertiesComponent
}
