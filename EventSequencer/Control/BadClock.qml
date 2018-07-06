import QtQuick 2.0

Item {
    property Component clockComponent: Component {
        Item {
            Timer {
                id: t
                running: document ? (isPlaying && currentFrame < document.endFrame) : running
                interval: document ? (1000 / document.framesPerSecond) : interval
                onTriggered: {
                    changeFrame(currentFrame + 1)
                }
            }
        }
    }
}
