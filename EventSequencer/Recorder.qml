import QtQuick 2.0
import QtQuick.Window 2.3
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.2
import QtQuick.Dialogs 1.2

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
    property ES.Strip activeCppStrip
    property var activeCppChannel
    property int cursorFrame

    property bool destroyOnHide: false
    onVisibleChanged: {
        if (destroyOnHide && visible === false) {
            destroy()
        }
    }

    CorraledResourceFile {
        id: corraledResourceFile
        fileResourceDirectory: recorderWin.fileResourceDirectory
        corralFileBase: "UNASSIGNED"
        corralFileSuffix: ".au"
    }

    MessageDialog {
        id: simpleMessageDialog
    }

    ES.RecorderControl {
        id: recorderControl
        audioFormatHolder: recorderWin.audioFormatHolder
        sessionAudio: recorderWin.sessionAudio
        fileResourceDirectory: recorderWin.fileResourceDirectory
        allowOverwrite: true
        onFileDone: corraledResourceFile.done()
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

                Label { text: "Audio Input Ready" }
                Label { text: recorderControl.audioInputReady }

                Label { text: "\u2514 Reason" }
                Label {
                    text: recorderControl.audioInputReadyStatus
                    Layout.fillWidth: true
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

                Row {
                    Layout.columnSpan: 2
                    Button {
                        text: "Record"
                        onClicked: {
                            var begin = corraledResourceFile.begin()
                            if (begin.success) {
                                recorderControl.record(begin.url)
                            } else {
                                simpleMessageDialog.text = begin.errorMsg
                                simpleMessageDialog.open()
                            }
                        }
                    }
                    Button {
                        text: "Stop"
                        onClicked: recorderControl.stop()
                    }
                }
            }
        }

        Frame {
            Layout.fillWidth: true
            id: fileActionFrame

            function possiblyAssignUrlToStrip(strip) {
                var take = corraledResourceFile.take()
                if (take.success) {
                    strip.resourceUrl = take.newUrl
                } else {
                    simpleMessageDialog.text = take.errorMsg
                    simpleMessageDialog.open()
                }
            }

            visible: corraledResourceFile.takeable

            Column {
                anchors.left: parent.left
                anchors.right: parent.right

                Button {
                    text: "Send to active strip"
                    enabled: {
                        if (activeCppStrip === null) return false
                        if (activeCppStrip.qmlStrip.channelControl === null) return false
                        return !!activeCppStrip.qmlStrip.channelControl.willAcceptResource
                    }
                    onClicked: {
                        fileActionFrame.possiblyAssignUrlToStrip(activeCppStrip)
                    }
                }
                Row {
                    Button {
                        text: "Send to active channel"
                        enabled: {
                            if (activeCppChannel === null) return false;
                            return true;

                            // Infrastructure does not support this yet. We need
                            // to be able to send to CollateChannel, which at the
                            // QML level doesn't take resources, but it proxies
                            // for some other channel type that does.
                            /*
                            if (activeCppChannel.qmlControl == null) return false;
                            return !!activeCppChannel.qmlControl.willAcceptResource
                            */
                        }
                        onClicked: {
                            var thestrip =
                                activeCppChannel.createStrip(cursorFrame,
                                                             sendToActiveChannelLength.text)
                            if (thestrip != null) {
                                fileActionFrame.possiblyAssignUrlToStrip(thestrip)
                                // TODO Maybe delete strip if we failed?
                            } else {
                                simpleMessageDialog.text = "Cannot send to active channel"
                                simpleMessageDialog.open()
                            }
                        }
                    }
                    Label {
                        anchors.top: parent.top
                        anchors.bottom: parent.bottom
                        text: "Length"
                        verticalAlignment: Text.AlignVCenter
                    }
                    TextField {
                        id: sendToActiveChannelLength
                        text: "1"
                    }
                }
                Button {
                    text: "Delete"
                    onClicked: corraledResourceFile.cancel()
                }
            }
        }
    }
}
