import QtQuick 2.0
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import "../" as Parent
import eventsequencer 1.0 as ES

Item {

    property Component channelPropertiesComponent: Component {
        GridLayout {
            anchors.left: parent.left
            anchors.right: parent.right
            columns: 2

            Label { text: "Channel" }
            Parent.ESTextField {
                Layout.fillWidth: true
                onEsEditingFinished: cppChannel.channel = ES.ChannelIndexFactory.makeFromPathString(text)
                text: cppChannel !== null ? cppChannel.channel.toPathString() : ""
            }
        }
    }

    property Component channelTrackComponent: Component {
        Repeater {
            model: cppChannel !== null ? cppChannel.model : null
            Rectangle {
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                x: zoom.mapFrameToDisplayX(segmentStart)
                width: zoom.mapLengthToDisplayWidth(segmentLength)
                color: segmentTypeToColor(segmentType)[0]
                border.width: 1
                onWidthChanged: {
                    if (segmentLength === 0) {
                        // TODO Remove this once we're sure
                        console.warn("Segment length of 0 shouldn't happen!")
                    }
                }
            }
        }
    }

    property Component docViewRenderComponent: Component {
        Repeater {
            property var noGarbageCollectPlease:
                cppChannel !== null ? cppChannel.model.makeFilterModel(textOffset_, textData.length) : null
            model: noGarbageCollectPlease
            Rectangle {
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                anchors.topMargin: 1
                anchors.bottomMargin: 1
                x: (segmentStart - textOffset_) * widthPerCharacter
                width: segmentLength * widthPerCharacter
                property var colors: segmentTypeToColor(segmentType)
                color: colors[2] || "white"

                // Left border
                Rectangle {
                    width: 2
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom
                    anchors.left: parent.left
                    color: colors[1] || "white"
                    visible: colors[1] !== undefined
                }
            }
        }
    }

    // Implementation
    function segmentTypeToColor(stype) {
        switch (stype + 0) {
        case ES.CollateChannel.SegmentType.Empty:    return ["black"    , undefined  , "whitesmoke"                ];
        case ES.CollateChannel.SegmentType.Chosen:   return ["lime"     , "green"    , Qt.lighter("lime"     , 1.8)];
        case ES.CollateChannel.SegmentType.Conflict: return ["red"      , undefined  , Qt.lighter("red"      , 1.8)];
        }
        console.error("Invalid segment type", stype);
    }
}
