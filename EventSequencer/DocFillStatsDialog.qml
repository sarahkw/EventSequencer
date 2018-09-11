import QtQuick 2.0
import QtQuick.Controls 2.2

Dialog {
    id: root
    title: "Stats"

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

    Label {
        text: "Stats here!"
    }
}
