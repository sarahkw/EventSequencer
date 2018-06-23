import QtQuick 2.0

QtObject {
    // When zoomLevel is positive, it will be the number of pixels that map
    // to each visual unit. When zoomLevel is zero or negative, the abs value
    // will be the number of ADDITIONAL pixels.
    property int zoomLevel: 0;

    // Small ticks are unlabeled ticks. Large ticks show their numbers.
    property int smallTicksPerLargeTick: 5
    property int minPixelsBetweenLargeTicks: 60

    property int realUnitsPerSmallTick: (function () {
        if (zoomLevel > 0) {
            return displayWidthPerRulerTick * zoomLevel;
        } else {
            return 1;
        }
    })()

    // INTERFACE

    function mapFrameToDisplayX(frame) {
        if (zoomLevel > 0) {
            return Math.floor(frame / zoomLevel);
        } else {
            return frame * displayWidthPerRulerTick;
        }
    }

    function mapLengthToDisplayWidth(length) {
        return mapFrameToDisplayX(length)
    }

    function mapDisplayWidthToFullFrames(displayWidth) {
        if (zoomLevel > 0) {
            return displayWidth * zoomLevel
        } else {
            return displayWidth / displayWidthPerRulerTick
        }
    }

    function zoom(deltaZoomLevel) {
        zoomLevel -= deltaZoomLevel
    }

    property int displayWidthPerRulerTick: (function () {
        var r = Math.ceil(minPixelsBetweenLargeTicks / smallTicksPerLargeTick)
        if (zoomLevel > 0) {
            // NOP
        } else {
            r += Math.abs(zoomLevel)
        }

        return r
    })()

    function tickIndexToNumber(tickIndex) {
        return tickIndex * realUnitsPerSmallTick
    }

    function tickIndexShouldShowNumber(tickIndex) {
        return tickIndex % smallTicksPerLargeTick == 0
    }
}
