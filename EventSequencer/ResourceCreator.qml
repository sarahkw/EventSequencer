import QtQuick 2.0
import QtQuick.Window 2.3
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.2

import eventsequencer 1.0 as ES

Window {
    width: 400
    height: 600
    title: "Resource Creator"
    flags: Qt.Dialog

    ES.ResourceCreatorControl {
        id: rcControl
    }

    GridLayout {
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        columns: 2

        Label { text: "Input" }
        RowLayout {
            ComboBox {
                id: cmbAudioInput
                Layout.fillWidth: true
                model: rcControl.audioInputs
                Component.onCompleted: currentIndex = rcControl.defaultAudioInputIndex
            }
            Button {
                text: "Reset"
                onClicked: cmbAudioInput.currentIndex = rcControl.defaultAudioInputIndex
            }
        }

        Label { text: "Description" }
        Label {
            Layout.fillWidth: true
            text: rcControl.audioInputDescription(cmbAudioInput.currentText)
            wrapMode: Text.Wrap
        }

        Label { text: "Container" }
        ComboBox {
            id: cmbContainer
            Layout.fillWidth: true
            editable: true
            model: rcControl.supportedContainers()
        }

        Label { text: "Description" }
        Label {
            Layout.fillWidth: true
            text: rcControl.containerDescription(cmbContainer.editText)
            wrapMode: Text.Wrap
        }

        Label { text: "Audio Codec" }
        ComboBox {
            id: cmbAudioCodec
            Layout.fillWidth: true
            editable: true
            model: rcControl.supportedAudioCodecs()
        }

        Label { text: "Description" }
        Label {
            Layout.fillWidth: true
            text: rcControl.audioCodecDescription(cmbAudioCodec.editText)
            wrapMode: Text.Wrap
        }

        Label { text: "State" }
        Label {
            Layout.fillWidth: true
            text: {
                switch (rcControl.audioRecorder.state) {
                case ES.ResourceCreatorControl.StoppedState: return "StoppedState"
                case ES.ResourceCreatorControl.RecordingState: return "RecordingState"
                case ES.ResourceCreatorControl.PausedState: return "PausedState"
                }
            }
        }

        Label { text: "Output Location" }
        Label {
            id: outputLocationOut
            Layout.fillWidth: true
            text: {
                rcControl.audioRecorder.state // Dependency
                return rcControl.audioRecorder.outputLocation
            }
        }

        Row {
            Layout.columnSpan: 2
            Button {
                text: "Record"
                onClicked: {
                    rcControl.configure(cmbAudioCodec.editText, cmbContainer.editText)
                    rcControl.audioRecorder.record()
                }
            }
            Button {
                text: "Stop"
                onClicked: rcControl.audioRecorder.stop()
            }
            Button {
                text: "Debug"
                onClicked: {
                    console.info(ES.ResourceCreatorControl.StoppedState)
                    console.info(rcControl.audioRecorder.state)
                }
            }
        }

    }
}
