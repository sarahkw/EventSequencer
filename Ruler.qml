import QtQuick 2.0

Rectangle {
    property int tickSize
    property int position

    id: sbHoriz
    color: "whitesmoke"

    Repeater {
        model: sbHoriz.width / sbHoriz.tickSize + 1
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
                text: index - Math.floor(position / sbHoriz.tickSize)
                anchors.bottom: parent.bottom
            }
        }
    }
}
