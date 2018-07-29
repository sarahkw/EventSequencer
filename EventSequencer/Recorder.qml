import QtQuick 2.0
import QtQuick.Window 2.3
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.2

import eventsequencer 1.0 as ES

Window {
    width: 400
    height: 600
    title: "Recorder"
    flags: Qt.Dialog

    property bool destroyOnHide: false
    onVisibleChanged: {
        if (destroyOnHide && visible === false) {
            destroy()
        }
    }

    ES.RecorderControl {
        id: recorderControl
    }

    GridLayout {
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        columns: 2

        Label { text: "State" }
        Label {
            Layout.fillWidth: true
            text: "Value"
        }

        Label { text: "Output Location" }
        Label {
            Layout.fillWidth: true
            text: "Value"
        }

        Row {
            Layout.columnSpan: 2
            Button {
                text: "Record"
            }
            Button {
                text: "Stop"
            }
        }
    }
}
