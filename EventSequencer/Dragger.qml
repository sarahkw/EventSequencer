import QtQuick 2.0

Rectangle {
    id: dragger

    property int position
    property bool flip: false

    x: flip ? (parent.width - position) : position

    width: 2
    color: "gray"

    MouseArea {
        id: draggerMouseArea
        anchors.fill: parent
        cursorShape: Qt.SizeHorCursor
        drag.target: dragger
        drag.axis: Drag.XAxis
    }

    onXChanged: {
        position = flip ? (parent.width - x) : x
    }
}
