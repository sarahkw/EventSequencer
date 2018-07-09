import QtQuick 2.0

Item {
    property Component clockComponent: Component {
        Item {
            Timer {
                // Plz only change isPlaying when the user clicks the button.
                property var isPlayingDup: isPlaying

                id: t
                running: document ? (isPlaying && currentFrame < document.endFrame) : running
                interval: document ? (1000 / document.framesPerSecond) : interval
                onIsPlayingDupChanged: {
                    if (isPlayingDup) {
                        // The frame at the cursor when Play is pressed should
                        // be played.
                        playFrame(currentFrame)
                    }
                }
                onTriggered: {
                    playFrame(currentFrame + 1)
                }
            }
        }
    }
}
