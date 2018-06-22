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

    // TEMPORARY for migration
    property int tempTickSize: framePixels
}
