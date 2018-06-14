import QtQuick 2.0

Rectangle {
    readonly property int selectionMode_WHOLE: 0
    readonly property int selectionMode_LEFT: 1
    readonly property int selectionMode_RIGHT: 2

    property bool selected: false
    property int selectionMode: selectionMode_WHOLE

    property color baseColor: "pink"
    property color handleColor: Qt.darker(baseColor, 1.1)
    property color selectedColor: Qt.darker(baseColor, 2)

    signal selectionClicked(var mouse)

    id: base

    border.width: 1
    color: selected ? selectedColor : baseColor
    width: 100
    height: 20

    Rectangle {
        id: leftRect
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.bottom: parent.bottom

        anchors.leftMargin: parent.border.width
        anchors.topMargin: parent.border.width
        anchors.bottomMargin: parent.border.width

        width: 20
        color: (parent.selected && parent.selectionMode != parent.selectionMode_RIGHT) ? selectedColor : handleColor
        Triangle {
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 5
            pointTo: Qt.RightArrow
            width: parent.height - 10
            height: parent.height - 10
            color: "black"
        }
    }

    Rectangle {
        id: rightRect
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom

        anchors.rightMargin: parent.border.width
        anchors.topMargin: parent.border.width
        anchors.bottomMargin: parent.border.width

        width: 20
        color: (parent.selected && parent.selectionMode != parent.selectionMode_LEFT) ? selectedColor : handleColor
        Triangle {
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: parent.right
            anchors.rightMargin: 5
            pointTo: Qt.LeftArrow
            width: parent.height - 10
            height: parent.height - 10
            color: "black"
        }

    }

    MouseArea {
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.right: leftRect.right

        acceptedButtons: Qt.RightButton
        onClicked: {
            base.selectionMode = selectionMode_LEFT
            base.selectionClicked(mouse)
        }
    }

    MouseArea {
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.left: rightRect.left

        acceptedButtons: Qt.RightButton
        onClicked: {
            base.selectionMode = selectionMode_RIGHT
            base.selectionClicked(mouse)
        }
    }

    MouseArea {
        anchors.left: leftRect.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.right: rightRect.left

        acceptedButtons: Qt.RightButton
        onClicked: {
            base.selectionMode = selectionMode_WHOLE
            base.selectionClicked(mouse)
        }
    }

}
