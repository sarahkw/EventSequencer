import QtQuick 2.0
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3

Item {
    implicitHeight: chld.height
    Frame {
        id: chld
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.leftMargin: 20
        anchors.rightMargin: 20
        Component.onCompleted: background.color = "white"
        palette.window: "red"

        RowLayout {
            anchors.left: parent.left
            anchors.right: parent.right
            Label {
                text: "ERROR"
                color: "red"
            }

            Label {
                Layout.fillWidth: true
                text: modelData
                wrapMode: Text.Wrap
            }
        }
    }
}
