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
                selectByMouse: true
                background: Rectangle {
                    color: "white"
                }
                wrapMode: TextEdit.Wrap
                onEditingFinished: {
                    var savtext = text // initBadJs will wipe out text otherwise
                    if (cppStrip.badJs === null) {
                        cppStrip.initBadJs()
                    }
                    cppStrip.badJs.script = savtext
                }
                text: cppStrip.badJs !== null ? cppStrip.badJs.script : ""
            }
        }
    }
}
