import QtQuick 2.0

QtObject {
    property int framePixels: 50

    /* Interface */
    function mapFrameToDisplayX(frame) {
        return frame * framePixels
    }

    /* Interface */
    function mapLengthToDisplayWidth(length) {
        return length * framePixels
    }

    /* Interface */
    function mapDisplayWidthToFullFrames(displayWidth) {
        return Math.floor(displayWidth / framePixels)
    }

    /* Interface */
    function zoom(deltaZoomLevel) {
        framePixels += deltaZoomLevel
    }

    /* Interface */
    property int displayWidthPerRulerTick: pseudoTickSize

    /* Interface */
    function tickIndexToNumber(tickIndex) {
        return tickIndex * pseudoScale
    }

    /* Interface */
    function tickIndexShouldShowNumber(tickIndex) {
        return (tickIndex * pseudoScale) % nextScale == 0
    }

    // Old ruler logic

    readonly property int pixelsPerTick: 2
    readonly property int minTickSize: 5
    readonly property int maxTickSize: minTickSize * minTickSize

    property var zoomCalculation: (function () {

        var _tickSize = framePixels;
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
}
