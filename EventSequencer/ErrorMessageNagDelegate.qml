import QtQuick 2.0
import QtQuick.Controls 2.2

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

        Row {
            spacing: 5
            Label {
                text: "ERROR"
                color: "red"
            }

            Label {
                text: modelData
            }
        }
    }
}
