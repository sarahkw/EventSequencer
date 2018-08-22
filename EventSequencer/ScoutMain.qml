import QtQuick 2.0
import QtQuick.Window 2.3
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.2
import QtQuick.Dialogs 1.2

import eventsequencer 1.0 as ES

Window {
    id: root
    visible: true
    width: 540
    height: 720
    title: "EvSeq Scout"

    property int cursorFrame: 0

    Component.onCompleted: {
        if (commandLine.file !== undefined) {
            txtOpenUrl.text = commandLine.file
            commandLine.file = undefined // Good for one use
        }
    }

    ES.Document {
        id: document
    }

    ES.Session {
        id: session
    }

    Component {
        id: docFillComponent
        DocFillPage {
            // Pass to DocFillPage Begin
            property var cppChannel
            property ES.Session session
            property ES.Document document
            property int cursorFrame
            property var changeCursorFrame // Fn
            // Pass to DocFillPage End

            function closeFn() {
                stackView.pop()
            }
        }
    }

    StackView {
        id: stackView
        anchors.fill: parent

        initialItem: ColumnLayout {
            id: unnamedParent_7d06
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
                text: root.title
                font.family: "Courier"
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.WordWrap

                font.pointSize: 18
            }

            ColumnLayout {
                Layout.fillWidth: true
                TextField {
                    Layout.fillWidth: true
                    id: txtOpenUrl
                    enabled: !btnLoad.checked
                }
                RowLayout {
                    Layout.fillWidth: true
                    Button {
                        Layout.fillWidth: true
                        text: "Browse"
                        onClicked: browseDialog.open()
                        FileDialog {
                            id: browseDialog
                            nameFilters: ["Event sequencer files (*.evseq)", "All files (*)"]
                            defaultSuffix: "evseq"
                            onAccepted: txtOpenUrl.text = fileUrl
                            Component.onCompleted: {
                                if (Qt.platform.os == "android") {
                                    folder = "file:///sdcard"
                                }
                            }
                        }
                        enabled: !btnLoad.checked
                    }
                    Button {
                        Layout.fillWidth: true
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
                spacing: unnamedParent_7d06 / 40

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
                            onClicked: {
                                stackView.push(docFillComponent, {
                                    cppChannel: modelData,
                                    session: session,
                                    document: document,
                                    cursorFrame: Qt.binding(function () { return root.cursorFrame }),
                                    changeCursorFrame: function (newFrame) { root.cursorFrame = newFrame }
                                })
                            }
                        }
                    }
                }
            }
        }
    }
}
