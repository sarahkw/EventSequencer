import QtQuick 2.0

Rectangle {
    id: cPanel
    property int channelPixels
    property int yposition

    property int activeChannel: 0

    color: "white"

    ScrollHelper {
        id: sh
        itemSize: channelPixels
        scrollbarSize: cPanel.height
        currentPosition: yposition
    }

    Repeater {
        model: sh.itemsToRender

        Item {
            property int myIndex: index + sh.initialIndex
            anchors.left: cPanel.left
            anchors.right: cPanel.right
            y: (sh.initialPosition + index * channelPixels)
            height: channelPixels

            Rectangle {
                anchors.left: parent.left
                anchors.right: parent.right
                height: 1
                color: "lightgrey"
            }

            Text {
                text: myIndex
            }

            Rectangle {
                readonly property bool amActive: activeChannel == myIndex
                border.width: 1
                border.color: amActive ? "grey" : "whitesmoke"
                anchors.top: parent.top
                anchors.topMargin: 5
                anchors.right: parent.right
                anchors.rightMargin: 2
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 5
                width: 10

                color: amActive ? "grey" : "white"
            }

            MouseArea {
                anchors.fill: parent
                acceptedButtons: Qt.RightButton
                onClicked: {
                    activeChannel = myIndex
                }
            }
        }
    }
}
