import QtQuick 2.0
import QtQuick.Window 2.3
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.2

import eventsequencer 1.0 as ES

Window {
    id: recorderWin
    width: 400
    height: 600
    title: "Recorder"
    flags: Qt.Dialog

    property QtObject audioFormatHolder
    property QtObject sessionAudio
    property string fileResourceDirectory

    property bool destroyOnHide: false
    onVisibleChanged: {
        if (destroyOnHide && visible === false) {
            destroy()
        }
    }

    ES.ManagedResources {
        id: managedResources
        fileResourceDirectory: recorderWin.fileResourceDirectory
    }

    ES.RecorderControl {
        id: recorderControl
        audioFormatHolder: recorderWin.audioFormatHolder
        sessionAudio: recorderWin.sessionAudio
    }

    GridLayout {
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        columns: 2

        Label { text: "Audio Input Ready" }
        Label { text: recorderControl.audioInputReady }

        Label { text: "\u2514 Reason" }
        Label { text: recorderControl.audioInputReadyStatus }

        Label { text: "Output Type" }
        RowLayout {
            Layout.fillWidth: true
            ComboBox {
                Layout.fillWidth: true
                model: ["Managed random", "Managed", "Filesystem"]
            }
            Button {
                text: "Browse"
            }
        }

        Label { text: "\u2514 Path" }
        ESTextField {
            Layout.fillWidth: true
        }

        Label { text: "\u2514 Allow Overwrite" }
        CheckBox {
            checked: recorderControl.allowOverwrite
            onCheckedChanged: recorderControl.allowOverwrite = checked
        }

        Label { text: "Error" }
        Label {
            Layout.fillWidth: true
            text: recorderControl.error
        }

        Label { text: "Audio State" }
        Label {
            Layout.fillWidth: true
            text: recorderControl.audioState
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
                onClicked: recorderControl.record()
            }
            Button {
                text: "Stop"
                onClicked: recorderControl.stop()
            }
            Button {
                text: "Debug"
                onClicked: recorderControl.debug()
            }
        }
    }
}
