import QtQuick 2.0

Rectangle {
    property bool selected: false

    color: selected ? "black" : "red"
    width: 100
    height: 20

    Triangle {
        anchors.verticalCenter: parent.verticalCenter
        pointTo: Qt.LeftArrow
        width: parent.height - 10
        height: parent.height - 10
        color: "black"
    }
    Triangle {
        anchors.verticalCenter: parent.verticalCenter
        anchors.right: parent.right
        pointTo: Qt.RightArrow
        width: parent.height - 10
        height: parent.height - 10
        color: "black"
    }
}
