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

    MessageDialog {
        id: simpleMessageDialog
    }

    ES.ManagedResources {
        id: managedResources
        fileResourceDirectory: recorderWin.fileResourceDirectory
    }

    ES.RecorderControl {
        id: recorderControl
        audioFormatHolder: recorderWin.audioFormatHolder
        sessionAudio: recorderWin.sessionAudio
        fileResourceDirectory: recorderWin.fileResourceDirectory
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

                Label { text: "Allow Overwrite" }
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

                Row {
                    Layout.columnSpan: 2
                    Button {
                        text: "Record"
                        onClicked: {
                            if (managedResources.fileResourceDirectory == "") {
                                simpleMessageDialog.text = "Resource directory doesn't exist. Save the file first."
                                simpleMessageDialog.open()
                                return
                            }
                            recorderControl.record(
                                        managedResources.withSpecifiedName(
                                            managedResources.generateResourceName(), ".au"))
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

            Connections {
                target: recorderControl
                onFileDone: {
                    fileActionFrame.writtenUrl = writtenUrl
                }
            }

            property string writtenUrl: ""
            visible: writtenUrl !== ""

            Column {
                anchors.left: parent.left
                anchors.right: parent.right
                Text {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    text: fileActionFrame.writtenUrl
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                }
                Button {
                    text: "Send to active strip"
                    enabled: {
                        if (activeCppStrip === null) return false
                        if (activeCppStrip.qmlStrip.channelControl === null) return false
                        return activeCppStrip.qmlStrip.channelControl.willAcceptResource != null
                    }
                    onClicked: {
                        activeCppStrip.resourceUrl = fileActionFrame.writtenUrl
                        fileActionFrame.writtenUrl = "" // Clear
                    }
                }
                Row {
                    Button {
                        text: "Send to active channel"
                        enabled: activeCppChannel != null
                        onClicked: {
                            var thestrip =
                                activeCppChannel.createStrip(cursorFrame,
                                                             sendToActiveChannelLength.text)
                            if (thestrip != null) {
                                thestrip.resourceUrl = fileActionFrame.writtenUrl
                                fileActionFrame.writtenUrl = "" // Clear
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
                    MessageDialog {
                        id: deleteConfirm
                        text: "Are you sure you want to delete?"
                        standardButtons: StandardButton.Yes | StandardButton.No
                        onYes: {
                            if (managedResources.deleteUrl(fileActionFrame.writtenUrl)) {
                                fileActionFrame.writtenUrl = ""
                            } else {
                                console.warn("Deletion failed")
                            }
                        }
                    }
                    text: "Delete"
                    onClicked: deleteConfirm.open()
                }
            }
        }
    }
}
