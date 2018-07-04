import QtQuick 2.0
import QtQuick.Controls 2.2

Item {
    property var roles: []
    property Component stripPropertiesComponent: Component {
        Column {
            Item {
                width: 1
                height: 15
            }

            Text {
                text: "BadJS to be eval()'ed:"
            }
            TextArea {
                anchors.left: parent.left
                anchors.right: parent.right
                id: ta
                selectByMouse: true
                background: Rectangle {
                    color: "white"
                }
                wrapMode: TextEdit.Wrap
                onEditingFinished: {
                    console.info("TODO: Commit")
                }
            }
        }
    }
}
