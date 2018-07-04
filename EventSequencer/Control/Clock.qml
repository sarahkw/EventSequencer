import QtQuick 2.0
import eventsequencer 1.0 as ES

Item {
    property var roles: ["clock"]
    property ES.Document document
    property bool isPlaying: false
    property int currentFrame: 0
    signal changeFrame(int newFrame)

    Timer {
        id: t
        running: document ? (isPlaying && currentFrame < document.endFrame) : running
        interval: document ? (1000 / document.framesPerSecond) : interval
        onTriggered: {
            changeFrame(currentFrame + 1)
        }
    }
}
