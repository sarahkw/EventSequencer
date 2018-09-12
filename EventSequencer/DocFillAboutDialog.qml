import QtQuick 2.0
import QtQuick.Controls 2.2

Dialog {
    id: aboutDialog

    property alias text: txtEdit.text

    onClosed: destroy()

    parent: ApplicationWindow.overlay
    x: (parent.width - width) / 2
    y: (parent.height - height) / 2
    width: parent.width * 0.97
    height: parent.height * 0.97

    modal: true
    closePolicy: Popup.CloseOnEscape
    footer: DialogButtonBox {
        Button {
            text: "Ok"
            DialogButtonBox.buttonRole: DialogButtonBox.AcceptRole
        }
    }

    ScrollView {
        anchors.fill: parent
        TextArea {
            id: txtEdit
            readOnly: true
            wrapMode: TextEdit.Wrap
        }
    }
}
