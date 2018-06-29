import QtQuick 2.0
import "util.js" as Util

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

        console.assert(num >= 1, "Numbers less than 1 not supported!");

        if (num === 1) {
            exponent = -1
        } else {
            while (num > 10) {
                exponent++
                num /= 10
            }
        }
        return Math.pow(10, exponent)
    }

    // Static
    function nextFactor(currentFactor, direction, division) {
        console.assert(currentFactor % division == 0,
                       "Current factor is invalid! (Not a factor.)");

        var firstValue = division;
        if (division === 1) {
            // If division is 1, our sequence is actually from 2 to 10.
            firstValue = 2;
        }

        if (direction > 0) {
            currentFactor += division;
            if (currentFactor > 10) {
                return [firstValue, true];
            } else {
                return [currentFactor, false];
            }
        } if (direction < 0) {
            currentFactor -= division;
            // TODO: I changed <= 0 to < firstValue and it passes tests.
            //       I'm drunk and don't know what I did.
            if (currentFactor < firstValue) {
                return [10, true];
            } else {
                return [currentFactor, false];
            }
        }
    }

    // Static
    function nextFocusNumber(currentFocusNumber, direction, division) {
        if (division === undefined) {
            division = 1
        }

        var scale = numberToScale(currentFocusNumber);
        var factor = currentFocusNumber / scale;
        var nfac = nextFactor(factor, direction, division);
        var nfac_num = nfac[0];
        var nfac_wrapped = nfac[1];

        if (direction > 0) {
            return scale * nfac_num * (nfac_wrapped ? 10 : 1);
        } else {
            return scale * nfac_num / (nfac_wrapped ? 10 : 1);
        }
    }

    function _framesPerRulerTick() {
        console.assert(zoomLevel > 0, "Shouldn't be here.");
        var focusNumber = zoomLevel;
        return focusNumber / rulerTicksPerFocusNumber;
    }

    // INTERFACE

    property bool isVeryZoomedIn: !(zoomLevel > 0)

    function mapFrameToDisplayX(frame) {
        if (zoomLevel > 0) {
            return Math.floor(
                frame / _framesPerRulerTick() * displayWidthPerRulerTick);
        } else {
            return frame * displayWidthPerRulerTick;
        }
    }

    function mapLengthToDisplayWidth(length) {
        if (zoomLevel > 0) {
            // Ceil it. I think it's better to not show any extra
            // space, then it is to not show a possible gap.
            //
            // I guess the best solution would be to forcefully add an
            // extra pixel spacing if there is a gap between adjacent
            // strips.
            return Math.ceil(
                length / _framesPerRulerTick() * displayWidthPerRulerTick);
        } else {
            return length * displayWidthPerRulerTick;
        }
    }

    function mapDisplayWidthToFrames(displayWidth) {
        if (zoomLevel > 0) {
            return displayWidth / displayWidthPerRulerTick * _framesPerRulerTick();
        } else {
            return displayWidth / displayWidthPerRulerTick
        }
    }

    function mapDisplayWidthToFullFrames(displayWidth) {
        return Util.trunc(mapDisplayWidthToFrames(displayWidth))
    }

    function zoom(deltaZoomLevel) {
        if (deltaZoomLevel === 0) return

        // Temp hack: One "action" per event
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
        console.assert(r === Math.floor(r),
                       "Ruler ticks need to be on full pixels");

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
