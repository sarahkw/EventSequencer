import QtQuick 2.0
import QtQuick.Window 2.3
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.2
import QtQuick.Dialogs 1.2

import eventsequencer 1.0 as ES

Window {
    visible: true
    width: 540
    height: 720
    title: "EvSeq Program Launcher"

    ES.Document {
        id: document
    }

    ColumnLayout {
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.leftMargin: parent.width / 10
        anchors.rightMargin: parent.width / 10
        anchors.top: parent.top
        anchors.topMargin: parent.height / 20
        anchors.bottom: parent.bottom
        spacing: parent.height / 20
        Text {
            Layout.fillWidth: true
            text: qsTr("EvSeq Program Launcher")
            font.family: "Courier"
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.WordWrap

            font.pointSize: 18
        }

        RowLayout {
            Layout.fillWidth: true
            TextField {
                id: txtOpenUrl
                Layout.fillWidth: true
                enabled: !btnLoad.checked
            }
            Button {
                text: "Browse"
                onClicked: browseDialog.open()
                FileDialog {
                    id: browseDialog
                    onAccepted: txtOpenUrl.text = fileUrl
                }
                enabled: !btnLoad.checked
            }
            Button {
                id: btnLoad
                checkable: true
                text: "Load"
                onToggled: {
                    txtErrorMessage.visible = false
                    if (checked) {
                        var result = document.load(txtOpenUrl.text)
                        var success = result[0]
                        if (success) {
                            lvForModel.model = document.channelsProvidingProgram()
                        } else {
                            var errmsg = result[1]
                            txtErrorMessage.text = errmsg
                            txtErrorMessage.visible = true
                        }
                    } else {
                        lvForModel.model = null
                        document.reset()
                    }
                }
            }
        }

        Text {
            id: txtErrorMessage
            Layout.fillWidth: true
            font.pointSize: 9
            color: "red"
            text: "This is an error message"
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.Wrap
            visible: false
        }

        ColumnLayout {
            id: unnamedParent_6834
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: parent.height / 40

            Text {
                Layout.fillWidth: true
                text: "Programs"
                font.family: "Courier"
                horizontalAlignment: Text.AlignHCenter
                font.pointSize: 12
            }
            ScrollView {
                Layout.fillWidth: true
                Layout.fillHeight: true
                ListView {
                    id: lvForModel
                    clip: true
                    spacing: 5

                    delegate: Button {
                        text: modelData + ""
                        width: unnamedParent_6834.width
                    }

                }
            }
        }
    }
}
