import QtQuick 2.0
import "util.js" as Util

Rectangle {
    property int tickSize
    property int position

    id: sbHoriz
    color: "whitesmoke"
    clip: true

    Repeater {
        model: sbHoriz.width / sbHoriz.tickSize + 2 /*1 isn't enough. I don't know why, but 2 seems to work visually.*/
        Item {
            anchors.top: sbHoriz.top
            anchors.bottom: sbHoriz.bottom
            x: position % sbHoriz.tickSize + index * sbHoriz.tickSize
            Rectangle {
                width: 1
                color: "black"
                anchors.top: parent.top
                anchors.bottom: texty.top
            }
            Text {
                id: texty
                font.pixelSize: 10
                anchors.horizontalCenter: parent.left
                text: index - Util.trunc(position / sbHoriz.tickSize)
                anchors.bottom: parent.bottom
            }
        }
    }
}
