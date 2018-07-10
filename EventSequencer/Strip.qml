import QtQuick 2.0

Rectangle {
    property bool selected: false
    property int selectionMode: SelectionMode.Whole

    property color baseColor: "pink"
    property color handleColor: Qt.darker(baseColor, 1.1)
    property color selectedColor: Qt.darker(baseColor, 2)

    signal selectionClicked(var mouse)

    id: base

    border.width: 1
    color: selected ? selectedColor : baseColor
    height: 20

    readonly property int handleWidth: 20
    readonly property int smallModeWidth: handleWidth + handleWidth + 3
    readonly property int minimumWidth: 3 // Always want to be visible.

    states: [
        State {
            // Hide handles when we're too small
            when: width < smallModeWidth
            name: "too_small"
            PropertyChanges {
                target: leftRect
                width: 0
                visible: false
            }
            PropertyChanges {
                target: rightRect
                width: 0
                visible: false
            }
        }
    ]

    Rectangle {
        id: leftRect
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.bottom: parent.bottom

        anchors.leftMargin: parent.border.width
        anchors.topMargin: parent.border.width
        anchors.bottomMargin: parent.border.width

        width: handleWidth
        color: (parent.selected && parent.selectionMode !== SelectionMode.Right) ? selectedColor : handleColor
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

        width: handleWidth
        color: (parent.selected && parent.selectionMode !== SelectionMode.Left) ? selectedColor : handleColor
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
            base.selectionMode = SelectionMode.Left
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
            base.selectionMode = SelectionMode.Right
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
            base.selectionMode = SelectionMode.Whole
            base.selectionClicked(mouse)
        }
    }

}
