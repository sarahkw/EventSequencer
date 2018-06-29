import QtQuick 2.0
import QtQuick.Controls 2.2

import eventsequencer 1.0 as ES

TextField {
    property ES.Document document
    property bool shouldShowTime
    property int frame

    function displayFrameNumber(frameNumber) {
        if (shouldShowTime) {
            return ES.FramesAndSeconds.framesToSeconds(
                document.framesPerSecond, frameNumber);
        } else {
            return frameNumber;
        }
    }

    text: displayFrameNumber(frame)

    onEditingFinished: {
        var result = ES.FramesAndSeconds.secondsToFrames(
                    document.framesPerSecond,
                    text,
                    !shouldShowTime /*numberOnlyIsFrames*/)
        var success = result[0]
        if (success) {
            text = "" // Stop the binding
            frame = result[1]
        }
        text = Qt.binding(function () { return displayFrameNumber(frame); });

        focus = false
    }
}
