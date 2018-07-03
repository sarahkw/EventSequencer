import QtQuick 2.0

Item {
    id: dragger

    property int position
    property bool flip: false

    x: flip ? (parent.width - position) : position

    width: 2
    property color color: "gray"

    Rectangle {
        width: 1
        color: parent.color
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.left: flip ? parent.left : undefined
        anchors.right: !flip ? parent.right : undefined
    }

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
