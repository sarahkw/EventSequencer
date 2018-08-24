import QtQuick 2.0
import QtQuick.Window 2.3
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.2
import QtQuick.Dialogs 1.2

import eventsequencer 1.0 as ES
import eventsequencer.playable 1.0 as ESPlayable

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

    ESPlayable.SingleUrl {
        id: playableSingleUrl
        fileResourceDirectory: playerWin.fileResourceDirectory
        singleUrl: txtSingleUrl.text
    }

    ESPlayable.StripsList {
        id: playableStripsList
        fileResourceDirectory: playerWin.fileResourceDirectory
        selectionMode: ESPlayable.StripsList.SelectionMode.Strip // Initial thing on UI
        selectedStrip: activeCppStrip
        selectedChannel: activeCppChannel
        cursorFrame: playerWin.cursorFrame
    }

    ES.PlayerControl {
        id: playerControl
        audioFormatHolder: playerWin.audioFormatHolder
        sessionAudio: playerWin.sessionAudio
        autoStopOnIdle: true
        playable: playableStripsList // Initial thing on UI
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
                Column {
                    Layout.fillWidth: true
                    RadioButton { checked: true
                                  text: "Strip"            ; onToggled: { if (checked) { playerControl.playable = playableStripsList; playableStripsList.selectionMode = ESPlayable.StripsList.SelectionMode.Strip             } } }
                    RadioButton { text: "ChannelFromBegin" ; onToggled: { if (checked) { playerControl.playable = playableStripsList; playableStripsList.selectionMode = ESPlayable.StripsList.SelectionMode.ChannelFromBegin  } } }
                    RadioButton { text: "ChannelFromCursor"; onToggled: { if (checked) { playerControl.playable = playableStripsList; playableStripsList.selectionMode = ESPlayable.StripsList.SelectionMode.ChannelFromCursor } } }
                    RadioButton { text: "ChannelOnCursor"  ; onToggled: { if (checked) { playerControl.playable = playableStripsList; playableStripsList.selectionMode = ESPlayable.StripsList.SelectionMode.ChannelOnCursor   } } }
                    RadioButton { text: "SingleUrl"        ; onToggled: { if (checked) { playerControl.playable = playableSingleUrl; } } }
                }

                Label { text: "SingleUrl" }
                TextField {
                    id: txtSingleUrl
                    Layout.fillWidth: true
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

        GroupBox {
            Layout.fillHeight: true
            Layout.fillWidth: true
            title: "Strips Report"
            ScrollView {
                anchors.fill: parent
                visible: playerControl.playable == playableStripsList
                TextEdit {
                    text: playableStripsList.currentStripsReport
                }
            }
        }
    }
}
