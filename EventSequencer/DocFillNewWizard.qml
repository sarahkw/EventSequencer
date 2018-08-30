import QtQuick 2.0
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.2

Rectangle {
    color: "whitesmoke"
    ColumnLayout {
        anchors.fill: parent
        anchors.topMargin: 5

        RowLayout {
            Layout.rightMargin: 5
            Label {
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignHCenter
                font.pointSize: 12
                text: "New Document"
            }
            Button {
                text: "Cancel"
                onClicked: closeFn()
            }
            Button {
                text: "Back"
            }
            Button {
                text: "Next"
            }
        }

        RowLayout {
            Layout.leftMargin: 5
            Layout.fillWidth: true
            Layout.fillHeight: true

            ColumnLayout {
                Layout.alignment: Qt.AlignTop
                Label {
                    text: "1. Content"
                    font.bold: true
                }
                Label {
                    text: "2. Audio"
                }
            }

            Rectangle {
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.leftMargin: 5

                ColumnLayout {
                    anchors.fill: parent

                    RowLayout {
                        Layout.fillWidth: true
                        Label { text: "Name" }
                        TextField {
                            Layout.fillWidth: true
                        }
                    }

                    GroupBox {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        title: "Contents"
                        padding: 2

                        // An extra item so that GroupBox doesn't try
                        // to auto calculate the size and create a
                        // binding loop.
                        Item { }

                        ColumnLayout {
                            anchors.fill: parent
                            Rectangle {
                                Layout.fillWidth: true
                                color: "lemonchiffon"
                                implicitHeight: textContentExplanation.implicitHeight

                                Text {
                                    anchors.left: parent.left
                                    anchors.right: parent.right
                                    id: textContentExplanation
                                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                                    text: "Contents cannot be changed later. Characters need to fit in a single UTF-16 codepoint and must only take up a single space."
                                }
                            }
                            ScrollView {
                                Layout.fillWidth: true
                                Layout.fillHeight: true
                                TextArea {
                                    font.family: "Courier New"
                                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                                    textFormat: TextEdit.PlainText
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
