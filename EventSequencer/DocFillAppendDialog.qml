import QtQuick 2.0
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3

import eventsequencer 1.0 as ES

Dialog {
    id: root

    onClosed: destroy()

    property var document_rebind: document

    parent: ApplicationWindow.overlay
    x: (parent.width - width) / 2
    y: (parent.height - height) / 2
    width: parent.width * 0.95
    height: parent.height * 0.5

    title: "Append"
    modal: true
    closePolicy: Popup.CloseOnEscape
    standardButtons: Dialog.Ok | Dialog.Cancel

    footer: DialogButtonBox {
        Button {
            DialogButtonBox.buttonRole: DialogButtonBox.ActionRole
            text: "Timestamp"
            onClicked: {
                var txt = Qt.formatDateTime(new Date(Date.now()), Qt.ISODate)
                txt = txt.replace("T", " ")
                txtAppendText.append(txt)
            }
        }
    }

    ES.DocFillTextContentEditor {
        id: dftce
        document: document_rebind
    }

    ColumnLayout {
        anchors.fill: parent
        Label {
            Layout.fillWidth: true
            text: "Text to append to the document:"
            wrapMode: Text.Wrap
        }
        ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            ScrollBar.vertical.policy: ScrollBar.AlwaysOn
            TextArea {
                id: txtAppendText
                wrapMode: TextEdit.Wrap
                textFormat: TextEdit.PlainText
                selectByMouse: true
                font.family: platformMonospacedFont
            }
        }
    }
}
