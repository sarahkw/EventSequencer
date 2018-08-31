import QtQuick 2.0
import QtQuick.Window 2.3
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.2

import eventsequencer 1.0 as ES

ApplicationWindow { // Use ApplicationWindow to support popup overlay
    id: root
    visible: true

    // Trying to see if not setting width and height resolves the
    // issue where on Android, sometimes the "Window" doesn't line up
    // with the screen until you resize it by flipping orientations.
    //
    // TODO If this didn't fix the problem, remove this
    property var widthAndHeight: {
        if (Qt.platform.os == "android") {
            // Yes, there is a
            //   Unable to assign null to int
            // error, but at least the assignment doesn't go through.
            return [null, null]
        } else {
            return [540, 720]
        }
    }
    width: widthAndHeight[0]
    height: widthAndHeight[1]

    title: "DocFill"

    property int cursorFrame: 0

    ES.DocFillSettings {
        id: applicationSettings
    }

    ES.DocumentManager {
        id: documentManager
        documentsPath: applicationSettings.documentsPath
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

            id: docFillComponentItem

            function closeFn() {
                stackView.pop()
            }

            Component.onCompleted: qodn.target = docFillComponentItem
        }
    }

    Component {
        id: newDocumentComponent
        DocFillNewWizard {
            function closeFn() {
                stackView.pop()
            }
        }
    }

    // Don't reset document until the item is actually gone, because otherwise,
    // we're swiping the rug from beneath it and it starts writing errors.
    ES.QmlObjectDestroyedNotifier {
        id: qodn
        onTargetDestroyed: {
            document.reset()
            root.cursorFrame = 0
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
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.WordWrap

                font.pointSize: 18
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

            RowLayout {
                Layout.fillWidth: true
                Layout.fillHeight: true

                ScrollView {
                    Layout.fillHeight: true
                    Layout.alignment: Qt.AlignTop
                    Column {
                        spacing: 5
                        RoundButton {
                            text: "New"
                            radius: 5
                            Component.onCompleted: background.color = Qt.lighter("lime", 1.8)
                            onClicked: stackView.push(newDocumentComponent)
                        }
                        RoundButton {
                            text: "Tutorial"
                            radius: 5
                            Component.onCompleted: background.color = Qt.lighter("lime", 1.8)
                        }
                        RoundButton {
                            text: "Settings"
                            radius: 5
                            Component.onCompleted: background.color = Qt.lighter("lime", 1.8)
                        }
                        RoundButton {
                            text: "Quit"
                            radius: 5
                            Component.onCompleted: background.color = Qt.lighter("lime", 1.8)
                            onClicked: Qt.quit()
                        }
                    }
                }

                ScrollView {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    clip: true
                    ListView {
                        model: documentManager.items
                        spacing: 5
                        delegate: RoundButton {
                            radius: 5
                            anchors.left: parent.left
                            anchors.right: parent.right
                            text: modelData.displayName
                            Component.onCompleted: background.color = "whitesmoke"
                            onClicked: {
                                txtErrorMessage.visible = false

                                var result = document.loadFilePath(modelData.filePath)
                                var success = result[0]
                                if (!success) {
                                    var errmsg = result[1]
                                    txtErrorMessage.text = errmsg
                                    txtErrorMessage.visible = true
                                    return
                                }

                                var programChannel = document.defaultProgramChannel()
                                if (programChannel === null) {
                                    txtErrorMessage.text = "Program not found on index 0"
                                    txtErrorMessage.visible = true
                                    return
                                }

                                stackView.push(docFillComponent, {
                                    cppChannel: programChannel,
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
