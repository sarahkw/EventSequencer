import QtQuick 2.0
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3

Item {
    property Component channelPropertiesComponent: Component {
        GridLayout {
            anchors.left: parent.left
            anchors.right: parent.right
            columns: 2

            Label { text: "Channel From" }
            TextField {
                Layout.fillWidth: true
                onEditingFinished: cppChannel.channelFrom = parseInt(text, 10)
                text: cppChannel.channelFrom
            }

            Label { text: "Channel To" }
            TextField {
                Layout.fillWidth: true
                onEditingFinished: cppChannel.channelTo = parseInt(text, 10)
                text: cppChannel.channelTo
            }
        }
    }
}
