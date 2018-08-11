import QtQuick 2.0
import QtQuick.Controls 2.3

import eventsequencer 1.0 as ES

import "Control" as Control

Rectangle {
    id: cPanel
    property int channelPixels
    property int yposition
    property ES.Document doc

    property int activeChannelPosition: 0

    color: "white"

    ScrollHelper {
        id: sh
        itemSize: channelPixels
        scrollbarSize: cPanel.height
        currentPosition: yposition
    }

    ES.CountChannelIndex {
        id: cci
        document: doc
        startAtPosition: sh.initialIndex
        count: sh.itemsToRender
    }

    Repeater {
        model: cci.model

        Item {
            property int myPosition: index + sh.initialIndex
            property ES.WaitFor myWait: doc.waitForChannelPosition(myPosition)
            property var cppChannel: myWait.result

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

            Text {
                id: txtIndex
                anchors.left: parent.left
                anchors.leftMargin: 5
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                verticalAlignment: Text.AlignVCenter
                font.pointSize: 7
                text: modelData.toPathString()
            }

            ChannelTypeComboBox {
                anchors.leftMargin: 5
                anchors.rightMargin: 10
                anchors.left: txtIndex.right
                anchors.right: selectIndicator.left
                anchors.verticalCenter: parent.verticalCenter

                currentChannelType: cppChannel !== null ? cppChannel.channelType : ES.ChannelType.UNSET
                onUpdateChannelType: {
                    if (newChannelType === ES.ChannelType.UNSET) {
                        if (cppChannel !== null) {
                            doc.deleteChannelByPosition(myPosition)
                        }
                    } else if (cppChannel === null || cppChannel.channelType !== newChannelType) {
                        doc.createChannelByPosition(myPosition, newChannelType)
                    }
                }
                onFocusChanged: focus = false
            }

            Rectangle {
                id: selectIndicator
                readonly property bool amActive: activeChannelPosition == myPosition
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
                    activeChannelPosition = myPosition
                }
            }
        }
    }
}
