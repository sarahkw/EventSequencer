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
    property var activeCppChannel
    property int cursorFrame

    property bool destroyOnHide: false
    onVisibleChanged: {
        if (destroyOnHide && visible === false) {
            destroy()
        }
    }

    ES.PlayerControl {
        id: playerControl
        audioFormatHolder: playerWin.audioFormatHolder
        sessionAudio: playerWin.sessionAudio
        fileResourceDirectory: playerWin.fileResourceDirectory

        selectionMode: rdoActiveStrip.checked ? ES.PlayerControl.SelectionMode.Strip : ES.PlayerControl.SelectionMode.Channel
        selectedStrip: activeCppStrip
        selectedChannel: activeCppChannel
        onlyStripsAfter: rdoAfterCursor.checked ? cursorFrame : null
    }

    ColumnLayout {
        anchors.fill: parent

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

                Label { text: "Selection" }
                Row {
                    Layout.fillWidth: true
                    RadioButton {
                        id: rdoActiveStrip
                        text: qsTr("Strip")
                        checked: true
                    }
                    RadioButton {
                        text: qsTr("Channel")
                    }
                    RadioButton {
                        id: rdoAfterCursor
                        text: qsTr("Channel After Cursor")
                    }
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

        ScrollView {
            Layout.fillHeight: true
            TextEdit {
                text: playerControl.currentStripsReport
            }
        }


    }
}
