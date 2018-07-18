import QtQuick 2.0
import QtQuick.Controls 2.2
import "../util.js" as Util

import "../" as Parent

Item {
    property Component channelPropertiesComponent: Component {
        Column {
            Label {
                text: "Content"
            }
            Item {
                anchors.left: parent.left
                anchors.right: parent.right
                height: sv.height
                Rectangle {
                    color: "white"
                    anchors.fill: sv
                }
                ScrollView {
                    id: sv
                    anchors.left: parent.left
                    anchors.right: parent.right
                    height: 200
                    TextArea {
                        onEditingFinished: cppChannel.content = text
                        text: cppChannel.content
                        selectByMouse: true
                    }
                }
            }
        }
    }

    property Component stripPropertiesComponent: Component {
        Column {
            Label {
                text: "Content"
            }
            TextField {
                anchors.left: parent.left
                anchors.right: parent.right
                onEditingFinished: Util.mutableobjset(cppStrip.mutableText, "content", text)
                text: Util.nvlobjprop(cppStrip.text, "content", "")
            }
        }
    }

    property Component stripComponent: Component {
        Rectangle {
            width: txt.width
            height: txt.height
            color: "whitesmoke"
            Text {
                id: txt
                color: selected ? "blue" : "black"
                text: Util.nvlobjprop(cppStrip.text, "content", '""')
                font: cmfuAlignedFont.builtFont
                elide: Text.ElideRight
                anchors.fill: parent
                verticalAlignment: Text.AlignVCenter

                MouseArea {
                    anchors.left: parent.left
                    anchors.top: parent.top
                    width: parent.contentWidth
                    height: parent.contentHeight
                    acceptedButtons: Qt.RightButton
                    onClicked: {
                        selectionClicked(mouse, Parent.SelectionMode.Whole)
                    }
                }
            }
        }
    }

    property Component channelTrackComponent: Component {
        Text {
            text: cppChannel.content
            font: cmfuAlignedFont.builtFont
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            verticalAlignment: Text.AlignVCenter
            width: zoom.mapLengthToDisplayWidth(cppChannel.content.length) // One frame per letter
            elide: Text.ElideRight
        }
    }
}
