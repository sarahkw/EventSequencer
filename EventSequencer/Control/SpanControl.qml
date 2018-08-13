import QtQuick 2.0
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import "../" as Parent

Item {
    property Component channelPropertiesComponent: Component {
        GridLayout {
            anchors.left: parent.left
            anchors.right: parent.right
            columns: 2

            Label { text: "Count" }
            Parent.ESTextField {
                Layout.fillWidth: true
                onEsEditingFinished: cppChannel.count = parseInt(text, 10)
                text: cppChannel !== null ? cppChannel.count : ""
            }

            Label { text: "Default Type" }
            Parent.ChannelTypeComboBox {
                Layout.fillWidth: true
                currentChannelType: cppChannel !== null ? cppChannel.defaultChannelType : 0
                onUpdateChannelType: cppChannel.defaultChannelType = newChannelType
            }
        }
    }
}
