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
    standardButtons: Dialog.Cancel
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
        Button {
            DialogButtonBox.buttonRole: DialogButtonBox.AcceptRole
            text: "OK"
            enabled: previewResult[0]
        }
    }

    property var previewResult: dftce.truncatePreview(document, cursorFrame)

    onAccepted: {
        dftce.truncate(document, cursorFrame)
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
            clip: true
            TextEdit {
                id: txtTruncateText
                width: parent.width
                wrapMode: TextEdit.Wrap
                font.family: platformMonospacedFont
                readOnly: true
                textFormat: TextEdit.PlainText
                text: previewResult[1]
            }
        }
    }
}
