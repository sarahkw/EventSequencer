import QtQuick 2.0

Rectangle {
    id: dragger

    property int position
    x: parent.width - position

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
        position = parent.width - x
    }
}
