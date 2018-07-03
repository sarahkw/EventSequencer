import QtQuick 2.0

Rectangle {
    id: cPanel
    property int channelPixels
    property int yposition

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
                border.width: 1
                border.color: "lightgrey"
                anchors.top: parent.top
                anchors.topMargin: 5
                anchors.right: parent.right
                anchors.rightMargin: 2
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 5
                width: 15
            }
        }
    }
}
