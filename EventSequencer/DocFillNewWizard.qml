import QtQuick 2.0
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.2

import eventsequencer 1.0 as ES

Rectangle {
    color: "whitesmoke"

    states: [
        State {
            when: swipeView.currentIndex === 0
            PropertyChanges {
                target: btnBack
                enabled: false
            }
        },
        State {
            when: swipeView.currentIndex === 1
            PropertyChanges {
                target: btnNext
                text: "Finish"
            }
        }
    ]
    
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
                id: btnBack
                text: "Back"
            }
            Button {
                id: btnNext
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
                    font.bold: swipeView.currentIndex === 0
                }
                Label {
                    text: "2. Audio"
                    font.bold: swipeView.currentIndex === 1
                }
            }

            Rectangle {
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.leftMargin: 5

                SwipeView {
                    id: swipeView
                    anchors.fill: parent
                    clip: true
                    
                    ColumnLayout {
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
                                    implicitHeight: internalRowLayout.implicitHeight

                                    RowLayout {
                                        id: internalRowLayout
                                        anchors.left: parent.left
                                        anchors.right: parent.right
                                        Text {
                                            Layout.fillWidth: true
                                            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                                            text: "Cannot be changed later. Characters need to fit in 1 UTF-16 codepoint and be single-spaced."
                                        }
                                        Button {
                                            text: "Paste"
                                            onClicked: {
                                                contentBody.text = ES.ClipboardWrapper.getText()
                                            }
                                        }
                                    }
                                }
                                ScrollView {
                                    Layout.fillWidth: true
                                    Layout.fillHeight: true
                                    TextArea {
                                        id: contentBody
                                        font.family: "Courier New"
                                        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                                        textFormat: TextEdit.PlainText
                                    }
                                }
                            }
                        }
                    }

                    Text {
                        text: "Page 2!"
                    }
                }
            }
        }
    }
}
