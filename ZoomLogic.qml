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

    function numberToScale(num) {
        // Not going to use Math.log because we will no longer be working with
        // discrete values and will have to worry about rounding and accuracy.
        var exponent = 0
        while (num >= 10) {
            exponent++
            num /= 10
        }
        return Math.pow(10, exponent)
    }

    function nextFocusNumber(currentFocusNumber, direction) {
        var myScale = numberToScale(currentFocusNumber)
        var myMultiplier = currentFocusNumber / myScale
        if (direction > 0) {
            if (currentFocusNumber >= myScale * 5) {
                myScale *= 10
                myMultiplier = 1
            } else {
                myMultiplier = 5
            }
        } else {
            if (currentFocusNumber >= myScale * 5) {
                myMultiplier = 1
            } else {
                myScale /= 10
                myMultiplier = 5
            }
        }

        return myScale * myMultiplier
    }

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
