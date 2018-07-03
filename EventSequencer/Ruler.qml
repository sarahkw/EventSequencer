import QtQuick 2.0
import "util.js" as Util

Rectangle {
    property ZoomLogic zoom
    property int xposition
    property var displayFrameNumber

    id: sbHoriz
    color: "whitesmoke"
    clip: true

    readonly property int initialIndex: -Math.ceil(xposition / sbHoriz.zoom.displayWidthPerRulerTick)
    readonly property int initialPosition: Util.negmod(xposition, sbHoriz.zoom.displayWidthPerRulerTick)

    Repeater {
        model: (sbHoriz.width /
                sbHoriz.zoom.displayWidthPerRulerTick +
                2 /*one for before, one for after*/
               )
        Item {
            property int myTickIndex: index + initialIndex
            property bool shouldShowNumber: zoom.tickIndexShouldShowNumber(myTickIndex)

            anchors.top: sbHoriz.top
            anchors.bottom: sbHoriz.bottom
            x: (initialPosition +
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
