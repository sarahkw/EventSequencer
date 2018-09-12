import QtQuick 2.0
import QtQuick.Window 2.3
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.2

import eventsequencer 1.0 as ES

ApplicationWindow { // Use ApplicationWindow to support popup overlay
    id: root
    visible: true

//    // Trying to see if not setting width and height resolves the
//    // issue where on Android, sometimes the "Window" doesn't line up
//    // with the screen until you resize it by flipping orientations.
//    //
//    // TODO If this didn't fix the problem, remove this
//    property var widthAndHeight: {
//        if (Qt.platform.os == "android") {
//            // Yes, there is a
//            //   Unable to assign null to int
//            // error, but at least the assignment doesn't go through.
//            return [null, null]
//        } else {
//            return [540, 720]
//        }
//    }
//    width: widthAndHeight[0]
//    height: widthAndHeight[1]

    // Let's try this again. I have removed the Qt Widgets stuff from
    // Android since, maybe that could have helped.
    width: 540
    height: 720
    
    title: "DocFill"

    property int cursorFrame: 0

    // Android back button.
    onClosing: {
        if (Qt.platform.os === "android") {
            var handler = stackView.currentItem.androidCloseHandler
            if (handler !== undefined) {
                handler()
                close.accepted = false
            }
        }
    }

    MsgBox {
        id: msgbox
    }

    ES.DocFillSettings {
        id: applicationSettings
    }

    ES.DocFillDatabase {
        id: docFillDatabase
        Component.onCompleted: {
            if (errorMessage !== "") {
                console.error(errorMessage)
            }
        }
    }

    ES.DocumentManager {
        id: documentManager
        documentsPath: applicationSettings.documentsPath
        onBeforeScanDocuments: ES.AndroidPermissionRequest.requestStorage()
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
            anchors.topMargin: 15
            anchors.bottom: parent.bottom
            spacing: 15

            Column {
                Layout.fillWidth: true
                Text {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    text: root.title
                    horizontalAlignment: Text.AlignHCenter
                    wrapMode: Text.WordWrap

                    font.pointSize: 18
                }
                Text {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    text: "0.1.4 dev"
                    horizontalAlignment: Text.AlignHCenter
                    wrapMode: Text.WordWrap
                }
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
                            onClicked: {
                                var component = Qt.createComponent("DocFillNewWizard.qml")
                                var obj = component.createObject(null, {
                                                                     closeFn : function () {
                                                                         stackView.pop()
                                                                         obj.destroy()
                                                                         documentManager.refresh()
                                                                     },
                                                                     documentsPath: Qt.binding(function () { return applicationSettings.documentsPath })
                                                                 })
                                stackView.push(obj)
                            }
                        }
                        RoundButton {
                            text: "Stats"
                            radius: 5
                            Component.onCompleted: background.color = Qt.lighter("lime", 1.8)

                            onClicked: {
                                var component = Qt.createComponent("DocFillStatsDialog.qml")
                                var obj = component.createObject(root, {})
                                obj.open()
                            }
                        }
                        RoundButton {
                            text: "Tutorial"
                            radius: 5
                            Component.onCompleted: background.color = Qt.lighter("lime", 1.8)
                            onClicked: {
                                // TODO This is mostly copy and paste, make this a function
                                var component = Qt.createComponent("DocFillNewWizard.qml")
                                var obj = component.createObject(null, {
                                                                     closeFn : function () {
                                                                         stackView.pop()
                                                                         obj.destroy()
                                                                         documentManager.refresh()
                                                                     },
                                                                     documentsPath: Qt.binding(function () { return applicationSettings.documentsPath }),
                                                                     documentName: "Tutorial",
                                                                     documentContents: ES.QmlFileReader.readFile(':/Tutorial.txt')
                                                                 })
                                stackView.push(obj)
                            }
                        }
                        RoundButton {
                            text: "Settings"
                            radius: 5
                            Component.onCompleted: background.color = Qt.lighter("lime", 1.8)

                            Dialog {
                                id: settingsDialog

                                parent: ApplicationWindow.overlay
                                x: (parent.width - width) / 2
                                y: (parent.height - height) / 2
                                width: parent.width * 0.95

                                title: "Settings"
                                modal: true
                                closePolicy: Popup.CloseOnEscape
                                standardButtons: Dialog.Ok | Dialog.Cancel

                                GroupBox {
                                    anchors.left: parent.left
                                    anchors.right: parent.right
                                    title: "Documents Path"
                                    ColumnLayout {
                                        anchors.left: parent.left
                                        anchors.right: parent.right
                                        TextField {
                                            id: txtDocsPath
                                            Layout.fillWidth: true
                                            selectByMouse: true
                                            text: applicationSettings.documentsPath
                                            readOnly: chkDocsPathUseDefault.checked
                                        }
                                        CheckBox {
                                            id: chkDocsPathUseDefault
                                            text: "Use Default"
                                            onToggled: if (checked) txtDocsPath.text = applicationSettings.defaultDocumentsPath
                                            checked: applicationSettings.documentsPathIsDefault
                                        }
                                    }
                                }

                                onAccepted: {
                                    if (chkDocsPathUseDefault.checked) {
                                        applicationSettings.unsetDocumentsPath()
                                    } else {
                                        applicationSettings.documentsPath = txtDocsPath.text
                                    }
                                }
                            }
                            onClicked: settingsDialog.open()
                        }
                        RoundButton {
                            text: "About"
                            radius: 5
                            Component.onCompleted: background.color = Qt.lighter("lime", 1.8)

                            onClicked: {
                                var component = Qt.createComponent("DocFillAboutDialog.qml")
                                var obj = component.createObject(root, {
                                                                     title: "About",
                                                                     text: ES.QmlFileReader.readFile(":/About.txt")
                                                                 })
                                obj.open()
                            }
                        }
                        RoundButton {
                            text: "Privacy"
                            radius: 5
                            Component.onCompleted: background.color = Qt.lighter("lime", 1.8)

                            onClicked: {
                                var component = Qt.createComponent("DocFillAboutDialog.qml")
                                var obj = component.createObject(root, {
                                                                     title: "Privacy",
                                                                     text: ES.QmlFileReader.readFile(":/Privacy.txt")
                                                                 })
                                obj.open()
                            }
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

                                var result = document.loadFilePath(modelData.filePath)
                                var success = result[0]
                                if (!success) {
                                    var errmsg = result[1]
                                    msgbox.msgbox(errmsg)
                                    return
                                }

                                var programChannel = document.defaultProgramChannel()
                                if (programChannel === null) {
                                    msgbox.msgbox("Program not found on index 0")
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

                            Menu {
                                id: subMenu
                                MenuItem {
                                    text: "Delete"
                                    onClicked: {
                                        var component = Qt.createComponent("DocFillDeleteDialog.qml")
                                        var obj = component.createObject(root, {
                                                                             //visible: true,
                                                                             filePath: modelData.filePath,
                                                                             returnResult: function (result) {
                                                                                 if (result !== "") {
                                                                                    msgbox.msgbox(result, "Delete")
                                                                                 }

                                                                                 // Note: .refresh() must come after msgbox because
                                                                                 // it will destroy this delegate and msgbox reference
                                                                                 // will be destroyed.
                                                                                 documentManager.refresh()
                                                                             }
                                                                         })
                                        // Wow I can't say visible=true instead or else
                                        // there are layout problems.
                                        obj.open()
                                    }
                                }
                                MenuItem {
                                    text: "Backfill stats"
                                    onClicked: {
                                        var component = Qt.createComponent("DocFillBackfillStatsDialog.qml")
                                        var obj = component.createObject(root, {
                                                                             filePath: modelData.filePath,
                                                                             returnResult: function (result) {
                                                                                 if (result !== "") {
                                                                                    msgbox.msgbox(result, "Backfill stats")
                                                                                 }
                                                                             }
                                        })
                                        obj.open()
                                    }
                                }
                            }

                            onPressAndHold: {
                                subMenu.open()
                            }
                        }
                    }
                }

                Item {
                    Layout.alignment: Qt.AlignTop
                    implicitHeight: instructionLabel.implicitWidth
                    implicitWidth: instructionLabel.implicitHeight
                    Label {
                        id: instructionLabel
                        text: "Long press for menu."
                        color: "lightgray"
                        y: -implicitHeight
                        rotation: 90
                        transformOrigin: Item.BottomLeft
                    }
                }
            }
        }
    }
}
