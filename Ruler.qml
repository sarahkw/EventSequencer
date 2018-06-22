import QtQuick 2.0
import "util.js" as Util

Rectangle {
    property ZoomOld zoom
    property int position

    id: sbHoriz
    color: "whitesmoke"
    clip: true

    Repeater {
        model: (sbHoriz.width /
                sbHoriz.zoom.displayWidthPerRulerTick +
                2 /*1 isn't enough. I don't know why, but 2 seems to work visually.*/
               )
        Item {
            anchors.top: sbHoriz.top
            anchors.bottom: sbHoriz.bottom
            x: (position % sbHoriz.zoom.displayWidthPerRulerTick +
                index * sbHoriz.zoom.displayWidthPerRulerTick)
            Rectangle {
                width: 1
                color: "black"
                anchors.top: parent.top
                anchors.bottom: texty.top
            }
            Text {
                // Tick index compensated for any panning done by the user.
                property int myTickIndex: (
                    index - Util.trunc(position / sbHoriz.zoom.displayWidthPerRulerTick))

                visible: zoom.tickIndexShouldShowNumber(myTickIndex)
                id: texty
                font.pixelSize: 10
                anchors.horizontalCenter: parent.left
                text: zoom.tickIndexToNumber(myTickIndex)
                anchors.bottom: parent.bottom
            }
        }
    }
}
