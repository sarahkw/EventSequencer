import QtQuick 2.0
import QtQuick.Controls 2.3
import "Control" as Control

Rectangle {
    id: cPanel
    property int channelPixels
    property int yposition

    property int activeChannel: 0

    // Temporary!
    property var channelToSelection: ({})
    property var channelToControl: ({})
    property var roleToControl: ({})

    color: "white"

    ScrollHelper {
        id: sh
        itemSize: channelPixels
        scrollbarSize: cPanel.height
        currentPosition: yposition
    }

    Component {
        id: badClockComponent
        Control.BadClock {
        }
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
                //model: ["", "Dummy", "BadClock", "Label", "JavaScript", "Sound", "Print"]
                model: ["", "BadClock", "BadJS", "DumbStrips"]
                currentIndex: channelToSelection[myIndex] === undefined ? 0 : channelToSelection[myIndex]
                onCurrentIndexChanged: {
                    if (channelToSelection[myIndex] !== currentIndex) {
                        channelToSelection[myIndex] = currentIndex
                        var derp = model[currentIndex]
                        if (derp === "BadClock") {
                            var newval = badClockComponent.createObject()
                            channelToControl[myIndex] = newval
                            var needFireSignal = false
                            newval.roles.forEach(function (role) {
                                roleToControl[role] = newval
                                needFireSignal = true
                            })
                            if (needFireSignal) {
                                var x = roleToControl
                                roleToControl = {}
                                roleToControl = x
                            }
                        } else {
                            // TODO: Unmap roles
                            delete channelToControl[myIndex]
                        }
                    }
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
