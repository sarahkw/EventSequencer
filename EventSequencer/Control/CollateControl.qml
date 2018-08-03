import QtQuick 2.0
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import "../" as Parent
import eventsequencer 1.0 as ES

Item {

    property bool docViewCanRender: true

    property Component channelPropertiesComponent: Component {
        GridLayout {
            anchors.left: parent.left
            anchors.right: parent.right
            columns: 2

            Label { text: "Channel From" }
            Parent.ESTextField {
                Layout.fillWidth: true
                onEsEditingFinished: cppChannel.channelFrom = parseInt(text, 10)
                text: cppChannel.channelFrom
            }

            Label { text: "Channel To" }
            Parent.ESTextField {
                Layout.fillWidth: true
                onEsEditingFinished: cppChannel.channelTo = parseInt(text, 10)
                text: cppChannel.channelTo
            }
        }
    }

    property Component channelTrackComponent: Component {
        Repeater {
            model: cppChannel.model
            Rectangle {
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                x: zoom.mapFrameToDisplayX(segmentStart)
                width: zoom.mapLengthToDisplayWidth(segmentLength)
                color: segmentTypeToColor(segmentType)[0]
            }
        }
    }

    property Component docViewRenderComponent: Component {
        Repeater {
            property var noGarbageCollectPlease: cppChannel.model.makeFilterModel(textOffset_, textData.length)
            model: noGarbageCollectPlease
            Rectangle {
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                anchors.topMargin: 1
                anchors.bottomMargin: 1
                x: (segmentStart - textOffset_) * widthPerCharacter
                width: segmentLength * widthPerCharacter
                property var colors: segmentTypeToColor(segmentType)
                border.color: colors[0]
                border.width: 1
                color: colors[1]
            }
        }
    }

    // Implementation
    function segmentTypeToColor(stype) {
        switch (stype + 0) {
        case ES.CollateChannel.SegmentType.Empty:    return ["black"    , "white"                     ];
        case ES.CollateChannel.SegmentType.Chosen:   return ["lime"     , Qt.lighter("lime"     , 1.8)];
        case ES.CollateChannel.SegmentType.Conflict: return ["red"      , Qt.lighter("red"      , 1.8)];
        }
        console.error("Invalid segment type", stype);
    }
}
