import QtQuick 2.0
import QtQuick.Window 2.3

Window {
    id: win
    property var model

    ListView {
        anchors.fill: parent
        model: win.model
        delegate: Row {
            Text {
                text: channelIndex
                width: 50
            }
            Text {
                text: modelData + ""
            }
        }
    }

}
