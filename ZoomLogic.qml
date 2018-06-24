import QtQuick 2.0

QtObject {
    // When zoomLevel is zero or negative, the abs value will be the
    // number of ADDITIONAL pixels. When zoomLevel is positive, it
    // will be the focus number.
    property int zoomLevel: 0;

    property int rulerTicksPerNumberTick: 5
    property int minPixelsBetweenNumberTicks: 60
    property int numberTicksPerFocusNumber: 2
    property int rulerTicksPerFocusNumber: numberTicksPerFocusNumber * rulerTicksPerNumberTick


    // Static
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

    // Static
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

    function _framesPerRulerTick() {
        console.assert(zoomLevel > 0);
        var focusNumber = zoomLevel;
        return focusNumber / rulerTicksPerFocusNumber;
    }

    // INTERFACE

    function mapFrameToDisplayX(frame) {
        if (zoomLevel > 0) {
            return Math.floor(
                frame / _framesPerRulerTick() * displayWidthPerRulerTick);
        } else {
            return frame * displayWidthPerRulerTick;
        }
    }

    function mapLengthToDisplayWidth(length) {
        return mapFrameToDisplayX(length)
    }

    function mapDisplayWidthToFullFrames(displayWidth) {
        if (zoomLevel > 0) {
            return Math.floor(
                displayWidth / displayWidthPerRulerTick * _framesPerRulerTick());
        } else {
            return displayWidth / displayWidthPerRulerTick
        }
    }

    function zoom(deltaZoomLevel) {
        if (deltaZoomLevel === 0) return

        deltaZoomLevel = deltaZoomLevel < 0 ? -1 : 1
        // Temp hack: Negative should mean zoom in.
        deltaZoomLevel *= -1

        var isZoomIn = deltaZoomLevel < 0
        var isZoomOut = deltaZoomLevel > 0

        var initialZoomOut = (zoomLevel == 0 && isZoomOut)
        if (zoomLevel > 0 || initialZoomOut) {
            var specialFocusNumber = 10; // TODO Don't hardcode!

            // Copy so intermediate values don't fire signals.
            var tmpZoomLevel = zoomLevel;

            var direction = isZoomOut ? 1 : -1;
            if (initialZoomOut) {
                tmpZoomLevel = nextFocusNumber(specialFocusNumber, direction)
            } else {
                tmpZoomLevel = nextFocusNumber(tmpZoomLevel, direction)
            }

            if (tmpZoomLevel < specialFocusNumber) {
                // Gracefully degrade to trivial-zoom
                tmpZoomLevel = 0
            }

            zoomLevel = tmpZoomLevel
        } else {
            if (isZoomIn) {
                zoomLevel--
            } else {
                zoomLevel++
            }
        }
    }

    property int displayWidthPerRulerTick: (function () {
        var r = minPixelsBetweenNumberTicks / rulerTicksPerNumberTick

        // XXX Didn't yet think about what happens to rounding when
        //     ruler ticks aren't full pixels.
        console.assert(r === Math.floor(r))

        if (zoomLevel > 0) {
            // NOP
        } else {
            r += Math.abs(zoomLevel)
        }

        return r
    })()

    function tickIndexToNumber(tickIndex) {
        if (zoomLevel > 0) {
            var focusNumber = zoomLevel;
            return focusNumber / rulerTicksPerFocusNumber * tickIndex;
        } else {
            return tickIndex;
        }
    }

    function tickIndexShouldShowNumber(tickIndex) {
        return tickIndex % rulerTicksPerNumberTick == 0
    }
}
