import QtQuick 2.0
import "util.js" as Util

Rectangle {
    property int tickSize
    property int position

    // TODO Change scale EARLIER than maxTickSize. Will have to pick
    // new numbers to show every time scale is changed, then.

    readonly property int pixelsPerTick: 2
    readonly property int minTickSize: 5
    readonly property int maxTickSize: minTickSize * minTickSize

    property var zoomCalculation: (function () {

        var _tickSize = tickSize;
        var _scale = 1;

        // TODO Eventually move towards a closed form calc, with Math.log
        while (_tickSize < minTickSize) {
            _tickSize += maxTickSize;
            _scale *= minTickSize;
        }

        return [_tickSize, _scale];
    })()
    property int pseudoTickSize: zoomCalculation[0] * pixelsPerTick
    property int pseudoScale: zoomCalculation[1]
    property int nextScale: pseudoScale * minTickSize

    id: sbHoriz
    color: "whitesmoke"
    clip: true

    Repeater {
        model: sbHoriz.width / sbHoriz.pseudoTickSize + 2 /*1 isn't enough. I don't know why, but 2 seems to work visually.*/
        Item {
            anchors.top: sbHoriz.top
            anchors.bottom: sbHoriz.bottom
            x: position % sbHoriz.pseudoTickSize + index * sbHoriz.pseudoTickSize
            Rectangle {
                width: 1
                color: "black"
                anchors.top: parent.top
                anchors.bottom: texty.top
            }
            Text {
                property int myNumber: (index - Util.trunc(position / sbHoriz.pseudoTickSize)) * pseudoScale
                visible: myNumber % nextScale == 0
                id: texty
                font.pixelSize: 10
                anchors.horizontalCenter: parent.left
                text: myNumber
                anchors.bottom: parent.bottom
            }
        }
    }
}
