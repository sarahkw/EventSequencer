import QtQuick 2.0
import "util.js" as Util

Rectangle {
    id: cPanel
    property int channelPixels
    property int yposition

    color: "white"

    readonly property int initialIndex: -Math.ceil(yposition / channelPixels)
    readonly property int initialPosition: Util.negmod(yposition, channelPixels)

    Repeater {
        model: (cPanel.height / channelPixels +
                2 /*one for before, one for after*/
                )

        Item {
            property int myIndex: index + initialIndex
            anchors.left: cPanel.left
            anchors.right: cPanel.right
            y: (initialPosition + index * channelPixels)
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
