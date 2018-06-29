import QtQuick 2.0
import "util.js" as Util

Rectangle {
    property var zoom
    property int position
    property var displayFrameNumber

    id: sbHoriz
    color: "whitesmoke"
    clip: true

    Repeater {
        model: (sbHoriz.width /
                sbHoriz.zoom.displayWidthPerRulerTick +
                2 /*1 isn't enough. I don't know why, but 2 seems to work visually.*/
               )
        Item {
            // Tick index compensated for any panning done by the user.
            property int myTickIndex: (
                index - Util.trunc(position / sbHoriz.zoom.displayWidthPerRulerTick))
            property bool shouldShowNumber: zoom.tickIndexShouldShowNumber(myTickIndex)

            anchors.top: sbHoriz.top
            anchors.bottom: sbHoriz.bottom
            x: (position % sbHoriz.zoom.displayWidthPerRulerTick +
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
