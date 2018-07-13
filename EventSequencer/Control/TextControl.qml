import QtQuick 2.0
import QtQuick.Controls 2.2
import "../util.js" as Util

import "../" as Parent

Item {
    property Component channelPropertiesComponent: Component {
        Text {
            text: "Hello from Text properties!"
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
                onEditingFinished: Util.mutableobjset(cppStrip, "mutableText", "content", text)
                text: Util.nvlobjprop(cppStrip.text, "content", "")
            }
        }
    }

    property Component stripComponent: Component {
        Text {
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
