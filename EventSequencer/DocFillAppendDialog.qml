import QtQuick 2.0
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3

Dialog {
    id: root

    onClosed: destroy()

    parent: ApplicationWindow.overlay
    x: (parent.width - width) / 2
    y: (parent.height - height) / 2
    width: parent.width * 0.95
    height: parent.height * 0.5

    title: "Append"
    modal: true
    closePolicy: Popup.CloseOnEscape
    standardButtons: Dialog.Ok | Dialog.Cancel


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
                wrapMode: TextEdit.Wrap
                selectByMouse: true
                font.family: platformMonospacedFont
            }
        }
    }
}
