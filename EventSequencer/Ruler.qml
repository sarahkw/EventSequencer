import QtQuick 2.0

Rectangle {
    property ZoomLogic zoom
    property int xposition
    property var displayFrameNumber

    id: sbHoriz
    color: "whitesmoke"
    clip: true

    ScrollHelper {
        id: sh
        itemSize: sbHoriz.zoom.displayWidthPerRulerTick
        scrollbarSize: sbHoriz.width
        currentPosition: xposition
    }

    Repeater {
        model: sh.itemsToRender
        Item {
            property int myTickIndex: index + sh.initialIndex
            property bool shouldShowNumber: zoom.tickIndexShouldShowNumber(myTickIndex)

            anchors.top: sbHoriz.top
            anchors.bottom: sbHoriz.bottom
            x: (sh.initialPosition +
                index * sbHoriz.zoom.displayWidthPerRulerTick)
            Rectangle {
                width: 1
                color: "black"
                anchors.top: parent.top
                anchors.bottom: texty.top
                anchors.bottomMargin: shouldShowNumber ? 0 : 3
            }
            Text {
                visible: shouldShowNumber
                id: texty
                font.pixelSize: 10
                anchors.horizontalCenter: parent.left
                text: (shouldShowNumber ?
                       displayFrameNumber(zoom.tickIndexToNumber(myTickIndex),
                                          !zoom.isVeryZoomedIn /*hideFramesWhenHoursShown*/) :
                       "")
                anchors.bottom: parent.bottom
            }
        }
    }
}
