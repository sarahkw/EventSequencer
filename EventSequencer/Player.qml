import QtQuick 2.0
import QtQuick.Window 2.3
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.2
import QtQuick.Dialogs 1.2

import eventsequencer 1.0 as ES

Window {
    id: playerWin
    width: 400
    height: 600
    title: "Player"
    flags: Qt.Dialog

    property QtObject audioFormatHolder
    property QtObject sessionAudio
    property string fileResourceDirectory
    property ES.Strip activeCppStrip

    property bool destroyOnHide: false
    onVisibleChanged: {
        if (destroyOnHide && visible === false) {
            destroy()
        }
    }

    ES.ManagedResources {
        id: managedResources
        fileResourceDirectory: playerWin.fileResourceDirectory
    }

    ES.PlayerControl {
        id: playerControl
        audioFormatHolder: playerWin.audioFormatHolder
        sessionAudio: playerWin.sessionAudio
    }

    ColumnLayout {
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top

        Frame {
            Layout.fillWidth: true

            GridLayout {
                id: glayRecord
                anchors.fill: parent
                columns: 2

                Label { text: "Audio Output Ready" }
                Label { text: playerControl.audioOutputReady }

                Label { text: "\u2514 Reason" }
                Label {
                    text: playerControl.audioOutputReadyStatus
                    Layout.fillWidth: true
                }

                Label { text: "Error" }
                Label {
                    Layout.fillWidth: true
                    text: playerControl.error
                }

                Label { text: "Audio State" }
                Label {
                    Layout.fillWidth: true
                    text: playerControl.audioState
                }

                Row {
                    Layout.columnSpan: 2
                    Button {
                        text: "Play"
                        onClicked: playerControl.play()
                    }
                    Button {
                        text: "Stop"
                        onClicked: playerControl.stop()
                    }
                }
            }
        }
    }
}
