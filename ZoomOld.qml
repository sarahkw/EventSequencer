import QtQuick 2.0

QtObject {
    property int framePixels: 50
    function mapFrameToDisplayX(frame) {
        return frame * framePixels
    }
    function mapLengthToDisplayWidth(length) {
        return length * framePixels
    }

    function mapDisplayWidthToFullFrames(displayWidth) {
        return _floorDiv(displayWidth, framePixels)
    }

    function zoom(deltaZoomLevel) {
        framePixels += deltaZoomLevel
    }

    function _floorDiv(a, b) {
        return Math.floor(a / b);
    }

    property int displayWidthPerRulerTick: pseudoTickSize

    function tickIndexToNumber(tickIndex) {
        return tickIndex * pseudoScale
    }

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
