import QtQuick 2.0
import QtQuick.Controls 2.3

import eventsequencer 1.0 as ES

import "Control" as Control

Rectangle {
    id: cPanel
    property int channelPixels
    property int yposition
    property ES.Document doc

    property int activeChannel: 0

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
            property ES.WaitFor myWait: doc.waitForChannel(myIndex)
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

            ComboBox {
                // TODO move modelUtil to Controls/
                readonly property var modelUtil: (function () { return {
                    model: ["", "BadClock", "BadJs", "Text", "BadAudio", "Label", "Collate"],
                    enumToIndex: function (v) {
                        switch (v) {
                        case ES.ChannelType.BadClock: return 1;
                        case ES.ChannelType.BadJs: return 2;
                        case ES.ChannelType.Text: return 3;
                        case ES.ChannelType.BadAudio: return 4;
                        case ES.ChannelType.Label: return 5;
                        case ES.ChannelType.Collate: return 6;
                        }
                    },
                    indexToEnum: function (v) {
                        switch (v) {
                        case 1: return ES.ChannelType.BadClock;
                        case 2: return ES.ChannelType.BadJs;
                        case 3: return ES.ChannelType.Text;
                        case 4: return ES.ChannelType.BadAudio;
                        case 5: return ES.ChannelType.Label;
                        case 6: return ES.ChannelType.Collate;
                        }
                    },
                }})()

                anchors.leftMargin: 10
                anchors.rightMargin: 10
                anchors.left: parent.left
                anchors.right: selectIndicator.left
                anchors.verticalCenter: parent.verticalCenter
                model: modelUtil.model
                currentIndex: cppChannel !== null ? modelUtil.enumToIndex(cppChannel.channelType) : 0

                onCurrentIndexChanged: {
                    if (currentIndex === 0) {
                        if (cppChannel !== null) {
                            doc.deleteChannel(myIndex)
                        }
                    } else if (cppChannel === null || modelUtil.enumToIndex(cppChannel.channelType) !== currentIndex) {
                        doc.createChannel(myIndex, modelUtil.indexToEnum(currentIndex))
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
