import QtQuick 2.0
import QtQuick.Controls 2.2

import eventsequencer 1.0 as ES

ESTextField {
    property ES.Document document
    property bool shouldShowTime
    property int frame

    signal frameEditingFinished(int frame)

    function displayFrameNumber(frameNumber) {
        if (shouldShowTime) {
            return ES.FramesAndSeconds.framesToSeconds(
                document.framesPerSecond, frameNumber);
        } else {
            return frameNumber;
        }
    }

    text: displayFrameNumber(frame)

    onEsEditingFinished: {
        var result = ES.FramesAndSeconds.secondsToFrames(
                    document.framesPerSecond,
                    text,
                    !shouldShowTime /*numberOnlyIsFrames*/)
        var success = result[0]
        if (success) {
            frameEditingFinished(result[1])
        } else {
            frameChanged() // Reset
        }
    }
}
