import QtQuick 2.0

Rectangle {
    property bool selected: false

    property color baseColor: "pink"
    property color handleColor: Qt.darker(baseColor, 1.1)
    property color selectedColor: Qt.darker(baseColor, 2)


    border.width: 1
    color: selected ? selectedColor : baseColor
    width: 100
    height: 20

    Rectangle {
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.bottom: parent.bottom

        anchors.leftMargin: parent.border.width
        anchors.topMargin: parent.border.width
        anchors.bottomMargin: parent.border.width

        width: 20
        color: parent.selected ? selectedColor : handleColor
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
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom

        anchors.rightMargin: parent.border.width
        anchors.topMargin: parent.border.width
        anchors.bottomMargin: parent.border.width

        width: 20
        color: parent.selected ? selectedColor : handleColor
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
}
