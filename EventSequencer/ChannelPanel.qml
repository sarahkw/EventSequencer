import QtQuick 2.0

Rectangle {
    id: cPanel
    property int channelPixels
    property int yposition

    color: "white"

    Repeater {
        model: (cPanel.height / channelPixels + 2)
        Rectangle {
            height: 1
            anchors.left: cPanel.left
            anchors.right: cPanel.right
            y: (yposition % channelPixels + index * channelPixels)
            color: "lightgrey"
        }
    }
}
