import QtQuick 2.0
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3

import eventsequencer 1.0 as ES

Dialog {
    id: root

    signal dirtied()

    onClosed: destroy()

    parent: ApplicationWindow.overlay
    x: (parent.width - width) / 2
    y: (parent.height - height) / 2
    width: parent.width * 0.95
    height: parent.height * 0.5

    title: "Truncate"
    modal: true
    closePolicy: Popup.CloseOnEscape
    standardButtons: Dialog.Ok | Dialog.Cancel
    footer: DialogButtonBox {
        Button {
            DialogButtonBox.buttonRole: DialogButtonBox.ActionRole
            text: "Copy"
            onClicked: {
                txtTruncateText.selectAll()
                txtTruncateText.copy()
                txtTruncateText.deselect()
            }
        }
    }

    onAccepted: {
        // TODO
        root.dirtied()
    }

    ES.DocFillTextContentEditor {
        id: dftce
    }

    Item {} // Empty item to prevent auto resize

    ColumnLayout {
        anchors.fill: parent
        Label {
            Layout.fillWidth: true
            text: "This is the text that will be truncated:"
            wrapMode: Text.Wrap
        }
        ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            ScrollBar.vertical.policy: ScrollBar.AlwaysOn
            contentWidth: width
            TextEdit {
                id: txtTruncateText
                width: parent.width
                wrapMode: TextEdit.Wrap
                font.family: platformMonospacedFont
                readOnly: true
                textFormat: TextEdit.PlainText
                text: dftce.truncatePreview(document, cursorFrame)
            }
        }
    }
}
