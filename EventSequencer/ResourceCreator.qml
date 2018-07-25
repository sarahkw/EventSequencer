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
    }
}
