import QtQuick 2.0
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import eventsequencer 1.0 as ES

Dialog {
    id: root

    property string filePath

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
    footer: DialogButtonBox {
        Button {
            text: "Ok"
            DialogButtonBox.buttonRole: DialogButtonBox.AcceptRole
        }
    }

    ColumnLayout {
        anchors.fill: parent
        ListView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            model: deleter.queuedForDeletion
            delegate: Label {
                text: modelData
            }
        }
    }
}
