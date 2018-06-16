import QtQuick 2.0

Rectangle {
    property int tickSize
    property int position

    id: sbHoriz
    color: "whitesmoke"
    clip: true

    // No Math.trunc yet.
    function ceilFloorTowardsZero(x) {
        if (x < 0) {
            return Math.ceil(x)
        } else {
            return Math.floor(x)
        }
    }

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
                text: index - ceilFloorTowardsZero(position / sbHoriz.tickSize)
                anchors.bottom: parent.bottom
            }
        }
    }
}
