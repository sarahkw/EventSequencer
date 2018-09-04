import QtQuick 2.0
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import eventsequencer 1.0 as ES

Dialog {
    id: root

    property string filePath
    property var returnResult // fn

    ES.DocumentManagerDeleter {
        id: deleter
        filePath: root.filePath
    }

    onClosed: destroy()

    parent: ApplicationWindow.overlay
    x: (parent.width - width) / 2
    y: (parent.height - height) / 2
    width: parent.width * 0.95
    height: parent.height * 0.5

    title: "Delete"
    modal: true
    closePolicy: Popup.CloseOnEscape
    standardButtons: Dialog.Yes | Dialog.No

    onAccepted: {
        var result = deleter.actuallyDelete()
        returnResult(result)
    }

    ColumnLayout {
        anchors.fill: parent
        Label {
            Layout.fillWidth: true
            text: "Are you sure you wish to permanently delete the following files/directories?"
            wrapMode: Text.Wrap
        }
        ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            ScrollBar.vertical.policy: ScrollBar.AlwaysOn
            ListView {
                model: deleter.queuedForDeletion
                delegate: Label {
                    text: {
                        if (modelData.isDirectory) {
                            modelData.fileName + "/"
                        } else {
                            modelData.fileName
                        }
                    }
                }
            }
        }
    }
}
