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
        palette.window: myColor
        property var messageItems: {
            var spacePos = modelData.indexOf(" ")
            return [modelData.substr(0, spacePos),
                    modelData.substr(spacePos + 1)]
        }

        property color myColor: {
            if (messageItems[0] === "ERROR") {
                return "red"
            } else {
                return "green"
            }
        }

        RowLayout {


            anchors.left: parent.left
            anchors.right: parent.right
            Label {
                text: chld.messageItems[0]
                color: chld.myColor
            }

            Label {
                Layout.fillWidth: true
                text: chld.messageItems[1]
                wrapMode: Text.Wrap
            }
        }
    }
}
