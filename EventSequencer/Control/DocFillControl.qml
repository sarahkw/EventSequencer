import QtQuick 2.0
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import "../" as Parent
import eventsequencer 1.0 as ES

Item {
    property Component channelPropertiesComponent: Component {
        GridLayout {
            anchors.left: parent.left
            anchors.right: parent.right
            columns: 2

            Label { text: "Text Channel" }
            Parent.ESTextField {
                Layout.fillWidth: true
                onEsEditingFinished: cppChannel.textChannel = ES.ChannelIndexFactory.makeFromPathString(text)
                text: cppChannel !== null ? cppChannel.textChannel.toPathString() : ""
            }

            Label { text: "Resource Channel" }
            Parent.ESTextField {
                Layout.fillWidth: true
                onEsEditingFinished: cppChannel.resourceChannel = ES.ChannelIndexFactory.makeFromPathString(text)
                text: cppChannel !== null ? cppChannel.resourceChannel.toPathString() : ""
            }

            CheckBox {
                Layout.fillWidth: true
                Layout.columnSpan: 2
                text: "Enable developer mode"
                checked: cppChannel !== null ? cppChannel.advancedFeaturesEnabled : false
                onToggled: cppChannel.advancedFeaturesEnabled = checked
                Component.onCompleted: contentItem.wrapMode = Text.WordWrap
            }

            CheckBox {
                Layout.fillWidth: true
                Layout.columnSpan: 2
                text: "Expand resource channel for overlapping strips"
                checked: cppChannel !== null ? cppChannel.attemptExpansionOfResourceChannel : false
                onToggled: cppChannel.attemptExpansionOfResourceChannel = checked
                Component.onCompleted: contentItem.wrapMode = Text.WordWrap
            }
        }
    }

    property Component docFillPropertiesComponent: Component {
        GridLayout {
            anchors.left: parent.left
            anchors.right: parent.right
            columns: 2

            CheckBox {
                Layout.fillWidth: true
                Layout.columnSpan: 2
                text: "Enable developer mode"
                checked: cppChannel !== null ? cppChannel.advancedFeaturesEnabled : false
                onToggled: cppChannel.advancedFeaturesEnabled = checked
                Component.onCompleted: contentItem.wrapMode = Text.WordWrap
            }

            CheckBox {
                Layout.fillWidth: true
                Layout.columnSpan: 2
                text: "Expand resource channel for overlapping strips"
                checked: cppChannel !== null ? cppChannel.attemptExpansionOfResourceChannel : false
                onToggled: cppChannel.attemptExpansionOfResourceChannel = checked
                Component.onCompleted: contentItem.wrapMode = Text.WordWrap
            }
        }
    }
}
