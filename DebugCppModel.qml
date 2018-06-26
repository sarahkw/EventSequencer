import QtQuick 2.0
import QtQuick.Window 2.3

import eventsequencer 1.0 as ES

Window {
    title: "DebugCppModel"
    flags: Qt.Dialog

    ES.Document {
        id: document
    }

    ListView {
        anchors.fill: parent
        model: document
        delegate: Text {
            text: modelData.channel + ", " + modelData.startFrame + ", " + modelData.length
        }
    }
}
