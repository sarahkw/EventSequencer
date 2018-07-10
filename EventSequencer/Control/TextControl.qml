import QtQuick 2.0
import QtQuick.Controls 2.2

Item {
    property Component channelPropertiesComponent: Component {
        Text {
            text: "Hello from Text properties!"
        }
    }

    property Component stripPropertiesComponent: Component {
        Column {
            Label {
                text: "Content"
            }
            TextField {
                anchors.left: parent.left
                anchors.right: parent.right
                onEditingFinished: {
                    var savtext = text
                    cppStrip.mutableText().content = savtext
                }
                text: cppStrip.text !== null ? cppStrip.text.content : ""
            }
        }
    }

    property Component stripComponent: Component {
        Text {
            text: cppStrip.text !== null ? cppStrip.text.content : ""
        }
    }
}
