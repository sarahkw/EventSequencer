import QtQuick 2.0
import QtQuick.Controls 2.3

Rectangle {
    id: cPanel
    property int channelPixels
    property int yposition

    property int activeChannel: 0

    // Temporary!
    property var channelToSelection: ({})

    color: "white"

    ScrollHelper {
        id: sh
        itemSize: channelPixels
        scrollbarSize: cPanel.height
        currentPosition: yposition
    }

    Repeater {
        model: sh.itemsToRender

        Item {
            property int myIndex: index + sh.initialIndex
            anchors.left: cPanel.left
            anchors.right: cPanel.right
            y: (sh.initialPosition + index * channelPixels)
            height: channelPixels

            Rectangle {
                anchors.left: parent.left
                anchors.right: parent.right
                height: 1
                color: "lightgrey"
            }

            ComboBox {
                anchors.leftMargin: 10
                anchors.rightMargin: 10
                anchors.left: parent.left
                anchors.right: selectIndicator.left
                anchors.verticalCenter: parent.verticalCenter
                model: ["", "Dummy", "Clock", "Label", "JavaScript", "Sound", "Print"]
                currentIndex: channelToSelection[myIndex] === undefined ? 0 : channelToSelection[myIndex]
                onCurrentIndexChanged: {
                    channelToSelection[myIndex] = currentIndex
                }
                onFocusChanged: focus = false
            }

            Rectangle {
                id: selectIndicator
                readonly property bool amActive: activeChannel == myIndex
                border.width: 1
                border.color: amActive ? "lightgrey" : "whitesmoke"
                anchors.top: parent.top
                anchors.topMargin: 5
                anchors.right: parent.right
                anchors.rightMargin: 2
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 5
                width: 10

                color: amActive ? "lightgrey" : "white"
            }

            MouseArea {
                anchors.fill: parent
                acceptedButtons: Qt.RightButton
                onClicked: {
                    activeChannel = myIndex
                }
            }
        }
    }
}
