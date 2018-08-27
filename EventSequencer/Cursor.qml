import QtQuick 2.0

Rectangle {
    id: root
    width: 2
    color: "lime"
    property alias text: cursorText.text
    readonly property int draggableSectionHeight: height - cursorText.y + 10

    Rectangle {
        id: cursorBackground
        color: root.color
        anchors.left: root.right
        anchors.top: cursorText.top
        anchors.bottom: cursorText.bottom
        anchors.right: cursorText.right
        anchors.rightMargin: -10
        anchors.topMargin: -2
        anchors.bottomMargin: -2
    }

    Text {
        id: cursorText
        anchors.left: root.right
        anchors.bottom: root.bottom
        anchors.leftMargin: 10
        anchors.bottomMargin: 10
        font.pixelSize: 12
    }
}
