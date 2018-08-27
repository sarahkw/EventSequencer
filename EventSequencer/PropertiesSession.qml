import QtQuick 2.0
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.2
import QtQml.Models 2.3

GridLayout {
    anchors.left: parent.left
    anchors.right: parent.right

    columns: 2

    Label {
        text: "Audio"
    }
    CheckBox {
        Layout.fillWidth: true
        text: "Set"

        checked: session.wantAudio
        onCheckedChanged: session.wantAudio = checked

        Loader {
            id: sessionAudioControlsLoader
            sourceComponent: session.wantAudio ? sessionAudioControls : null
            property var audio: session.wantAudio ? session.audio : null
            Component {
                id: sessionAudioControls
                ObjectModel {
                    Label { text: "\u2514 Input" }
                    ComboBox {
                        Layout.fillWidth: true
                        model: audio && audio.inputDevicesModel
                        currentIndex: audio ? audio.selectedInputIndex : -1
                        onActivated: audio.selectedInputIndex = index
                    }
                    Label { text: "\u2514 Output" }
                    ComboBox {
                        Layout.fillWidth: true
                        model: audio && audio.outputDevicesModel
                        currentIndex: audio ? audio.selectedOutputIndex : -1
                        onActivated: audio.selectedOutputIndex = index
                    }
                }
            }
        }
    }

    Repeater {
        model: sessionAudioControlsLoader.item
    }

}
