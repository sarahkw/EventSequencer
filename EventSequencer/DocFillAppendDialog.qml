import QtQuick 2.0
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3

import eventsequencer 1.0 as ES

Dialog {
    id: root

    property bool appendAndAssignMode: false

    signal appended(int rangeBegin, int rangeEnd)

    onClosed: destroy()

    parent: ApplicationWindow.overlay
    x: (parent.width - width) / 2
    //y: (parent.height - height) / 2
    y: parent.height * 0.05
    width: parent.width * 0.95
    height: parent.height * 0.75

    title: appendAndAssignMode ? "Append and Assign" : "Append"
    modal: true
    closePolicy: Popup.CloseOnEscape
    standardButtons: Dialog.Cancel

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
        Button {
            DialogButtonBox.buttonRole: DialogButtonBox.AcceptRole
            text: "OK"
            enabled: txtAppendText.text.length > 0
        }
    }

    onAccepted: {
        var result = dftce.appendText(document, txtAppendText.text, chkAutoNewlines.checked)
        var rangeBegin = result[0]
        var rangeEnd = result[1]
        root.appended(rangeBegin, rangeEnd)
    }

    ES.DocFillTextContentEditor {
        id: dftce
    }

    ColumnLayout {
        anchors.fill: parent
        Label {
            Layout.fillWidth: true
            text: appendAndAssignMode ?
                      "Text to append to the document and automatically assign afterwards:" :
                      "Text to append to the document:"
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
                // selectByMouse: true -- this breaks scrolling on Android
                font.family: platformMonospacedFont
            }
        }
        CheckBox {
            id: chkAutoNewlines
            text: "Automatically insert newlines"
            checked: true
        }
    }
}
