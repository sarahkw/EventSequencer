import QtQuick 2.0
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.2

import eventsequencer 1.0 as ES

Pane {
    property var closeFn
    property alias documentsPath: documentCreator.documentsPath

    property alias documentName: documentCreator.name
    property alias documentContents: documentCreator.contents

    function androidCloseHandler() {
        closeFn()
    }

    MsgBox {
        id: msgbox
    }

    ES.DocFillNewDocumentCreator {
        id: documentCreator
        sessionAudio: session.audio
    }

    states: [
        State {
            when: swipeView.currentIndex === 0
            PropertyChanges {
                target: btnBack
                enabled: false
            }
            PropertyChanges {
                target: btnNext
                onClicked: swipeView.currentIndex = 1
            }
        },
        State {
            when: swipeView.currentIndex === 1
            PropertyChanges {
                target: btnBack
                onClicked: swipeView.currentIndex = 0
            }
            PropertyChanges {
                target: btnNext
                text: "Finish"
                onClicked: {
                    var result = documentCreator.make()
                    var success = result[0]
                    if (success) {
                        closeFn()
                    } else {
                        var errorMsg = result[1]
                        msgbox.msgbox(errorMsg, "Error")
                    }
                }
            }
        }
    ]

    ES.ConstrainedMetricsFontUtil {
        id: cmfu
    }
    
    ColumnLayout {
        anchors.fill: parent

        RowLayout {
            Button {
                Layout.fillWidth: true
                text: "Cancel"
                onClicked: closeFn()
            }
            Button {
                Layout.fillWidth: true
                id: btnBack
                text: "Back"
            }
            Button {
                Layout.fillWidth: true
                id: btnNext
                text: "Next"
            }
        }

        SwipeView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            id: swipeView
            clip: true
            interactive: false
            
            ColumnLayout {
                Label {
                    Layout.fillWidth: true
                    font.bold: true
                    text: "Contents"
                }

                RowLayout {
                    Layout.fillWidth: true
                    Label { text: "Name" }
                    TextField {
                        Layout.fillWidth: true
                        text: documentCreator.name
                        onEditingFinished: documentCreator.name = text
                        placeholderText: "Use only characters valid for file names"
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
                        Pane {
                            Layout.fillWidth: true
                            palette.window: "lemonchiffon"

                            RowLayout {
                                id: internalRowLayout
                                anchors.left: parent.left
                                anchors.right: parent.right
                                Label {
                                    Layout.fillWidth: true
                                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                                    text: "Cannot be changed later. Characters must fit in 1 UTF-16 codepoint and must take up only 1 space."
                                }
                                Button {
                                    text: "Edit"
                                    Menu {
                                        id: editMenu
                                        MenuItem {
                                            text: "Cut"
                                            onClicked: contentBody.cut()
                                        }
                                        MenuItem {
                                            text: "Copy"
                                            onClicked: contentBody.copy()
                                        }
                                        MenuItem {
                                            text: "Paste"
                                            onClicked: contentBody.paste()
                                        }
                                        MenuItem {
                                            text: "Select All"
                                            onClicked: contentBody.selectAll()
                                        }
                                    }
                                    onClicked: {
                                        editMenu.open()
                                    }
                                }
                            }
                        }
                        ScrollView {
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            TextArea {
                                id: contentBody
                                font: cmfu.builtFont
                                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                                textFormat: TextEdit.PlainText
                                text: documentCreator.contents
                                onEditingFinished: documentCreator.contents = text
                            }
                        }
                    }
                }
            }

            ColumnLayout {
                Label {
                    Layout.fillWidth: true
                    font.bold: true
                    text: "Audio Settings"
                }

                ScrollView {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    contentWidth: width
                    clip: true
                    ColumnLayout {
                        anchors.left: parent.left
                        anchors.right: parent.right

                        Label {
                            Layout.fillWidth: true
                            text: "Review the audio settings in which audio is recorded. The settings default to your input device parameters, but with 1 channel."
                            wrapMode: Text.Wrap
                        }

                        GroupBox {
                            label: CheckBox {
                                id: chkCustomSettings
                                text: "Use custom settings"
                            }

                            Layout.fillWidth: true
                            GridLayout {
                                columns: 2
                                anchors.left: parent.left
                                anchors.right: parent.right

                                Label { text: "Sample Rate" }
                                ESTextField {
                                    Layout.fillWidth: true
                                    text: documentCreator.audioFormatHolder.audioFormat.sampleRate
                                    onEsEditingFinished: documentCreator.audioFormatHolder.audioFormat.sampleRate = parseInt(text, 10)
                                    enabled: chkCustomSettings.checked
                                }
                                Label { text: "Sample Size" }
                                ESTextField {
                                    Layout.fillWidth: true
                                    text: documentCreator.audioFormatHolder.audioFormat.sampleSize
                                    onEsEditingFinished: documentCreator.audioFormatHolder.audioFormat.sampleSize = parseInt(text, 10)
                                    enabled: chkCustomSettings.checked
                                }
                                Label { text: "Channels" }
                                ESTextField {
                                    Layout.fillWidth: true
                                    text: documentCreator.audioFormatHolder.audioFormat.channelCount
                                    onEsEditingFinished: documentCreator.audioFormatHolder.audioFormat.channelCount = parseInt(text, 10)
                                    enabled: chkCustomSettings.checked
                                }
                                Label { text: "Sample Type" }
                                ComboBox {
                                    Layout.fillWidth: true
                                    model: documentCreator.audioFormatHolder.sampleTypeModel
                                    currentIndex: documentCreator.audioFormatHolder.sampleTypeIndex
                                    onActivated: documentCreator.audioFormatHolder.sampleTypeIndex = index
                                    enabled: chkCustomSettings.checked
                                }
                                Label { text: "Byte Order" }
                                ComboBox {
                                    Layout.fillWidth: true
                                    model: documentCreator.audioFormatHolder.endianModel
                                    currentIndex: documentCreator.audioFormatHolder.endianIndex
                                    onActivated: documentCreator.audioFormatHolder.endianIndex = index
                                    enabled: chkCustomSettings.checked
                                }

                                Button {
                                    Layout.fillWidth: true
                                    Layout.columnSpan: 2
                                    text: "Set Default"
                                    onClicked: menu.open()
                                    enabled: chkCustomSettings.checked

                                    property Menu menu: Menu {
                                        MenuItem {
                                            text: "From Input"
                                            onTriggered: {
                                                session.audio.inputPreferredFormat(documentCreator.audioFormatHolder)
                                            }
                                        }
                                        MenuItem {
                                            text: "From Output"
                                            onTriggered: {
                                                session.audio.outputPreferredFormat(documentCreator.audioFormatHolder)
                                            }
                                        }
                                    }
                                }
                                Button {
                                    Layout.fillWidth: true
                                    Layout.columnSpan: 2
                                    text: "Test"
                                    enabled: chkCustomSettings.checked

                                    onClicked: {
                                        msgbox.msgbox(session.audio.testFormatSupport(documentCreator.audioFormatHolder))
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
